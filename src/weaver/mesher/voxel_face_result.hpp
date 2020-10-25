#ifndef WEAVER_MESHER_VOXEL_FACE_RESULT_HPP
#define WEAVER_MESHER_VOXEL_FACE_RESULT_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/vector3.hpp"
#include "../core/voxel_component_def.hpp"
#include <vector>

namespace tc
{
namespace weaver
{
	struct WEAVER_API voxel_face_result
	{
		vector3d min{ 0.0, 0.0, 0.0 };
		vector3d max{ 1.0, 1.0, 1.0 };
		vector3d translate{ 0.0, 0.0, 0.0 };
		vector2d uv_min{ 0.0, 0.0 };
		vector2d uv_max{ 1.0, 1.0 };
		std::string_view material{};
		bool cull{ true };
	};
}
}

#endif // WEAVER_MESHER_VOXEL_FACE_RESULT_HPP
