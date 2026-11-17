#pragma once

#include <Core/Unit.hpp>
#include <Core/World.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <cstdint>

namespace sw::features
{
	class Hunter final : public core::Unit
	{
	public:
		static constexpr const char* TypeName = "Hunter";

		explicit Hunter(const io::SpawnHunter& command);

		bool act(core::World& world) override;

	private:
		uint32_t _agility;
		uint32_t _strength;
		uint32_t _range;
	};
}
