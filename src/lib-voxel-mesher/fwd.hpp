#pragma once

#ifndef VOXEL_MESHER_ASSERT
#include <cassert>
#define VOXEL_MESHER_ASSERT(condition) assert(condition)
#endif

namespace tc
{
template <typename data_t> struct base_vector2;

template <typename data_t> struct base_vector3;

struct quad;
struct mesher_result;
} // namespace tc
