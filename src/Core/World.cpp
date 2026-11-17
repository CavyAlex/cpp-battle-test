#include "World.hpp"

#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace sw::core
{
	World::World(EventLog& log, RandomPick randomPick) :
			_log(log),
			_randomPick(std::move(randomPick))
	{}

	void World::createMap(uint32_t width, uint32_t height)
	{
		if (_mapCreated)
		{
			throw std::runtime_error("Map is already created");
		}
		if (width == 0 || height == 0)
		{
			throw std::runtime_error("Map size must be positive");
		}
		_width = width;
		_height = height;
		_mapCreated = true;
		log(io::MapCreated{width, height});
	}

	bool World::hasMap() const
	{
		return _mapCreated;
	}

	bool World::isInside(Position cell) const
	{
		return cell.x < _width && cell.y < _height;
	}

	uint64_t World::getTick() const
	{
		return _tick;
	}

	void World::nextTick()
	{
		++_tick;
	}

	void World::spawn(std::unique_ptr<Unit> unit)
	{
		requireMap();
		const auto id = unit->getId();
		const auto sameId = [id](const auto& other)
		{
			return other->getId() == id;
		};
		if (std::any_of(_units.begin(), _units.end(), sameId))
		{
			throw std::runtime_error("Unit id is already taken: " + std::to_string(id));
		}
		const auto position = unit->getPosition();
		if (!isInside(position))
		{
			throw std::runtime_error("Cannot spawn unit " + std::to_string(id) + ": cell is outside the map");
		}
		if (!canStand(*unit, position))
		{
			throw std::runtime_error("Cannot spawn unit " + std::to_string(id) + ": cell is occupied");
		}
		log(io::UnitSpawned{id, unit->getType(), position.x, position.y});
		_units.push_back(std::move(unit));
	}

	Unit& World::findUnit(UnitId id)
	{
		for (auto& unit : _units)
		{
			if (unit->getId() == id)
			{
				return *unit;
			}
		}
		throw std::runtime_error("Unknown unit id: " + std::to_string(id));
	}

	const std::vector<std::unique_ptr<Unit>>& World::getUnits() const
	{
		return _units;
	}

	size_t World::getAliveCount() const
	{
		return std::count_if(_units.begin(), _units.end(), [](const auto& unit) { return unit->isAlive(); });
	}

	bool World::removeDead()
	{
		const auto removed = std::erase_if(_units, [](const auto& unit) { return !unit->isAlive(); });
		return removed > 0;
	}

	bool World::canStand(const Unit& unit, Position cell) const
	{
		if (!isInside(cell))
		{
			return false;
		}
		// Юнит, который не занимает клетку (Ворон, Мина), может стоять поверх других
		if (!unit.occupiesCell())
		{
			return true;
		}
		return !isOccupiedByOther(cell, unit);
	}

	bool World::isOccupiedByOther(Position cell, const Unit& unit) const
	{
		return std::any_of(
			_units.begin(),
			_units.end(),
			[&](const auto& other)
			{ return other.get() != &unit && other->occupiesCell() && other->getPosition() == cell; });
	}

	std::vector<Unit*> World::unitsWithin(Position center, uint32_t radius, const Unit* exclude) const
	{
		std::vector<Unit*> result;
		for (const auto& unit : _units)
		{
			if (unit.get() != exclude && unit->isAlive() && distance(center, unit->getPosition()) <= radius)
			{
				result.push_back(unit.get());
			}
		}
		return result;
	}

	std::vector<Unit*> World::findTargets(const Unit& attacker, AttackKind kind, AttackRange range) const
	{
		std::vector<Unit*> result;
		for (const auto& unit : _units)
		{
			// Без HP урон нанести нельзя — такой юнит не цель, даже если забыл отказать в incomingAttackRange
			if (unit.get() == &attacker || !unit->isAlive() || !unit->getHp())
			{
				continue;
			}
			const auto allowed = unit->incomingAttackRange(kind, range);
			const auto dist = distance(attacker.getPosition(), unit->getPosition());
			if (allowed && dist >= allowed->min && dist <= allowed->max)
			{
				result.push_back(unit.get());
			}
		}
		return result;
	}

	Unit* World::pickRandom(const std::vector<Unit*>& candidates)
	{
		if (candidates.empty())
		{
			return nullptr;
		}
		return candidates.at(_randomPick(candidates.size()));
	}

	void World::march(UnitId id, Position target)
	{
		requireMap();
		auto& unit = findUnit(id);
		if (!unit._movement)
		{
			throw std::runtime_error("Unit " + std::to_string(id) + " cannot move");
		}
		if (!isInside(target))
		{
			throw std::runtime_error("March target is outside the map for unit " + std::to_string(id));
		}
		const auto position = unit._position;
		log(io::MarchStarted{id, position.x, position.y, target.x, target.y});
		if (position == target)
		{
			log(io::MarchEnded{id, position.x, position.y});
			unit._movement->marchTarget.reset();
			return;
		}
		unit._movement->marchTarget = target;
	}

	void World::moveTo(Unit& unit, Position cell)
	{
		if (!unit._movement)
		{
			throw std::logic_error("Static unit " + std::to_string(unit._id) + " cannot move");
		}
		if (distance(unit._position, cell) != 1 || !canStand(unit, cell))
		{
			throw std::logic_error("Unit " + std::to_string(unit._id) + " cannot step to the requested cell");
		}
		unit._position = cell;
		log(io::UnitMoved{unit._id, cell.x, cell.y});
		if (unit._movement->marchTarget == cell)
		{
			log(io::MarchEnded{unit._id, cell.x, cell.y});
			unit._movement->marchTarget.reset();
		}
	}

	void World::changeHp(Unit& target, uint32_t hp)
	{
		if (!target._hp || !target.isAlive())
		{
			throw std::logic_error("Unit " + std::to_string(target._id) + " has no HP to change or is already dead");
		}
		target._hp = hp;
		if (hp == 0)
		{
			log(io::UnitDied{target._id});
		}
	}

	void World::requireMap() const
	{
		if (!_mapCreated)
		{
			throw std::runtime_error("Map is not created: CREATE_MAP must be the first command");
		}
	}
}
