#pragma once

#include "fwd.hpp"
#include "vertex.hpp"
#include "mesher_result.hpp"
#include <array>

namespace tc
{
class culling_voxel_mesher {
	enum boundry { r = 0, f = 1, u = 2, count = 3 };

    public:
	template <typename Iter> inline mesher_result eval(Iter volume_begin, Iter volume_end) const
	{
		return eval(std::forward<Iter>(volume_begin), std::forward<Iter>(volume_end),
			    [](const auto &i) { return !!(*i); });
	}

	template <typename Iter, typename Predicate>
	mesher_result eval(Iter volume_begin, Iter volume_end, Predicate &&pred) const
	{
		static constexpr auto dir = build_directions();
		int32_t d_w{ static_cast<int32_t>(width) };
		int32_t d_h{ static_cast<int32_t>(height) };
		int32_t d_d{ static_cast<int32_t>(depth) };
		int32_t y_offset{ d_w };
		int32_t z_offset{ d_w * d_h };
		int32_t total_size{ z_offset * d_d };

		std::vector<int32_t> vert_map{};
		mesher_result result;
		auto &vertices = result.vertices;
		auto &quads = result.quads;

		const auto total_vert_count{ (height + 1) * (width + 1) * (depth + 1) };
		vert_map.resize(total_vert_count, -1);
		vertices.reserve(total_vert_count);
		quads.reserve(height * width * depth * 6);

		auto is_in_bounds = [w = d_w, h = d_h, d = d_d](const auto &v) {
			return (0.0 <= v.x && v.x < w) && (0.0 <= v.y && v.y < h) &&
			       (0.0 <= v.z && v.z < d);
		};

		auto volume_check = [pred, b = volume_begin, e = volume_end](auto pred, auto c) {
			if (c < b || c >= e) {
				return false;
			}

			return pred(c);
		};

		auto insert_vert = [&vert_map, &vertices](auto &key, const auto &vert) {
			auto &map_key = vert_map[key];
			if (map_key >= 0) {
				key = map_key;
				return;
			}

			map_key = key = vertices.size();
			vertices.emplace_back(vert);
		};

		// move the begining this check to to a layer below the start of the
		// section so we can draw the bottoms of the voxels

		// anytime there is a change from current voxel's state to another state
		// on a neighbor that means we need to render something.
		// e.g. if the voxel isn't present in <0, 0, -1>, but <0, 0, 0> is
		// when we check <0, 0, -1> for it's neighbors we see that <0, 0, -1> is unset
		// and <0, 0, 0> is set so there needs to be quad placed on the boundry between

		vertex vert;
		auto volume = volume_begin;
		// Iterate from one layer below the region
		// to the top layer
		for (vert.z = -1; vert.z < depth; ++vert.z) {
			// Iterate from one before the region
			// to the furthest back
			for (vert.y = -1; vert.y < height; ++vert.y) {
				// Iterate from one before the region
				// to the furthest right
				for (vert.x = -1; vert.x < width; ++vert.x) {
					const bool in_bounds = is_in_bounds(vert);
					auto state = in_bounds && volume_check(pred, volume);

					std::array<vector3d, 3> nc{
						vert + vertex{ 1.0 }, vert + vertex{ 0.0, 1.0 },
						vert + vertex{ 0.0, 0.0, 1.0 }
					};

					std::array<int32_t, 3> ni{
						calc_index(nc[0]),
						calc_index(nc[1]),
						calc_index(nc[2]),
					};

					std::array<bool, 3> n{
						is_in_bounds(nc[0]) &&
							volume_check(pred, volume_begin + ni[0]),
						is_in_bounds(nc[1]) &&
							volume_check(pred, volume_begin + ni[1]),
						is_in_bounds(nc[2]) &&
							volume_check(pred, volume_begin + ni[2]),
					};

					for (auto d = 0; d < boundry::count; ++d) {
						if (state == n[d]) {
							// there hasn't been a change in state in this direction
							continue;
						}

						add_quad(d, state, vert, quads, insert_vert);
					}

					if (in_bounds) {
						++volume;
					}
				}
			}
		}

		return result;
	}

	size_t width{ 0 };
	size_t height{ 0 };
	size_t depth{ 0 };
	bool draw_at_boundry{ false };

    private:
	template <typename VertInsert>
	auto add_quad(int32_t direction, bool state, const vertex &vert,
			     std::vector<quad> &quads, VertInsert vert_insert) const 
	{
		constexpr auto dir = build_directions();

		auto calc_vert_index = [w = width + 1, h = height + 1](const auto &vert) {
			vector3i i{ vert };
			return i.z * w * h + i.y * w + i.x;
		};

		auto tmp = vert;
		auto u = dir[direction][0];
		auto v = dir[direction][1];
		++tmp[direction];

		std::array<vertex, 4> verts{};
		verts[0] = tmp; // vert
		verts[1] = verts[0] + u; // vert + u
		verts[2] = verts[1] + v; // vert + u + v
		verts[3] = verts[0] + v; // vert + v

		if (!state) {
			std::iter_swap(std::begin(verts), std::begin(verts) + 3);
			std::iter_swap(std::begin(verts) + 1, std::begin(verts) + 2);
		}

		std::array<vector2d, 4> uv{
			vector2d{ 0.0, 0.0 },
			vector2d{ 1.0, 0.0 },
			vector2d{ 1.0, 1.0 },
			vector2d{ 0.0, 1.0 },
		};

		auto cb = verts[2] - verts[1];
		auto ab = verts[0] - verts[1];
		auto normal = cb.cross(ab);
		cb.normalize_quick();

		quad q{};
		q.normal = normal;
		q.uv = uv;

		for (auto i = 0; i < 4; ++i) {
			q[i] = calc_vert_index(verts[i]);
			vert_insert(q[i], verts[i]);
		}

		quads.emplace_back(std::move(q));
	}

	auto calc_index(const vertex &vert) const {
		vector3i i{ vert };
		return i.z * width * height + i.y * width + i.x;
	};

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
