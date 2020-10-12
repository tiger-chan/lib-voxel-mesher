#ifndef WEAVER_MESHER_VOXEL_READER_HPP
#define WEAVER_MESHER_VOXEL_READER_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"

namespace tc
{
	namespace weaver
	{
		template<typename Type>
		struct WEAVER_API voxel_reader
		{
			bool visible(const Type&) const { return false; }
			voxel_id_t operator()(const Type&) const { return unset_voxel_id; }
		};

		template<typename Type>
		struct voxel_reader<Type*>
		{
			inline bool visible(const Type* v) const
			{
				return v == nullptr ? false : reader.visible(*v);
			}
			inline voxel_id_t operator()(const Type* v) const
			{
				return v == nullptr ? unset_voxel_id : reader(*v);
			}
			voxel_reader<Type> reader{};
		};
	}
}

#endif // WEAVER_MESHER_VOXEL_READER_HPP
