#ifndef WEAVER_MESHER_SIMPLE_VOXEL_MESHER_HPP
#define WEAVER_MESHER_SIMPLE_VOXEL_MESHER_HPP


#include "../core/fwd.hpp"
#include "../core/vertex.hpp"
#include "mesher_result.hpp"
#include <array>

namespace tc
{
class simple_voxel_mesher {
    public:
	template <typename Iter>
	inline mesher_result eval(Iter volume_begin, Iter volume_end) const
	{
		return eval(std::forward<Iter>(volume_begin), std::forward<Iter>(volume_end), [](const auto &i) { return !!(*i); });
	}

	template <typename Iter, typename Predicate>
	mesher_result eval(Iter volume_begin, Iter volume_end, Predicate &&p) const
	{
		using vert_t = base_vertex<int32_t>;
		mesher_result result;

		auto calc_index = [w = width + 1, h = height + 1](const auto &vert) {
			return static_cast<int32_t>(vert.z) * w * h +
			       static_cast<int32_t>(vert.y) * w + static_cast<int32_t>(vert.x);
		};

		auto &vertices = result.vertices;
		auto &quads = result.quads;

		std::vector<int32_t> vert_map{};

		const auto total_vert_count{ (height + 1) * (width + 1) * (depth + 1) };

		vert_map.resize(total_vert_count, -1);
		vertices.reserve(total_vert_count);
		quads.reserve(height * width * depth * 6);

		auto cur = volume_begin;
		vert_t vert;
		for (vert[2] = 0; vert[2] < depth; ++vert[2]) {
			for (vert[1] = 0; vert[1] < height; ++vert[1]) {
				for (vert[0] = 0; vert[0] < width; ++vert[0]) {
					// if the current voxel shouldn't be shown continue;
					if (!p(cur++)) {
						continue;
					}

					// since these are voxels each side is the same as the opposite side
					// so we iterate only three sides and duplicate the quads on each
					for (auto d = 0; d < 3; ++d) {
						std::array<vertex, 4> tmp_quad{};
						vertex tmp{ vert };
						vertex u{};
						vertex v{};
						u[(d + 1) % 3] = 1.0;
						v[(d + 2) % 3] = 1.0;

						for (auto side = 0; side < 2; ++side) {
							tmp[d] = vert[d] + side;
							tmp_quad[0] = tmp;
							tmp_quad[1] = tmp_quad[0] + u;
							tmp_quad[2] = tmp_quad[0] + u + v;
							tmp_quad[3] = tmp_quad[0] + v;

							std::array<vector2d, 4> uv{
								vector2d{ 0.0, 0.0 },
								vector2d{ 1.0, 0.0 },
								vector2d{ 1.0, 1.0 },
								vector2d{ 0.0, 1.0 },
							};

							if (side == 0) {
								std::iter_swap(
									std::begin(tmp_quad),
									std::begin(tmp_quad) + 3);
								std::iter_swap(
									std::begin(tmp_quad) + 1,
									std::begin(tmp_quad) + 2);
							}

							auto cb = tmp_quad[2] - tmp_quad[1];
							auto ab = tmp_quad[0] - tmp_quad[1];
							auto normal = cb.cross(ab);
							cb.normalize_quick();

							auto insert_vert = [&vert_map, &vertices](
										   auto &key,
										   auto &map_key,
										   auto vert) {
								if (map_key >= 0) {
									key = map_key;
									return;
								}

								map_key = key = vertices.size();
								vertices.emplace_back(
									std::move(vert));
							};

							quad q{};
							q.normal = normal;
							q.uv = uv;

							for (int i = 0; i < 4; ++i) {
								q[i] = calc_index(tmp_quad[i]);
								insert_vert(q[i], vert_map[q[i]],
									    tmp_quad[i]);
							}

							quads.emplace_back(std::move(q));
						}
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
};
} // namespace tc

#endif // WEAVER_MESHER_SIMPLE_VOXEL_MESHER_HPP
