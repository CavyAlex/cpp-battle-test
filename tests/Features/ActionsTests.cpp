#include <Features/Actions/Attack.hpp>
#include <Features/Actions/March.hpp>
#include <TestFramework.hpp>
#include <TestWorld.hpp>
#include <stdexcept>

using namespace sw;
using namespace sw::test;

TEST(A1_meleeAttackPicksRandomNeighbor)
{
	for (const auto& [pick, expectedTarget] : {std::pair{size_t{0}, 2}, std::pair{size_t{1}, 3}})
	{
		TestWorld t;
		auto& attacker = t.addStub(1, {1, 1});
		t.addStub(2, {0, 1});
		t.addStub(3, {2, 1});
		t.addStub(4, {3, 1});
		t.pickResult = pick;

		CHECK(features::meleeAttack(attacker, t.world, 5));
		CHECK(t.pickCalls == std::vector<size_t>{2});
		CHECK(
			t.events()
			== std::vector<std::string>{
				"[1] UNIT_ATTACKED attackerUnitId=1 targetUnitId=" + std::to_string(expectedTarget)
				+ " damage=5 targetHp=5"});
	}
}

TEST(A2_meleeAttackWithoutNeighborsDoesNothing)
{
	TestWorld t;
	auto& attacker = t.addStub(1, {0, 0});
	t.addStub(2, {5, 5});

	CHECK(!features::meleeAttack(attacker, t.world, 5));
	CHECK(t.pickCalls.empty());
	CHECK(t.events().empty());
}

TEST(S1_noMarchTargetMeansNoStep)
{
	TestWorld t;
	auto& unit = t.addStub(1, {0, 0});

	CHECK(!features::marchStep(unit, t.world));
	CHECK(t.events().empty());
}

TEST(S2_fastUnitMakesSeveralStepsPerTurn)
{
	TestWorld t;
	auto& unit = t.addStub(1, {0, 0}, {.movement = core::Movement{.speed = 2}});
	t.world.march(1, {4, 0});
	t.clearEvents();

	CHECK(features::marchStep(unit, t.world));
	CHECK(t.events() == std::vector<std::string>{"[1] UNIT_MOVED unitId=1 x=1 y=0", "[1] UNIT_MOVED unitId=1 x=2 y=0"});
}

TEST(S3_fastUnitStopsAtMarchTarget)
{
	TestWorld t;
	auto& unit = t.addStub(1, {0, 0}, {.movement = core::Movement{.speed = 2}});
	t.world.march(1, {1, 0});
	t.clearEvents();

	CHECK(features::marchStep(unit, t.world));
	CHECK(
		t.events() == std::vector<std::string>{"[1] UNIT_MOVED unitId=1 x=1 y=0", "[1] MARCH_ENDED unitId=1 x=1 y=0"});
}

TEST(F1_damageAboveHpStopsAtZero)
{
	TestWorld t;
	auto& attacker = t.addStub(1, {0, 0});
	auto& target = t.addStub(2, {1, 0}, {.hp = 3});
	features::damage(t.world, attacker, target, 10);

	CHECK(
		t.events()
		== std::vector<std::string>{
			"[1] UNIT_ATTACKED attackerUnitId=1 targetUnitId=2 damage=10 targetHp=0", "[1] UNIT_DIED unitId=2"});
	CHECK(target.getHp() == 0u);
}

TEST(F2_damageToInvalidTargetLogsNothing)
{
	TestWorld t;
	auto& attacker = t.addStub(1, {0, 0});
	auto& target = t.addStub(2, {1, 0}, {.hp = std::nullopt});

	CHECK_THROWS(features::damage(t.world, attacker, target, 1), std::logic_error);
	CHECK(t.events().empty());
}
