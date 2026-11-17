#include <TestFramework.hpp>
#include <TestWorld.hpp>
#include <stdexcept>

using namespace sw;
using namespace sw::test;

namespace
{
	std::optional<core::AttackRange> refuseAll(core::AttackKind, core::AttackRange)
	{
		return std::nullopt;
	}

	// Как будущий Ворон: ближним боем не достать, дальний бой — на дистанции на 1 меньше
	std::optional<core::AttackRange> flying(core::AttackKind kind, core::AttackRange range)
	{
		if (kind == core::AttackKind::Melee)
		{
			return std::nullopt;
		}
		return core::AttackRange{range.min - 1, range.max - 1};
	}
}

// Выбор целей

TEST(T1_unitKilledThisTurnIsNotATarget)
{
	TestWorld t;
	auto& attacker = t.addStub(1, {1, 1});
	auto& dying = t.addStub(2, {0, 1}, {.hp = 5});
	auto& alive = t.addStub(3, {2, 1});
	t.world.changeHp(dying, 0);

	CHECK(t.world.findTargets(attacker, core::AttackKind::Melee, {1, 1}) == std::vector<core::Unit*>{&alive});
}

TEST(T2_unitRefusingAllAttacksIsNeverATarget)
{
	TestWorld t;
	auto& attacker = t.addStub(1, {0, 0});
	t.addStub(2, {1, 0}, {.incomingAttackRange = refuseAll});

	CHECK(t.world.findTargets(attacker, core::AttackKind::Melee, {1, 1}).empty());
	CHECK(t.world.findTargets(attacker, core::AttackKind::Ranged, {1, 5}).empty());
}

TEST(T3_targetCanShiftRangedDistance)
{
	TestWorld t;
	auto& attacker = t.addStub(1, {0, 0});
	auto& near = t.addStub(2, {1, 0}, {.incomingAttackRange = flying});
	t.addStub(3, {4, 0}, {.incomingAttackRange = flying});

	CHECK(t.world.findTargets(attacker, core::AttackKind::Ranged, {2, 4}) == std::vector<core::Unit*>{&near});
	CHECK(t.world.findTargets(attacker, core::AttackKind::Melee, {1, 1}).empty());
}

TEST(T4_noCandidatesMeansNoRandomPick)
{
	TestWorld t;

	CHECK(t.world.pickRandom({}) == nullptr);
	CHECK(t.pickCalls.empty());
}

TEST(T5_outOfRangeRandomIndexThrows)
{
	TestWorld t;
	auto& first = t.addStub(1, {0, 0});
	auto& second = t.addStub(2, {1, 0});
	t.pickResult = 5;

	CHECK_THROWS(t.world.pickRandom({&first, &second}), std::out_of_range);
}

TEST(T6_unitWithoutHpIsNeverATarget)
{
	TestWorld t;
	auto& attacker = t.addStub(1, {0, 0});
	t.addStub(2, {1, 0}, {.hp = std::nullopt});

	CHECK(t.world.findTargets(attacker, core::AttackKind::Melee, {1, 1}).empty());
	CHECK(t.world.findTargets(attacker, core::AttackKind::Ranged, {1, 5}).empty());
}

// Перемещение и марш

TEST(M1_staticUnitCannotMarch)
{
	TestWorld t;
	t.addStub(1, {0, 0}, {.movement = std::nullopt});

	CHECK_THROWS(t.world.march(1, {3, 3}), std::runtime_error);
	CHECK(t.events().empty());
}

TEST(M2_staticUnitCannotMove)
{
	TestWorld t;
	auto& unit = t.addStub(1, {0, 0}, {.movement = std::nullopt});

	CHECK_THROWS(t.world.moveTo(unit, {1, 0}), std::logic_error);
}

TEST(M3_moveToNonAdjacentCellThrows)
{
	TestWorld t;
	auto& unit = t.addStub(1, {0, 0});

	CHECK_THROWS(t.world.moveTo(unit, {2, 0}), std::logic_error);
	CHECK(unit.getPosition() == core::Position{0, 0});
}

TEST(M4_moveToOccupiedCellThrows)
{
	TestWorld t;
	auto& unit = t.addStub(1, {0, 0});
	t.addStub(2, {1, 0});

	CHECK_THROWS(t.world.moveTo(unit, {1, 0}), std::logic_error);
	CHECK(unit.getPosition() == core::Position{0, 0});
}

TEST(M5_unitNotOccupyingCellSharesItWithOthers)
{
	TestWorld t;
	t.addStub(1, {1, 1}, {.occupiesCell = false});
	auto& regular = t.addStub(2, {1, 1});

	CHECK(t.world.canStand(regular, {1, 1}));
}

TEST(M6_marchToOwnCellEndsImmediately)
{
	TestWorld t;
	auto& unit = t.addStub(1, {2, 2});
	t.world.march(1, {2, 2});

	CHECK(
		t.events()
		== std::vector<std::string>{
			"[1] MARCH_STARTED unitId=1 x=2 y=2 targetX=2 targetY=2", "[1] MARCH_ENDED unitId=1 x=2 y=2"});
	CHECK(!unit.getMovement()->marchTarget);
}

TEST(M7_reachingMarchTargetEndsMarch)
{
	TestWorld t;
	auto& unit = t.addStub(1, {0, 0});
	t.world.march(1, {1, 0});
	t.clearEvents();
	t.world.moveTo(unit, {1, 0});

	CHECK(
		t.events() == std::vector<std::string>{"[1] UNIT_MOVED unitId=1 x=1 y=0", "[1] MARCH_ENDED unitId=1 x=1 y=0"});
	CHECK(!unit.getMovement()->marchTarget);
}

// Урон и смерть

TEST(D1_unitWithoutHpCannotChangeHp)
{
	TestWorld t;
	auto& target = t.addStub(1, {0, 0}, {.hp = std::nullopt});

	CHECK_THROWS(t.world.changeHp(target, 1), std::logic_error);
}

TEST(D2_deadUnitCannotChangeHp)
{
	TestWorld t;
	auto& target = t.addStub(1, {0, 0}, {.hp = 1});
	t.world.changeHp(target, 0);

	CHECK_THROWS(t.world.changeHp(target, 5), std::logic_error);
	CHECK(t.events() == std::vector<std::string>{"[1] UNIT_DIED unitId=1"});
}

TEST(D3_hpDroppedToZeroLogsDeathOnly)
{
	TestWorld t;
	auto& target = t.addStub(1, {0, 0}, {.hp = 3});
	t.world.changeHp(target, 2);
	CHECK(t.events().empty());

	t.world.changeHp(target, 0);
	CHECK(t.events() == std::vector<std::string>{"[1] UNIT_DIED unitId=1"});
	CHECK(!target.isAlive());
}

TEST(D4_unitLeavingMapIsRemoved)
{
	TestWorld t;
	t.addStub(1, {0, 0});
	auto& mine = t.addStub(2, {1, 0}, {.hp = std::nullopt});
	mine.leaveMap();

	CHECK(!mine.isAlive());
	CHECK(t.world.removeDead());
	CHECK(t.world.getUnits().size() == 1);
}
