#ifndef WEAVER_MESHER_CULLING_VOXEL_MESHER_HPP
#define WEAVER_MESHER_CULLING_VOXEL_MESHER_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "fwd.hpp"
#include "mesher_result.hpp"
#include <array>

namespace tc
{
class WEAVER_API culling_voxel_mesher {
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
		if (add_border) {
			int32_t dw{ static_cast<int32_t>(width) };
			int32_t dh{ static_cast<int32_t>(height) };
			int32_t dd{ static_cast<int32_t>(depth) };
			int32_t bw{ dw + 2 };
			int32_t bh{ dh + 2 };
			int32_t bd{ dd + 2 };

			std::vector<Iter> volume;
			volume.resize(bw * bh * bd, volume_end);
			for (auto z = 0; z < dd; ++z) {
				auto zb = (z + 1) * bh * bw;
				auto zi = z * dh * dw;
				for (auto y = 0; y < dh; ++y) {
					auto yb = (y + 1) * bh + zb;
					auto yi = y * dh + zi;
					for (auto x = 0; x < dw; ++x) {
						auto b = (x + 1) + yb;
						auto i = x + yi;
						volume[b] = volume_begin + i;
					}
				}
			}

			return work(std::begin(volume), std::end(volume),
				    [&pred, e = std::end(volume)](auto it) {
					    if (it == e) {
						    return false;
					    }
					    return pred(*it);
				    });
		} else {
			return work(volume_begin, volume_end, std::forward<Predicate>(pred));
		}
	}

	size_t width{ 0 };
	size_t height{ 0 };
	size_t depth{ 0 };
	bool add_border{ false };

    private:
	template <typename Iter, typename Predicate>
	mesher_result work(Iter volume_begin, Iter volume_end, Predicate &&pred) const
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
		auto &vertices = result.vertices;
		auto &quads = result.quads;

		const auto total_vert_count{ (height + 2) * (width + 2) * (depth + 2) };
		vert_map.resize(total_vert_count, -1);
		vertices.reserve(total_vert_count);
		quads.reserve(height * width * depth * 6);

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
		for (vert.z = 0; vert.z < bd; ++vert.z) {
			for (vert.y = 0; vert.y < bh; ++vert.y) {
				for (vert.x = 0; vert.x < bw; ++vert.x, ++volume) {
					auto vi = volume - volume_begin;
					const bool in_bounds = is_in_bounds(vert);
					auto state = volume_check(pred, volume);

					auto &&[nb, n] = find_boundries(vert, volume_begin, pred,
									volume_check);

					for (auto d = 0; d < boundry::count; ++d) {
						if (state == n[d]) {
							// there hasn't been a change in state in this direction
							continue;
						}

						if (!in_bounds && !nb[d]) {
							continue;
						}

						static const vertex remove_border{ 1.0, 1.0, 1.0 };

						add_quad(d, state, vert - remove_border, quads,
							 insert_vert);
					}
				}
			}
		}

		return result;
	}

	template <typename VertInsert>
	auto add_quad(int32_t direction, bool state, const vertex &vert, std::vector<quad> &quads,
		      VertInsert vert_insert) const
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

		quads.emplace_back(q);
	}

	auto calc_index(const vertex &vert) const
	{
		vector3i i{ vert };
		return i.z * (width + 2) * (height + 2) + i.y * (width + 2) + i.x;
	};

	template <typename Iter, typename Predicate, typename VolumeCheck>
	auto find_boundries(const vertex &vert, Iter volume, Predicate &&pred,
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
			volume_check(pred, volume + ni[0]),
			volume_check(pred, volume + ni[1]),
			volume_check(pred, volume + ni[2]),
		};

		return std::make_tuple(nb, n);
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

#endif // WEAVER_MESHER_CULLING_VOXEL_MESHER_HPP
