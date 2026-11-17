#include "March.hpp"

#include <Core/Pathfinding.hpp>

namespace sw::features
{
	bool marchStep(core::Unit& self, core::World& world)
	{
		const auto& movement = self.getMovement();
		if (!movement)
		{
			return false;
		}

		bool moved = false;
		for (uint32_t step = 0; step < movement->speed && movement->marchTarget; ++step)
		{
			const auto next = core::nextStep(world, self, *movement->marchTarget);
			if (!next)
			{
				break;
			}
			world.moveTo(self, *next);
			moved = true;
		}
		return moved;
	}
}
