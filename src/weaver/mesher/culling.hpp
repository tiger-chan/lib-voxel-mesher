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

namespace tc
{
template<typename Type>
class WEAVER_API culling {
	template<typename T>
	using reader_t = weaver::voxel_reader<T>;
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

			std::vector<Type*> volume;
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

			return work(std::begin(volume), std::end(volume), reader_t<Type*>{reader});
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
		static constexpr auto dir = build_directions();
		int32_t dw{ static_cast<int32_t>(width) };
		int32_t dh{ static_cast<int32_t>(height) };
		int32_t dd{ static_cast<int32_t>(depth) };
		int32_t bw{ dw + 2 };
		int32_t bh{ dh + 2 };
		int32_t bd{ dd + 2 };

		std::vector<int32_t> vert_map{};
		mesher_result result;
		auto &quads = result.quads;

		quads.reserve(height * width * depth * 6);

		auto volume_check = [reader = reader, e = volume_end](auto c) {
			if (c >= e) {
				return false;
			}

			return reader.visible(*c);
		};

		vertex vert;
		auto volume = volume_begin;
		for (vert.z = 0; vert.z < bd; ++vert.z) {
			for (vert.y = 0; vert.y < bh; ++vert.y) {
				for (vert.x = 0; vert.x < bw; ++vert.x, ++volume) {
					auto vi = volume - volume_begin;
					const bool in_bounds = is_in_bounds(vert);
					auto state = volume_check(volume);

					auto &&[nb, n, ni] = find_boundries(vert, volume_begin, volume_check);

					for (auto d = 0; d < boundry::count; ++d) {
						if (state == n[d]) {
							// there hasn't been a change in state in this direction
							continue;
						}

						if (!in_bounds && !nb[d]) {
							continue;
						}

						auto vox = state == true ? volume : volume_begin + ni[d];

						static const vertex remove_border{ 1.0, 1.0, 1.0 };

						add_quad(d, state, vert - remove_border, quads, vox, readers);
					}
				}
			}
		}

		return result;
	}

	template <typename Iter, typename T, typename VertInsert>
	auto add_quad(int32_t direction, bool state, const vertex &vert, std::vector<quad> &quads, Iter current_vox, reader_t<T>& reader) const
	{
		constexpr auto dir = build_directions();

		vertex delta{0.0,0.0,0.0};
		if (!state) {
			// need to add a delta because when the state is false
			// that means we are drawing from a neighbor not this voxel
			switch(direction) {
				case boundry::r: delta = { 1.0, 0.0, 0.0 }; break;
				case boundry::f: delta = { 0.0, 1.0, 0.0 }; break;
				case boundry::u: delta = { 0.0, 0.0, 1.0 }; break;
			}
		}

		auto faces = cube_faces;
		auto index = direction + (state ? 0 : 3);

		auto voxel_defintion = reader(*current_vox, static_cast<voxel_face>(index));
		auto type_id = reader(*current_vox);
		
		auto base_face = faces[index];
		base_face.normal.normalize_quick();
		
		for (auto&& def: voxel_defintion)
		{
			vector3d& min{def.min};
			vector3d& max{def.max};
			auto face = base_face;

			face[0] = clamp(base_face[0], min, max);
			face[1] = clamp(base_face[1], min, max);
			face[2] = clamp(base_face[2], min, max);
			face[3] = clamp(base_face[3], min, max);
			

			for (auto& v: face) {
				v += vert + delta + def.translate;
			}	
			quads.emplace_back(face);
		}

	}

	auto calc_index(const vertex &vert) const
	{
		vector3i i{ vert };
		return i.z * (width + 2) * (height + 2) + i.y * (width + 2) + i.x;
	};

	template <typename Iter, typename VolumeCheck>
	auto find_boundries(const vertex &vert, Iter volume,
			    VolumeCheck &&volume_check) const
	{
		std::array<vector3d, 3> nc{ vert + vertex{ 1.0 }, vert + vertex{ 0.0, 1.0 },
					    vert + vertex{ 0.0, 0.0, 1.0 } };

		std::array<int32_t, 3> ni{
			calc_index(nc[0]),
			calc_index(nc[1]),
			calc_index(nc[2]),
		};

		std::array<bool, 3> nb{
			is_in_bounds(nc[0]),
			is_in_bounds(nc[1]),
			is_in_bounds(nc[2]),
		};

		std::array<bool, 3> n{
			volume_check(volume + ni[0]),
			volume_check(volume + ni[1]),
			volume_check(volume + ni[2]),
		};

		return std::make_tuple(nb, n, ni);
	}

	auto is_in_bounds(const vertex &v) const
	{
		return (1.0 <= v.x && v.x <= static_cast<int32_t>(width)) &&
		       (1.0 <= v.y && v.y <= static_cast<int32_t>(height)) &&
		       (1.0 <= v.z && v.z <= static_cast<int32_t>(depth));
	}

	static constexpr auto build_directions()
	{
		std::array<std::array<vector3d, 2>, 3> result;

		for (auto i = 0; i < 3; ++i) {
			result[i][0][(i + 1) % 3] = 1;
			result[i][1][(i + 2) % 3] = 1;
		}

		return result;
	};
};
} // namespace tc

#endif // WEAVER_MESHER_CULLING_HPP
