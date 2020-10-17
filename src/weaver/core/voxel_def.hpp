#ifndef WEAVER_CORE_VOXEL_DEF_HPP
#define WEAVER_CORE_VOXEL_DEF_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "vector3.hpp"

namespace tc
{
struct WEAVER_API voxel_def
{
	vector3d min{ 0.0, 0.0, 0.0 };
	vector3d max{ 1.0, 1.0, 1.0 };
	vector3d translate{ 0.0, 0.0, 0.0 };
};
} // namespace tc

#endif // WEAVER_CORE_VOXEL_DEF_HPP