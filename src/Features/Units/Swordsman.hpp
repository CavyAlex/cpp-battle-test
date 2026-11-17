#pragma once

#include <Core/Unit.hpp>
#include <Core/World.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <cstdint>

namespace sw::features
{
	class Swordsman final : public core::Unit
	{
	public:
		static constexpr const char* TypeName = "Swordsman";

		explicit Swordsman(const io::SpawnSwordsman& command);

		bool act(core::World& world) override;

	private:
		uint32_t _strength;
	};
}
