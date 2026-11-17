#pragma once

#include <Core/Unit.hpp>
#include <Core/World.hpp>
#include <IO/System/EventLog.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace sw::test
{
	// Свойства тестового юнита: по умолчанию обычный ходячий юнит с 10 HP
	struct StubOptions
	{
		std::optional<uint32_t> hp = 10;
		std::optional<core::Movement> movement = core::Movement{};
		bool occupiesCell = true;
		std::function<std::optional<core::AttackRange>(core::AttackKind, core::AttackRange)> incomingAttackRange;
	};

	// Юнит без собственного поведения: нужен, чтобы проверять Core на юнитах с разными свойствами
	class StubUnit final : public core::Unit
	{
	public:
		StubUnit(core::UnitId id, core::Position position, StubOptions options) :
				Unit(id, "Stub", position, options.hp, options.movement, options.occupiesCell),
				_incomingAttackRange(std::move(options.incomingAttackRange))
		{}

		bool act(core::World&) override
		{
			return false;
		}

		std::optional<core::AttackRange> incomingAttackRange(
			core::AttackKind kind, core::AttackRange range) const override
		{
			return _incomingAttackRange ? _incomingAttackRange(kind, range) : Unit::incomingAttackRange(kind, range);
		}

	private:
		std::function<std::optional<core::AttackRange>(core::AttackKind, core::AttackRange)> _incomingAttackRange;
	};

	// Мир для теста: события перехватываются из stdout, случайный выбор заменён заглушкой
	class TestWorld
	{
	public:
		explicit TestWorld(uint32_t width = 10, uint32_t height = 10) :
				_previousOutput(std::cout.rdbuf(_output.rdbuf())),
				world(
					_log,
					[this](size_t n)
					{
						pickCalls.push_back(n);
						return pickResult;
					})
		{
			world.createMap(width, height);
			clearEvents();
		}

		~TestWorld()
		{
			std::cout.rdbuf(_previousOutput);
		}

		TestWorld(const TestWorld&) = delete;
		TestWorld& operator=(const TestWorld&) = delete;

		StubUnit& addStub(core::UnitId id, core::Position position, StubOptions options = {})
		{
			return add(std::make_unique<StubUnit>(id, position, std::move(options)));
		}

		// Событие UNIT_SPAWNED из лога убирается: тест видит только события проверяемого действия
		template <class TUnit>
		TUnit& add(std::unique_ptr<TUnit> unit)
		{
			auto& reference = *unit;
			world.spawn(std::move(unit));
			clearEvents();
			return reference;
		}

		// Строки лога без пробелов в конце
		std::vector<std::string> events() const
		{
			std::vector<std::string> lines;
			std::istringstream stream(_output.str());
			for (std::string line; std::getline(stream, line);)
			{
				line.erase(line.find_last_not_of(' ') + 1);
				lines.push_back(line);
			}
			return lines;
		}

		void clearEvents()
		{
			_output.str("");
		}

	private:
		std::ostringstream _output;
		std::streambuf* _previousOutput;
		EventLog _log;

	public:
		std::vector<size_t> pickCalls;
		size_t pickResult = 0;
		core::World world;
	};
}
