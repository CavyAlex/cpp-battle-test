#include "Unit.hpp"

#include <stdexcept>
#include <utility>

namespace sw::core
{
	Unit::Unit(
		UnitId id,
		std::string type,
		Position position,
		std::optional<uint32_t> hp,
		std::optional<Movement> movement,
		bool occupiesCell) :
			_id(id),
			_type(std::move(type)),
			_position(position),
			_hp(hp),
			_movement(std::move(movement)),
			_occupiesCell(occupiesCell)
	{
		if (_hp)
		{
			requirePositive(*_hp, "hp");
		}
	}

	void Unit::requirePositive(uint32_t value, const char* field) const
	{
		if (value == 0)
		{
			throw std::runtime_error(_type + " " + std::to_string(_id) + ": " + field + " must be positive");
		}
	}

	void Unit::requireAtLeast(uint32_t value, uint32_t min, const char* field) const
	{
		if (value < min)
		{
			throw std::runtime_error(
				_type + " " + std::to_string(_id) + ": " + field + " must be at least " + std::to_string(min));
		}
	}

	UnitId Unit::getId() const
	{
		return _id;
	}

	const std::string& Unit::getType() const
	{
		return _type;
	}

	Position Unit::getPosition() const
	{
		return _position;
	}

	bool Unit::occupiesCell() const
	{
		return _occupiesCell;
	}

	const std::optional<uint32_t>& Unit::getHp() const
	{
		return _hp;
	}

	const std::optional<Movement>& Unit::getMovement() const
	{
		return _movement;
	}

	bool Unit::isAlive() const
	{
		return !_leftMap && (!_hp || *_hp > 0);
	}

	void Unit::leaveMap()
	{
		_leftMap = true;
	}

	std::optional<AttackRange> Unit::incomingAttackRange(AttackKind, AttackRange range) const
	{
		return range;
	}
}
