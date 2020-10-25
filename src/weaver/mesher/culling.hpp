#ifndef WEAVER_MESHER_CULLING_HPP
#define WEAVER_MESHER_CULLING_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "fwd.hpp"
#include "voxel_reader.hpp"
#include "mesher_result.hpp"
#include "cube_def.hpp"
#include <array>
#include "../core/algorithm.hpp"
#include "../core/voxel_face.hpp"

namespace tc
{
template <typename Type> class WEAVER_API culling {
	template <typename T> using reader_t = weaver::voxel_reader<T>;
	enum boundry { r = 0, f = 1, u = 2, count = 3 };

    public:
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
			if (c >= e) {
				return false;
			}

			return reader.visible(*c);
		};

		auto check_neighbor = [reader = reader, volume_check](auto c, auto dir) {
			auto defs = reader(*c, dir);
			for (auto &&d : defs) {
				if (d.component.cull) {
					return true;
				}
			}

			return false;
		};

		vertex vert;
		auto volume = volume_begin + bh * bw;
		for (vert.z = 1; vert.z < bd - 1; ++vert.z) {
			for (vert.y = 0; vert.y < bh; ++vert.y) {
				for (vert.x = 0; vert.x < bw; ++vert.x, ++volume) {
					const bool in_bounds = is_in_bounds(vert);
					if (!in_bounds) {
						continue;
					}

					auto state = volume_check(volume);
					if (!state) {
						continue;
					}

					auto &&[nb, n, ni, cull] = find_boundries(
						vert, volume_begin, volume_check, check_neighbor);

					auto type_id = reader(*volume);
					static constexpr auto size =
						static_cast<size_t>(voxel_face::_count);
					for (auto d = 0; d < size; ++d) {
						if (state == n[d] && cull[d]) {
							// there hasn't been a change in state in this direction
							continue;
						}

						static const vertex remove_border{ 1.0, 1.0, 1.0 };

						add_quad(d, state, vert - remove_border, quads,
							 type_id, volume, reader);
					}
				}
			}
		}

		return result;
	}

	template <typename Iter, typename T>
	auto add_quad(int32_t direction, bool state, const vertex &vert, std::vector<quad> &quads,
		      weaver::voxel_id_t type_id, Iter current_vox, reader_t<T> &reader) const
	{
		auto faces = cube_faces;
		static constexpr auto size = static_cast<size_t>(voxel_face::_count);
		auto d = direction;
		auto dir = static_cast<voxel_face>(d);
		auto voxel_defintion = reader(*current_vox, dir);

		auto base_face = faces[d];
		base_face.normal.normalize_quick();
		base_face.type_id = type_id;

		for (auto &&def : voxel_defintion) {
			auto face = base_face;

			std::array<vector2d, 2> uv_space { };
			uv_space[0] = weaver::lerp(base_face.uv[0], base_face.uv[2], def.component.uv_min); // bottom left
			uv_space[1] = weaver::lerp(base_face.uv[0], base_face.uv[2], def.component.uv_max); // top right

			face.for_each([&vert, &base_face, &uv_space, &def](auto i, auto &&p, auto &&uv) {
				p = clamp(base_face[i], def.min, def.max);
				p += vert + def.translate;
				uv = weaver::lerp(uv_space[0], uv_space[1], 1 - uv);
			});

			quads.emplace_back(face);
		}
	}

	auto calc_index(const vertex &vert) const
	{
		vector3i i{ vert };
		return i.z * (width + 2) * (height + 2) + i.y * (width + 2) + i.x;
	};

	template <typename Iter, typename VolumeCheck, typename CullingCheck>
	auto find_boundries(const vertex &vert, Iter volume, VolumeCheck &&volume_check,
			    CullingCheck &&cull_check) const
	{
		static constexpr auto size = static_cast<size_t>(voxel_face::_count);
		std::array<vector3d, size> nc{
			vert + vertex{ 1.0 }, // Right
			vert + vertex{ 0.0, 1.0 }, // Back
			vert + vertex{ 0.0, 0.0, 1.0 }, // Top
			vert + vertex{ -1.0 }, // Left
			vert + vertex{ 0.0, -1.0 }, // Front
			vert + vertex{ 0.0, 0.0, -1.0 }, // Bottom
		};
		std::array<int32_t, size> ni{};
		std::array<bool, size> nb{};
		std::array<bool, size> n{};
		std::array<bool, size> cull_n{};

		for (auto i = 0; i < ni.size(); ++i) {
			ni[i] = calc_index(nc[i]);
			nb[i] = is_in_bounds(nc[i]);
			n[i] = volume_check(volume + ni[i]);

			voxel_face face = static_cast<voxel_face>((i + 3) % size);
			cull_n[i] = n[i] && cull_check(volume + ni[i], face);
		}

		return std::make_tuple(nb, n, ni, cull_n);
	}

	auto is_in_bounds(const vertex &v) const
	{
		return (1.0 <= v.x && v.x <= static_cast<int32_t>(width)) &&
		       (1.0 <= v.y && v.y <= static_cast<int32_t>(height)) &&
		       (1.0 <= v.z && v.z <= static_cast<int32_t>(depth));
	}
};
} // namespace tc

#endif // WEAVER_MESHER_CULLING_HPP
