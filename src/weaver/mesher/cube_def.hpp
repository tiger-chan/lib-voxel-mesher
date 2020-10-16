#ifndef WEAVER_MESHER_CUBE_DEF_HPP
#define WEAVER_MESHER_CUBE_DEF_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/vertex.hpp"
#include "../core/quad.hpp"
#include "fwd.hpp"
#include <array>

namespace tc
{
struct WEAVER_API cube_def {
	static constexpr std::array<quad, 6> build_faces()
	{
		vector3d a{ 0.0, 0.0, 0.0 };
		vector3d b{ 1.0, 0.0, 0.0 };
		vector3d c{ 1.0, 1.0, 0.0 };
		vector3d d{ 0.0, 1.0, 0.0 };

		vector3d e{ 0.0, 0.0, 1.0 };
		vector3d f{ 1.0, 0.0, 1.0 };
		vector3d g{ 1.0, 1.0, 1.0 };
		vector3d h{ 0.0, 1.0, 1.0 };

		std::array<quad, 6> quads{
			quad{ b, c, g, f }, // Right face
			quad{ a, b, f, e }, // Front face
			quad{ e, f, g, h }, // Top face
			quad{ d, a, e, h }, // Left face
			quad{ c, d, h, g }, // Back face
			quad{ d, c, b, a }, // Bottom face
		};

		std::array<vector2d, 4> uv{
			vector2d{ 1.0, 1.0 },
			vector2d{ 0.0, 1.0 },
			vector2d{ 0.0, 0.0 },
			vector2d{ 1.0, 0.0 },
		};

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