#include <Core/Simulation.hpp>
#include <Core/World.hpp>
#include <Features/Units/Hunter.hpp>
#include <Features/Units/Swordsman.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>

namespace
{
	struct Options
	{
		std::string scenarioPath;
		uint32_t seed{};
	};

	Options parseOptions(int argc, char** argv)
	{
		const std::string usage = "Usage: sw_battle_test <scenario_file> [--seed <number>]";
		if (argc == 2)
		{
			return {argv[1], std::random_device{}()};
		}
		if (argc == 4 && std::string(argv[2]) == "--seed")
		{
			return {argv[1], static_cast<uint32_t>(std::stoul(argv[3]))};
		}
		throw std::runtime_error(usage);
	}

	void run(const Options& options)
	{
		using namespace sw;

		std::ifstream file(options.scenarioPath);
		if (!file)
		{
			throw std::runtime_error("File not found: " + options.scenarioPath);
		}

		// Индекс считаем сами, а не через std::uniform_int_distribution: так один seed даёт
		// одинаковый результат на любой стандартной библиотеке
		std::mt19937 engine(options.seed);
		EventLog eventLog;
		core::World world(eventLog, [&engine](size_t n) { return static_cast<size_t>(engine() % n); });

		io::CommandParser parser;
		parser.add<io::CreateMap>([&](auto command) { world.createMap(command.width, command.height); })
			.add<io::March>([&](auto command) { world.march(command.unitId, {command.targetX, command.targetY}); })
			.add<io::SpawnSwordsman>([&](auto command) { world.spawn(std::make_unique<features::Swordsman>(command)); })
			.add<io::SpawnHunter>([&](auto command) { world.spawn(std::make_unique<features::Hunter>(command)); });

		parser.parse(file);
		// «CREATE_MAP — эта команда начинает любой сценарий»: пустой сценарий — ошибка, а не пустой лог
		if (!world.hasMap())
		{
			throw std::runtime_error("Scenario has no CREATE_MAP");
		}
		core::runSimulation(world);
	}
}

int main(int argc, char** argv)
{
	try
	{
		run(parseOptions(argc, argv));
		return 0;
	}
	catch (const std::exception& error)
	{
		std::cerr << "Error: " << error.what() << std::endl;
		return 1;
	}
}
