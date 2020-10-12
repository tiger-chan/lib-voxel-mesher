#ifndef WEAVER_MESHER_SIMPLE_HPP
#define WEAVER_MESHER_SIMPLE_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "fwd.hpp"
#include "voxel_reader.hpp"
#include "mesher_result.hpp"
#include <array>

namespace tc
{
template<typename Type>
class WEAVER_API simple {
	template<typename T>
	using reader_t = weaver::voxel_reader<T>;
	enum boundry { r = 0, f = 1, u = 2, count = 3 };

    public:

	template <typename Iter>
	mesher_result eval(Iter volume_begin, Iter volume_end) const
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

			return work(std::begin(volume), std::end(volume), reader_t<Type*>{});
		} else {
			return work(volume_begin, volume_end, reader_t<Type>{});
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

		std::vector<int32_t> vert_map{};
		mesher_result result;
		auto &vertices = result.vertices;
		auto &quads = result.quads;

		const auto total_vert_count{ (height + 2) * (width + 2) * (depth + 2) };
		vert_map.resize(total_vert_count, -1);
		vertices.reserve(total_vert_count);
		quads.reserve(height * width * depth * 6);

		auto volume_check = [reader = reader, e = volume_end](auto c) {
			if (c == e) {
				return false;
			}

			return reader.visible(*c);
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

		auto calc_vert_index = [w = width + 1, h = height + 1](const auto &vert) {
			vector3i i{ vert };
			return i.z * w * h + i.y * w + i.x;
		};

		vertex vert;
		auto volume = volume_begin;
		for (vert.z = 0; vert.z < bd; ++vert.z) {
			for (vert.y = 0; vert.y < bh; ++vert.y) {
				for (vert.x = 0; vert.x < bw; ++vert.x, ++volume) {
					auto vi = volume - volume_begin;
					const bool in_bounds = is_in_bounds(vert);
					auto state = volume_check(volume);

					if (!in_bounds || !state) {
						// skip if it's not in bounds or not visable
						continue;
					}

					auto type_id = reader(*volume);

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

							quad q{};
							q.normal = normal;
							q.uv = uv;
							q.type_id = type_id;

							for (int i = 0; i < 4; ++i) {
								q[i] = calc_vert_index(tmp_quad[i]);
								insert_vert(q[i], tmp_quad[i]);
							}

							quads.emplace_back(std::move(q));
						}
					}
				}
			}
		}

		return result;
	}

	private:
	auto calc_index(const vertex &vert) const
	{
		vector3i i{ vert };
		return i.z * (width + 2) * (height + 2) + i.y * (width + 2) + i.x;
	};

	auto is_in_bounds(const vertex &v) const
	{
		return (1.0 <= v.x && v.x <= static_cast<int32_t>(width)) &&
		       (1.0 <= v.y && v.y <= static_cast<int32_t>(height)) &&
		       (1.0 <= v.z && v.z <= static_cast<int32_t>(depth));
	}
};
} // namespace tc

#endif // WEAVER_MESHER_SIMPLE_HPP
