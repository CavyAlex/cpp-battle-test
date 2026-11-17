#include "Swordsman.hpp"

#include <Features/Actions/Attack.hpp>
#include <Features/Actions/March.hpp>

namespace sw::features
{
	Swordsman::Swordsman(const io::SpawnSwordsman& command) :
			Unit(command.unitId, TypeName, {command.x, command.y}, command.hp, core::Movement{}),
			_strength(command.strength)
	{
		requirePositive(command.strength, "strength");
	}

	bool Swordsman::act(core::World& world)
	{
		return meleeAttack(*this, world, _strength) || marchStep(*this, world);
	}
}
