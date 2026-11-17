#include "Hunter.hpp"

#include <Features/Actions/Attack.hpp>
#include <Features/Actions/March.hpp>

namespace sw::features
{
	Hunter::Hunter(const io::SpawnHunter& command) :
			Unit(command.unitId, TypeName, {command.x, command.y}, command.hp, core::Movement{}),
			_agility(command.agility),
			_strength(command.strength),
			_range(command.range)
	{
		requirePositive(command.agility, "agility");
		requirePositive(command.strength, "strength");
		requireAtLeast(command.range, 2, "range");
	}

	bool Hunter::act(core::World& world)
	{
		const bool hasNeighbors = !world.unitsWithin(getPosition(), 1, this).empty();
		if (!hasNeighbors && rangedAttack(*this, world, {2, _range}, _agility))
		{
			return true;
		}
		return meleeAttack(*this, world, _strength) || marchStep(*this, world);
	}
}
