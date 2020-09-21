#pragma once

#include <cstdint>
#include <array>
#include "vector2.hpp"
#include "vector3.hpp"

namespace tc
{
	struct quad : public std::array<int32_t, 4>
	{
		vector3d normal{};
		std::array<vector2d, 4> uv;
	};
}
