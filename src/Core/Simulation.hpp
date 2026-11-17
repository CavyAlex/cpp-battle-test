#pragma once

namespace sw::core
{
	class World;

	// Ходы до тех пор, пока на поле больше одного юнита и кто-то может действовать
	void runSimulation(World& world);
}
