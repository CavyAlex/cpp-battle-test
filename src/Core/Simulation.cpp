#include "Simulation.hpp"

#include "World.hpp"

namespace sw::core
{
	void runSimulation(World& world)
	{
		while (world.getAliveCount() > 1)
		{
			world.nextTick();

			// Погибшие в этом ходу тоже ходят
			const auto unitCount = world.getUnits().size();
			bool anyActed = false;
			for (size_t i = 0; i < unitCount; ++i)
			{
				anyActed |= world.getUnits()[i]->act(world);
			}

			// Мир не изменился — следующий ход будет пустым
			const bool anyRemoved = world.removeDead();
			if (!anyActed && !anyRemoved)
			{
				break;
			}
		}
	}
}
