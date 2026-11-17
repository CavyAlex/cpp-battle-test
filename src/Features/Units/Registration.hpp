#pragma once

#include <Core/World.hpp>
#include <IO/System/CommandParser.hpp>

namespace sw::features
{
	// Подключает к парсеру команды спавна всех юнитов. Новый юнит дописывается сюда
	void registerUnits(io::CommandParser& parser, core::World& world);
}
