#include <Core/Simulation.hpp>
#include <Core/World.hpp>
#include <Features/Units/Registration.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <charconv>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
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

	uint32_t parseSeed(const std::string& text)
	{
		uint32_t seed{};
		const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), seed);
		if (text.empty() || error != std::errc{} || end != text.data() + text.size())
		{
			throw std::runtime_error("Invalid --seed value: " + text);
		}
		return seed;
	}

	Options parseOptions(int argc, char** argv)
	{
		const std::string usage = "Usage: sw_battle_test <scenario_file> [--seed <number>]";
		if (argc == 2)
		{
			return {argv[1], std::random_device{}()};
		}
		if (argc == 4 && std::string(argv[2]) == "--seed")
		{
			return {argv[1], parseSeed(argv[3])};
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

		// Не uniform_int_distribution: она по-разному устроена в libc++ и libstdc++
		std::mt19937 engine(options.seed);
		EventLog eventLog;
		core::World world(eventLog, [&engine](size_t n) { return static_cast<size_t>(engine() % n); });

		io::CommandParser parser;
		parser.add<io::CreateMap>([&](auto command) { world.createMap(command.width, command.height); })
			.add<io::March>([&](auto command) { world.march(command.unitId, {command.targetX, command.targetY}); });
		features::registerUnits(parser, world);

		parser.parse(file);
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
