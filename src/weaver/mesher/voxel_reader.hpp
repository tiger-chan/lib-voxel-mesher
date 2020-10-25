#ifndef WEAVER_MESHER_VOXEL_READER_HPP
#define WEAVER_MESHER_VOXEL_READER_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/fwd.hpp"
#include "../core/quad.hpp"
#include "../core/voxel_face.hpp"
#include "voxel_face_result.hpp"

namespace tc
{
namespace weaver
{
template <typename Type> struct WEAVER_API voxel_reader {
	bool visible(const Type &) const
	{
		return false;
	}

	voxel_id_t operator()(const Type &) const
	{
		return unset_voxel_id;
	}

	std::vector<voxel_face_result> operator()(const Type &, voxel_face vf) const
	{
		return std::vector<voxel_face_result>{ voxel_face_result{} };
	}
};

template <typename Type> struct voxel_reader<Type *> {
	inline bool visible(const Type *v) const
	{
		return v == nullptr ? false : reader.visible(*v);
	}

	inline voxel_id_t operator()(const Type *v) const
	{
		return v == nullptr ? unset_voxel_id : reader(*v);
	}

	inline std::vector<voxel_face_result> operator()(const Type *v, voxel_face vf) const
	{
		return v == nullptr ? std::vector<voxel_face_result>{ voxel_face_result{} } : reader(*v, vf);
	}

	voxel_reader<Type> reader{};
};
} // namespace weaver
} // namespace tc

#endif // WEAVER_MESHER_VOXEL_READER_HPP
