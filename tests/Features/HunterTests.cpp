#include <Features/Units/Hunter.hpp>
#include <TestFramework.hpp>
#include <TestWorld.hpp>

using namespace sw;
using namespace sw::test;

TEST(H1_hunterShootsRandomTargetInRange)
{
	TestWorld t;
	auto& hunter = t.add(std::make_unique<features::Hunter>(io::SpawnHunter{1, 0, 0, 10, 5, 1, 4}));
	t.addStub(2, {2, 0});
	t.addStub(3, {4, 4});
	t.addStub(4, {5, 0});
	t.pickResult = 1;

	CHECK(hunter.act(t.world));
	CHECK(t.pickCalls == std::vector<size_t>{2});
	CHECK(
		t.events()
		== std::vector<std::string>{"[1] UNIT_ATTACKED attackerUnitId=1 targetUnitId=3 damage=5 targetHp=5"});
}

TEST(H2_hunterWithNeighborStrikesInsteadOfShooting)
{
	TestWorld t;
	auto& hunter = t.add(std::make_unique<features::Hunter>(io::SpawnHunter{1, 0, 0, 10, 5, 2, 4}));
	t.addStub(2, {1, 0});
	t.addStub(3, {3, 0});

	CHECK(hunter.act(t.world));
	CHECK(t.pickCalls == std::vector<size_t>{1});
	CHECK(
		t.events()
		== std::vector<std::string>{"[1] UNIT_ATTACKED attackerUnitId=1 targetUnitId=2 damage=2 targetHp=8"});
}
