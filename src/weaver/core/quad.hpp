#ifndef WEAVER_CORE_QUAD_HPP
#define WEAVER_CORE_QUAD_HPP

#include <cstdint>
#include <array>
#include "attributes.hpp"
#include "vector2.hpp"
#include "vector3.hpp"

namespace tc
{
	struct WEAVER_API quad : public std::array<int32_t, 4>
	{
		vector3d normal{};
		std::array<vector2d, 4> uv;
	};
}

#endif // WEAVER_CORE_QUAD_HPP