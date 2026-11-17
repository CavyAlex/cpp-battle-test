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
	// Выбор случайного индекса из [0, n). В тестах подменяется заглушкой
	using RandomPick = std::function<size_t(size_t n)>;

	// Состояние симуляции и единственная точка его изменения.
	// Некорректные команды (вне карты, занятая клетка, неизвестный id) — исключение std::runtime_error.
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

		// В порядке создания, включая мёртвых до уборки в конце хода
		const std::vector<std::unique_ptr<Unit>>& getUnits() const;
		size_t getAliveCount() const;

		// Убирает мёртвых с поля. Возвращает true, если кого-то убрали
		bool removeDead();

		// Внутри поля и, если юнит занимает клетку, её не занимает никто другой
		bool canStand(const Unit& unit, Position cell) const;

		// Живые юниты, кроме exclude, на расстоянии не больше radius от center
		std::vector<Unit*> unitsWithin(Position center, uint32_t radius, const Unit* exclude) const;

		// Живые юниты с HP, кроме attacker, по которым проходит атака kind с дистанцией range.
		// Итоговую дистанцию для каждого кандидата задаёт его Unit::incomingAttackRange
		std::vector<Unit*> findTargets(const Unit& attacker, AttackKind kind, AttackRange range) const;

		// nullptr, если кандидатов нет
		Unit* pickRandom(const std::vector<Unit*>& candidates);

		void march(UnitId id, Position target);
		void moveTo(Unit& unit, Position cell);
		// Единственный способ изменить HP. Событие самого действия («атаковал», «вылечил») пишет действие,
		// а UNIT_DIED при нуле — World. Юниту без HP или уже мёртвому — std::logic_error
		void changeHp(Unit& target, uint32_t hp);

		// Для событий, которые заводят Features
		template <class TEvent>
		void log(TEvent&& event)
		{
			_log.log(_tick, std::forward<TEvent>(event));
		}

	private:
		void requireMap() const;

		// Клетку занимает кто-то, кроме unit. Юниты, которые не занимают клетку, не в счёт
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
