#include <Core/Pathfinding.hpp>
#include <TestFramework.hpp>
#include <TestWorld.hpp>

using namespace sw;
using namespace sw::test;

namespace
{
	std::optional<core::Position> stepOn(
		uint32_t width,
		uint32_t height,
		const std::vector<core::Position>& walls,
		core::Position from,
		core::Position to,
		bool occupiesCell = true)
	{
		TestWorld t(width, height);
		auto& unit = t.addStub(1, from, {.occupiesCell = occupiesCell});
		core::UnitId id = 2;
		for (const auto wall : walls)
		{
			t.addStub(id++, wall);
		}
		return core::nextStep(t.world, unit, to);
	}
}

TEST(P1_straightPathPreferredAmongEqualOnes)
{
	CHECK(stepOn(10, 10, {}, {0, 0}, {3, 0}) == core::Position{1, 0});
}

TEST(P2_diagonalPath)
{
	CHECK(stepOn(10, 10, {}, {0, 0}, {3, 3}) == core::Position{1, 1});
}

TEST(P3_goesAroundObstacle)
{
	CHECK(stepOn(10, 10, {{1, 0}}, {0, 0}, {3, 0}) == core::Position{1, 1});
}

TEST(P4_greedyStepDoesNotSearchForGapInWall)
{
	CHECK(stepOn(5, 5, {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, {0, 0}, {4, 0}) == core::Position{1, 0});
}

TEST(P5_wallWithoutGapStillAllowsStepCloser)
{
	CHECK(stepOn(5, 5, {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}}, {0, 0}, {4, 0}) == core::Position{1, 0});
}

TEST(P6_occupiedTargetIsApproached)
{
	CHECK(stepOn(10, 10, {{3, 0}}, {0, 0}, {3, 0}) == core::Position{1, 0});
}

TEST(P7_nextToOccupiedTargetWaits)
{
	CHECK(!stepOn(10, 10, {{3, 0}}, {2, 0}, {3, 0}));
}

TEST(P8_surroundedUnitWaits)
{
	CHECK(!stepOn(3, 3, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {2, 1}, {0, 2}, {1, 2}, {2, 2}}, {1, 1}, {2, 2}));
}

TEST(P9_unitOnTargetDoesNotMove)
{
	CHECK(!stepOn(10, 10, {}, {3, 0}, {3, 0}));
}

TEST(P10_unitNotOccupyingCellsWalksThroughOthers)
{
	CHECK(stepOn(10, 10, {{1, 0}, {1, 1}}, {0, 0}, {3, 0}, false) == core::Position{1, 0});
}
