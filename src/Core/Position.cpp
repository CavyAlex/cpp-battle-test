#include "Position.hpp"

#include <algorithm>

namespace sw::core
{
	uint32_t distance(Position a, Position b)
	{
		auto diff = [](uint32_t lhs, uint32_t rhs)
		{
			return lhs > rhs ? lhs - rhs : rhs - lhs;
		};
		return std::max(diff(a.x, b.x), diff(a.y, b.y));
	}
}
