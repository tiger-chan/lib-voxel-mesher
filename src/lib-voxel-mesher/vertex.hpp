#pragma once

#include "fwd.hpp"
#include <utility>
#include "vector3.hpp"

namespace tc
{
template <typename data_t> using base_vertex = base_vector3<data_t>;

using vertex = base_vertex<double>;
} // namespace tc
