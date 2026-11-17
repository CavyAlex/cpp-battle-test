#pragma once

#include "Position.hpp"
#include "Unit.hpp"

#include <IO/System/EventLog.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace sw::core
{
	// Индекс из [0, n); в тестах подменяется заглушкой
	using RandomPick = std::function<size_t(size_t n)>;

	// Ошибки сценария — std::runtime_error, нарушение правил мира из кода Features — std::logic_error
	class World
	{
	public:
		World(EventLog& log, RandomPick randomPick);

		void createMap(uint32_t width, uint32_t height);
		bool hasMap() const;
		bool isInside(Position cell) const;

		uint64_t getTick() const;
		void nextTick();

		void spawn(std::unique_ptr<Unit> unit);
		Unit& findUnit(UnitId id);

		// В порядке создания, включая погибших до уборки в конце хода
		const std::vector<std::unique_ptr<Unit>>& getUnits() const;
		size_t getAliveCount() const;

		// true, если кого-то убрали
		bool removeDead();

		bool canStand(const Unit& unit, Position cell) const;

		std::vector<Unit*> unitsWithin(Position center, uint32_t radius, const Unit* exclude) const;

		// С учётом incomingAttackRange цели; юниты без HP целями не бывают
		std::vector<Unit*> findTargets(const Unit& attacker, AttackKind kind, AttackRange range) const;

		Unit* pickRandom(const std::vector<Unit*>& candidates);

		void march(UnitId id, Position target);
		void moveTo(Unit& unit, Position cell);
		// UNIT_DIED при нуле пишет World, событие самого действия — вызывающий
		void changeHp(Unit& target, uint32_t hp);

		template <class TEvent>
		void log(TEvent&& event)
		{
			_log.log(_tick, std::forward<TEvent>(event));
		}

	private:
		void requireMap() const;

		bool isOccupiedByOther(Position cell, const Unit& unit) const;

		EventLog& _log;
		RandomPick _randomPick;
		uint32_t _width{};
		uint32_t _height{};
		bool _mapCreated{false};
		uint64_t _tick{1};
		std::vector<std::unique_ptr<Unit>> _units;
	};
}
