#pragma once

#include <compare>
#include <cstdint>

namespace sw::core
{
	struct Position
	{
		uint32_t x{};
		uint32_t y{};

		auto operator<=>(const Position&) const = default;
	};

	// Расстояние по Чебышёву: за один шаг юнит может уйти в любую из 8 соседних клеток
	uint32_t distance(Position a, Position b);
}
