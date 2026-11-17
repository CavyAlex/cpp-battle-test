#pragma once

#include <Core/Position.hpp>
#include <Core/Unit.hpp>
#include <Core/World.hpp>
#include <optional>

namespace sw::core
{
	// Следующая клетка на пути к target: свободная соседняя клетка строго ближе к цели,
	// из равных — ближе к прямой до цели. Если target занята, юнит подходит вплотную и ждёт.
	// nullopt — ближе подойти нельзя (уже на цели, вплотную к занятой цели или путь перекрыт).
	std::optional<Position> nextStep(const World& world, const Unit& unit, Position target);
}
