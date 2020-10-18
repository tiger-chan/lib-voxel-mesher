#ifndef WEAVER_MESHER_CUBE_DEF_HPP
#define WEAVER_MESHER_CUBE_DEF_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/vertex.hpp"
#include "../core/quad.hpp"
#include "fwd.hpp"
#include <array>

#ifndef WEAVER_UV_BOTTOM_LEFT_ORIGIN
#	define WEAVER_UV_BOTTOM_LEFT_ORIGIN false
#endif

namespace tc
{
struct WEAVER_API cube_def {
	static constexpr std::array<quad, 6> build_faces()
	{
		vector3d a{ 0.0, 0.0, 0.0 };
		vector3d b{ 1.0, 0.0, 0.0 };
		vector3d c{ 0.0, 1.0, 0.0 };
		vector3d d{ 1.0, 1.0, 0.0 };

		vector3d e{ 0.0, 0.0, 1.0 };
		vector3d f{ 1.0, 0.0, 1.0 };
		vector3d g{ 0.0, 1.0, 1.0 };
		vector3d z{ 1.0, 1.0, 1.0 };

		std::array<quad, 6> quads{
			quad{ b, d, z, f }, // Right face
			quad{ d, c, g, z }, // Back face
			quad{ e, f, z, g }, // Top face
			quad{ c, a, e, g }, // Left face
			quad{ a, b, f, e }, // Front face
			quad{ c, d, b, a }, // Bottom face
		};

		std::array<vector2d, 4> uv;
		if constexpr (WEAVER_UV_BOTTOM_LEFT_ORIGIN) {
			uv = std::array<vector2d, 4>{
				vector2d{ 0.0, 0.0 }, // UV Bottom left
				vector2d{ 1.0, 0.0 }, // UV Bottom right
				vector2d{ 1.0, 1.0 }, // UV Top Right
				vector2d{ 0.0, 1.0 }, // UV Top Left
			};
		}
		else {
			uv = std::array<vector2d, 4>{
				vector2d{ 0.0, 1.0 }, // UV Bottom left
				vector2d{ 1.0, 1.0 }, // UV Bottom right
				vector2d{ 1.0, 0.0 }, // UV Top Right
				vector2d{ 0.0, 0.0 }, // UV Top Left
			};
		}
		
		for (auto &q : quads) {
			auto cb = q[2] - q[1];
			auto ab = q[0] - q[1];
			auto normal = cb.cross(ab);
			q.normal = normal;
			q.uv = uv;
		}

		return quads;
	}
};
static constexpr auto cube_faces = cube_def::build_faces();
} // namespace tc

#endif // WEAVER_MESHER_CUBE_DEF_HPP