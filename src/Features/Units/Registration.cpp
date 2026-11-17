#include "Registration.hpp"

#include "Hunter.hpp"
#include "Swordsman.hpp"

#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <memory>

namespace sw::features
{
	void registerUnits(io::CommandParser& parser, core::World& world)
	{
		parser.add<io::SpawnSwordsman>([&world](auto command) { world.spawn(std::make_unique<Swordsman>(command)); })
			.add<io::SpawnHunter>([&world](auto command) { world.spawn(std::make_unique<Hunter>(command)); });
	}
}
