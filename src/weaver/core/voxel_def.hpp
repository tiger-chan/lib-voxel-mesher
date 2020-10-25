#ifndef WEAVER_CORE_VOXEL_DEF_HPP
#define WEAVER_CORE_VOXEL_DEF_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "voxel_face.hpp"
#include "vector3.hpp"
#include "voxel_component_def.hpp"
#include <vector>

namespace tc
{

struct WEAVER_API voxel_def
{
	std::vector<voxel_component_def> components;
};

} // namespace tc

#endif // WEAVER_CORE_VOXEL_DEF_HPP
