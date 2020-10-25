#ifndef WEAVER_MESHER_SIMPLE_HPP
#define WEAVER_MESHER_SIMPLE_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "fwd.hpp"
#include "voxel_reader.hpp"
#include "mesher_result.hpp"
#include "cube_def.hpp"
#include <array>

namespace tc
{
template <typename Type> class WEAVER_API simple {
	enum boundry { r = 0, f = 1, u = 2, count = 3 };

    public:
	template <typename T> using reader_t = weaver::voxel_reader<T>;
	
	template <typename Iter>
	mesher_result eval(Iter volume_begin, Iter volume_end, reader_t<Type> reader = {}) const
	{
		if (add_border) {
			int32_t dw{ static_cast<int32_t>(width) };
			int32_t dh{ static_cast<int32_t>(height) };
			int32_t dd{ static_cast<int32_t>(depth) };
			int32_t bw{ dw + 2 };
			int32_t bh{ dh + 2 };
			int32_t bd{ dd + 2 };

			std::vector<Type *> volume;
			volume.resize(bw * bh * bd, nullptr);
			for (auto z = 0; z < dd; ++z) {
				auto zb = (z + 1) * bh * bw;
				auto zi = z * dh * dw;
				for (auto y = 0; y < dh; ++y) {
					auto yb = (y + 1) * bh + zb;
					auto yi = y * dh + zi;
					for (auto x = 0; x < dw; ++x) {
						auto b = (x + 1) + yb;
						auto i = x + yi;
						volume[b] = (volume_begin + i).operator->();
					}
				}
			}

			return work(std::begin(volume), std::end(volume),
				    reader_t<Type *>{ reader });
		} else {
			return work(volume_begin, volume_end, reader);
		}
	}

	size_t width{ 0 };
	size_t height{ 0 };
	size_t depth{ 0 };
	bool add_border{ false };

    private:
	template <typename Iter, typename T>
	mesher_result work(Iter volume_begin, Iter volume_end, reader_t<T> reader = {}) const
	{
		int32_t dw{ static_cast<int32_t>(width) };
		int32_t dh{ static_cast<int32_t>(height) };
		int32_t dd{ static_cast<int32_t>(depth) };
		int32_t bw{ dw + 2 };
		int32_t bh{ dh + 2 };
		int32_t bd{ dd + 2 };

		mesher_result result;
		auto &quads = result.quads;

		quads.reserve(height * width * depth * 6);

		auto volume_check = [reader = reader, e = volume_end](auto c) {
			if (c == e) {
				return false;
			}

			return reader.visible(*c);
		};

		vertex vert;
		auto volume = volume_begin + bh * bw;
		for (vert.z = 1; vert.z < bd - 1; ++vert.z) {
			for (vert.y = 0; vert.y < bh; ++vert.y) {
				for (vert.x = 0; vert.x < bw; ++vert.x, ++volume) {
					auto vi = volume - volume_begin;
					const bool in_bounds = is_in_bounds(vert);
					auto state = volume_check(volume);

					if (!in_bounds || !state) {
						// skip if it's not in bounds or not visable
						continue;
					}

					static const vertex remove_border{ 1.0, 1.0, 1.0 };

					add_quads(vert - remove_border, quads, volume, reader);
				}
			}
		}

		return result;
	}

    private:
	auto is_in_bounds(const vertex &v) const
	{
		return (1.0 <= v.x && v.x <= static_cast<int32_t>(width)) &&
		       (1.0 <= v.y && v.y <= static_cast<int32_t>(height)) &&
		       (1.0 <= v.z && v.z <= static_cast<int32_t>(depth));
	}

	template <typename Iter, typename T>
	auto add_quads(const vertex &vert, std::vector<quad> &quads, Iter current_vox,
		       reader_t<T> &reader) const
	{
		auto faces = cube_faces;
		auto type_id = reader(*current_vox);

		for (auto d = 0; d < 3; ++d) {
			for (auto side = 0; side < 2; ++side) {
				auto state = side == 1;

				auto index = d + (state ? 0 : 3);

				auto voxel_defintion =
					reader(*current_vox, static_cast<voxel_face>(index));

				auto base_face = faces[index];
				base_face.normal.normalize_quick();
				base_face.type_id = type_id;

				for (auto &&def : voxel_defintion) {
					auto face = base_face;
					face.material_id = def.material;

					std::array<vector2d, 2> uv_space { };
					uv_space[0] = weaver::lerp(base_face.uv[0], base_face.uv[2], def.uv_min); // bottom left
					uv_space[1] = weaver::lerp(base_face.uv[0], base_face.uv[2], def.uv_max); // top right

					face.for_each([&vert, &base_face, &uv_space, &def](auto i, auto &&p, auto &&uv) {
						p = clamp(base_face[i], def.min, def.max);
						p += vert + def.translate;
						uv = weaver::lerp(uv_space[0], uv_space[1], 1 - uv);
					});

					quads.emplace_back(face);
				}
			}
		}
	}
};
} // namespace tc

#endif // WEAVER_MESHER_SIMPLE_HPP
