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

					static const vertex remove_border{ 1.0, 1.0, 1.0 };

					add_quads(vert - remove_border, quads, volume, reader);
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

	template <typename Iter, typename T>
	auto add_quads(const vertex &vert, std::vector<quad> &quads, Iter current_vox, reader_t<T>& reader) const
	{
		auto faces = cube_faces;

		for (auto d = 0; d < 3; ++d) {
			for (auto side = 0; side < 2; ++side) {
				auto state = side == 1;
				
				auto index = d + (state ? 0 : 3);
				auto face = faces[index];
				face.normal.normalize_quick();
				for (auto& v: face) {
					v += vert;
				}

				face.type_id = reader(*current_vox);

				quads.emplace_back(face);
			}
		}
	}
};
} // namespace tc

#endif // WEAVER_MESHER_SIMPLE_HPP
