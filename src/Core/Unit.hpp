#pragma once

#include "Position.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace sw::core
{
	class World;

	using UnitId = uint32_t;

	enum class AttackKind
	{
		Melee,
		Ranged
	};

	// Дистанция атаки в клетках, границы включительно
	struct AttackRange
	{
		uint32_t min{};
		uint32_t max{};
	};

	struct Movement
	{
		uint32_t speed{1};
		std::optional<Position> marchTarget;
	};

	class Unit
	{
	public:
		virtual ~Unit() = default;

		Unit(const Unit&) = delete;
		Unit& operator=(const Unit&) = delete;

		UnitId getId() const;
		const std::string& getType() const;
		Position getPosition() const;
		bool occupiesCell() const;

		const std::optional<uint32_t>& getHp() const;

		// nullopt — статичный юнит
		const std::optional<Movement>& getMovement() const;

		// false — HP кончилось или юнит ушёл с поля
		bool isAlive() const;
		void leaveMap();

		// true, если юнит что-то сделал
		virtual bool act(World& world) = 0;

		// nullopt — атаку не пускает, иначе дистанция атаки по этому юниту
		virtual std::optional<AttackRange> incomingAttackRange(AttackKind kind, AttackRange range) const;

	protected:
		Unit(
			UnitId id,
			std::string type,
			Position position,
			std::optional<uint32_t> hp,
			std::optional<Movement> movement,
			bool occupiesCell = true);

		void requirePositive(uint32_t value, const char* field) const;
		void requireAtLeast(uint32_t value, uint32_t min, const char* field) const;

	private:
		// Позицию, марш и HP меняет только World, чтобы каждое изменение попадало в лог
		friend class World;

		UnitId _id;
		std::string _type;
		Position _position;
		std::optional<uint32_t> _hp;
		std::optional<Movement> _movement;
		bool _occupiesCell;
		bool _leftMap{false};
	};
}
