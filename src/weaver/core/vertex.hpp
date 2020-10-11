#ifndef WEAVER_CORE_VERTEX_HPP
#define WEAVER_CORE_VERTEX_HPP

#include "fwd.hpp"
#include <utility>
#include "vector3.hpp"

namespace tc
{
template <typename data_t> using base_vertex = base_vector3<data_t>;

using vertex = base_vertex<double>;
} // namespace tc

#endif // WEAVER_CORE_VERTEX_HPP
