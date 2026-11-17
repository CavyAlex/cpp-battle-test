#include "Attack.hpp"

#include <IO/Events/UnitAttacked.hpp>
#include <stdexcept>
#include <string>

namespace sw::features
{
	namespace
	{
		bool attackRandomTarget(
			core::Unit& self, core::World& world, core::AttackKind kind, core::AttackRange range, uint32_t amount)
		{
			auto* target = world.pickRandom(world.findTargets(self, kind, range));
			if (!target)
			{
				return false;
			}
			damage(world, self, *target, amount);
			return true;
		}
	}

	bool meleeAttack(core::Unit& self, core::World& world, uint32_t amount)
	{
		return attackRandomTarget(self, world, core::AttackKind::Melee, {1, 1}, amount);
	}

	bool rangedAttack(core::Unit& self, core::World& world, core::AttackRange range, uint32_t amount)
	{
		return attackRandomTarget(self, world, core::AttackKind::Ranged, range, amount);
	}

	void damage(core::World& world, const core::Unit& attacker, core::Unit& target, uint32_t amount)
	{
		const auto& currentHp = target.getHp();
		// Не дубль changeHp: проверяем до записи UNIT_ATTACKED в лог
		if (!currentHp || !target.isAlive())
		{
			throw std::logic_error("Unit " + std::to_string(target.getId()) + " cannot take damage");
		}
		const auto hp = *currentHp > amount ? *currentHp - amount : 0;
		world.log(io::UnitAttacked{attacker.getId(), target.getId(), amount, hp});
		world.changeHp(target, hp);
	}
}
