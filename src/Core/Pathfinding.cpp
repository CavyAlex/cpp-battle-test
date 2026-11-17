#include "Pathfinding.hpp"

#include <algorithm>
#include <cstdint>

namespace sw::core
{
	namespace
	{
		// Меньшая из разностей координат. При равном расстоянии по Чебышёву D порядок по ней совпадает
		// с порядком по прямой (dx² + dy² = D² + m²), но без умножений и переполнений
		uint32_t minAxisOffset(Position a, Position b)
		{
			const auto diff = [](uint32_t lhs, uint32_t rhs)
			{
				return lhs > rhs ? lhs - rhs : rhs - lhs;
			};
			return std::min(diff(a.x, b.x), diff(a.y, b.y));
		}
	}

	std::optional<Position> nextStep(const World& world, const Unit& unit, Position target)
	{
		const auto start = unit.getPosition();
		std::optional<Position> best;
		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				if ((dx < 0 && start.x == 0) || (dy < 0 && start.y == 0))
				{
					continue;
				}
				const Position next{start.x + dx, start.y + dy};
				if (distance(next, target) >= distance(start, target) || !world.canStand(unit, next))
				{
					continue;
				}
				if (!best || minAxisOffset(next, target) < minAxisOffset(*best, target))
				{
					best = next;
				}
			}
		}
		return best;
	}
}
