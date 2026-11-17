#pragma once

#include <Core/Position.hpp>
#include <Core/Unit.hpp>
#include <Core/World.hpp>
#include <optional>

namespace sw::core
{
	// Шаг в свободную соседнюю клетку ближе к target; nullopt — ближе не подойти
	std::optional<Position> nextStep(const World& world, const Unit& unit, Position target);
}
