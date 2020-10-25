#ifndef WEAVER_CORE_VOXEL_LOADER_HPP
#define WEAVER_CORE_VOXEL_LOADER_HPP

#include <string>
#include <string_view>
#include <regex>
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include "voxel_def.hpp"

namespace tc
{

static voxel_face &operator>>(const std::string &val, voxel_face &face)
{
	if (val == "north") {
		face = voxel_face::back;
	} else if (val == "south") {
		face = voxel_face::front;
	} else if (val == "east") {
		face = voxel_face::right;
	} else if (val == "west") {
		face = voxel_face::left;
	} else if (val == "top") {
		face = voxel_face::top;
	} else if (val == "bottom") {
		face = voxel_face::bottom;
	}

	return face;
}

static void to_json(nlohmann::json& j, const vector2d& v) {
	j = nlohmann::json{
		{v.x, v.y}
	};
}

static void from_json(const nlohmann::json& j, vector2d& v) {
	WEAVER_ASSERT(j.is_array());
	for (auto i = 0; i < j.size() && i < 2; ++i) {
		j.at(i).get_to(v[i]);
	}
}

static void to_json(nlohmann::json& j, const vector3d& v) {
	j = nlohmann::json{
		{v.x, v.y, v.z}
	};
}

static void from_json(const nlohmann::json& j, vector3d& v) {
	WEAVER_ASSERT(j.is_array());
	for (auto i = 0; i < j.size() && i < 3; ++i) {
		j.at(i).get_to(v[i]);
	}
}

static void to_json(nlohmann::json& j, const face_def& v) {
	j = nlohmann::json{
		{"uv_min", v.uv_min},
		{"uv_max", v.uv_max},
		{"cull", v.cull},
		{"material", v.material}
	};
}

static void from_json(const nlohmann::json& j, face_def& v) {
	if (j.contains("uv_min")) {
		j.at("uv_min").get_to(v.uv_min);
	}
	
	if (j.contains("uv_max")) {
		j.at("uv_max").get_to(v.uv_max);
	}

	if (j.contains("cull")) {
		j.at("cull").get_to(v.cull);
	}

	if (j.contains("material")) {
		j.at("material").get_to(v.material);
	}
}

static void to_json(nlohmann::json& j, const voxel_component_def& v) {
	j = nlohmann::json{
		{"min", v.min},
		{"max", v.max},
		{"translate", v.translate},
		{"face", v.faces}
	};
}

static void from_json(const nlohmann::json& j, voxel_component_def& v) {
	if (j.contains("min")) {
		j.at("min").get_to(v.min);
	}
	
	if (j.contains("max")) {
		j.at("max").get_to(v.max);
	}

	if (j.contains("translate")) {
		j.at("translate").get_to(v.translate);
	}

	if (j.contains("face")) {
		for (auto&& face: j.at("face").items()) {
			voxel_face key;
			face.key() >> key;
			if (key == voxel_face::_count) {
				continue;
			}

			auto&& f = v.faces[key];
			face.value().get_to(f);
		}
	}
}

static void to_json(nlohmann::json& j, const voxel_def& v) {
	j = nlohmann::json{
		{"components", v.components}
	};
}

static void from_json(const nlohmann::json& j, voxel_def& v) {
	if (j.contains("components")) {
		j.at("components").get_to(v.components);
	}
}

namespace weaver
{
static std::string load_file(const std::filesystem::path &path)
{
	using namespace std;
	ifstream stream(path.c_str());
	string out;

	stream.seekg(0, ios::end);
	out.reserve(stream.tellg());
	stream.seekg(0, ios::beg);

	using streambuf_t = istreambuf_iterator<char>;

	out.assign((streambuf_t(stream)), streambuf_t());

	return out;
}

static nlohmann::json load_json(const std::filesystem::path &path)
{
	std::string out = load_file(path);
	using namespace nlohmann;
	return json::parse(out.c_str());
}

static std::pair<std::string, nlohmann::json> load_voxel_file(const std::filesystem::path &path)
{
	auto name = path.stem().string();
	auto json = load_json(path);
	return std::make_pair(std::move(name), std::move(json));
}

static std::vector<nlohmann::json *>
find_hierarchy(std::unordered_map<std::string, nlohmann::json> &definitions, nlohmann::json &entry)
{
	nlohmann::json *current = &entry;
	std::vector<nlohmann::json *> results;
	while (current != nullptr) {
		results.emplace_back(current);

		auto &&parent_key = (*current)["$parent"];
		if (parent_key.is_null()) {
			return results;
		}

		current = &(definitions[parent_key.get<std::string>()]);
	}

	return results;
}

static void replace_all(std::string& str, const std::string_view& from, const std::string_view& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

static std::tuple<std::unordered_map<std::string, nlohmann::json>, std::unordered_map<std::string, tc::voxel_def>> load_voxels(const std::string &dir)
{
	namespace fs = std::filesystem;
	// This function should produce something that can be 1 used to fill a map of voxel_def objects
	// and 2 return the raw json.
	// to do that I need some form of fill function that has access to the current element and
	// all parents that are referenced, or just the parent one level heigher but create a list
	// of all parented objects and keep a count of which have the most dependent upon it.
	// and also possibly cicular dependencies.
	std::unordered_map<std::string, nlohmann::json> entries;
	for (const auto &entry : fs::directory_iterator(dir)) {
		auto it = entries.emplace(load_voxel_file(entry.path()));
		auto &&pair = it.first;
	}

	static const std::string component_face_path = "/components/${index}/face/${face}/material";
	static const std::array<std::string, 6> face_names {
		"north", "south", "east",
		"west", "top", "bottom",
	};

	std::unordered_map<std::string, voxel_def> voxels;
	voxels.reserve(entries.size());
	for (auto &&pair : entries) {
		auto &&voxel_json = pair.second;
		auto hierarchy = find_hierarchy(entries, voxel_json);
		auto filled = *hierarchy.back();
		for (size_t i = hierarchy.size() - 1; i < hierarchy.size(); --i) {
			auto&& json = *hierarchy[i];

			auto components = filled["components"];
			WEAVER_ASSERT(components.is_array());
			auto begin_it = std::begin(components);

			auto&& materials = json["materials"];
			for (auto comp_it = begin_it; comp_it != std::end(components); ++comp_it)
			{
				auto index = comp_it - begin_it;
				auto component_path = component_face_path;
				replace_all(component_path, "${index}", std::to_string(index));
				for (auto&& face_key: face_names) {
					auto face_path = component_path;

					if (materials.is_object()) {
						replace_all(face_path, "${face}", face_key);

						for (auto&& material: materials.items()) {
							nlohmann::json patch = R"([
								{ "op": "test" },
								{ "op": "replace" }
							])"_json;
							
							auto&& key = material.key();
							auto&& val = material.value();

							patch[0]["path"] = face_path;
							patch[0]["value"] = "${" + key + "}";


							patch[1]["path"] = face_path;
							patch[1]["value"] = val.get<std::string>();

							filled = filled.patch(patch);
						}
					}
				}
			}
		}

		voxel_def def = filled;
		voxels.emplace(pair.first, std::move(def));
	}

	return std::make_tuple(std::move(entries), std::move(voxels));
}
} // namespace weaver
} // namespace tc

#endif // WEAVER_CORE_VOXEL_LOADER_HPP
