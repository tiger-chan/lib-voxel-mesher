#ifndef WEAVER_CORE_QUAD_HPP
#define WEAVER_CORE_QUAD_HPP

#include <cstdint>
#include <array>
#include "../config/config.hpp"
#include "attributes.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "vertex.hpp"

namespace tc
{
	struct WEAVER_API quad : public std::array<vertex, 4>
	{
		enum class triangle {
			first, second
		};

		enum class rotation {
			ccw, cw
		};

		constexpr std::array<int32_t, 3> get_triange(triangle t, rotation r = rotation::ccw) const {
			if (r == rotation::ccw) {
				switch(t)
				{
					default:
					case triangle::first: return { 0, 1, 2 };
					case triangle::second: return { 0, 2, 3 };
				};
			}

			switch(t)
			{
				default:
				case triangle::first: return { 2, 1, 0 };
				case triangle::second: return { 3, 2, 0 };
			};
		}

		template<typename Fn>
		void for_each(Fn fn) {
			for (int32_t i = 0; i < 4; ++i) {
				fn(i, this->operator[](i), uv[i]);
			};
		}

		template<typename Fn>
		void for_each(Fn fn) const {
			for (int32_t i = 0; i < 4; ++i) {
				fn(i, (*this)[i], uv[i]);
			};
		}

		vector3d normal{};
		std::array<vector2d, 4> uv;
		weaver::voxel_id_t type_id{ weaver::unset_voxel_id };
	};
}

#endif // WEAVER_CORE_QUAD_HPP