#ifndef WEAVER_CORE_QUAD_HPP
#define WEAVER_CORE_QUAD_HPP

#include <cstdint>
#include <array>
#include "../config/config.hpp"
#include "attributes.hpp"
#include "vector2.hpp"
#include "vector3.hpp"

namespace tc
{
	struct WEAVER_API quad : public std::array<int32_t, 4>
	{
		vector3d normal{};
		std::array<vector2d, 4> uv;
		weaver::voxel_id_t type_id{ weaver::unset_voxel_id };
	};
}

#endif // WEAVER_CORE_QUAD_HPP