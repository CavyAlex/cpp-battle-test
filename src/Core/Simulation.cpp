#include "Simulation.hpp"

#include "World.hpp"

namespace sw::core
{
	void runSimulation(World& world)
	{
		while (world.getAliveCount() > 1)
		{
			world.nextTick();

			// Ходят те, кто был на поле в начале хода, включая погибших в этом ходу
			const auto unitCount = world.getUnits().size();
			bool anyActed = false;
			for (size_t i = 0; i < unitCount; ++i)
			{
				anyActed |= world.getUnits()[i]->act(world);
			}

			// Если никто не действовал и никого не убрали, мир не изменился — следующий ход будет таким же пустым
			const bool anyRemoved = world.removeDead();
			if (!anyActed && !anyRemoved)
			{
				break;
			}
		}
	}
}
