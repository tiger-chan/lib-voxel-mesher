#ifndef WEAVER_CORE_VOXEL_COMPONENT_DEF_HPP
#define WEAVER_CORE_VOXEL_COMPONENT_DEF_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "voxel_face.hpp"
#include "vector3.hpp"
#include "vector2.hpp"
#include <unordered_map>
#include <string>

namespace tc
{
struct WEAVER_API face_def {
	vector2d uv_min{ 0.0, 0.0 };
	vector2d uv_max{ 1.0, 1.0 };
	std::string material{};
	bool cull{ true };
};

struct WEAVER_API voxel_component_def
{
	vector3d min{ 0.0, 0.0, 0.0 };
	vector3d max{ 1.0, 1.0, 1.0 };
	vector3d translate{ 0.0, 0.0, 0.0 };
	std::unordered_map<voxel_face, face_def> faces;
};
} // namespace tc

#endif // WEAVER_CORE_VOXEL_COMPONENT_DEF_HPP
