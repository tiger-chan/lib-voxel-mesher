#ifndef WEAVER_CORE_FWD_HPP
#define WEAVER_CORE_FWD_HPP

#ifndef VOXEL_MESHER_ASSERT
#include <cassert>
#define VOXEL_MESHER_ASSERT(condition) assert(condition)
#endif

namespace tc
{
template <typename data_t> struct base_vector2;

template <typename data_t> struct base_vector3;

struct quad;
} // namespace tc

#endif // WEAVER_CORE_FWD_HPP