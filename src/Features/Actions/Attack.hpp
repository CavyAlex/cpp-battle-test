#pragma once

#include <Core/Unit.hpp>
#include <Core/World.hpp>
#include <cstdint>

namespace sw::features
{
	// Бьёт случайного юнита в соседней клетке. Возвращает true, если удар состоялся
	bool meleeAttack(core::Unit& self, core::World& world, uint32_t amount);

	// Стреляет в случайного юнита на дистанции range. Возвращает true, если выстрел состоялся
	bool rangedAttack(core::Unit& self, core::World& world, core::AttackRange range, uint32_t amount);

	// Наносит урон: пишет UNIT_ATTACKED и уменьшает HP (не ниже нуля), при нуле World допишет UNIT_DIED
	void damage(core::World& world, const core::Unit& attacker, core::Unit& target, uint32_t amount);
}
