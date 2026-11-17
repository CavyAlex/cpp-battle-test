#pragma once

#include <Core/Unit.hpp>
#include <Core/World.hpp>

namespace sw::features
{
	// До movement.speed шагов к цели марша. Возвращает true, если юнит сдвинулся
	bool marchStep(core::Unit& self, core::World& world);
}
