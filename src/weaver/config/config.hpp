#ifndef WEAVER_CONFIG_CONFIG_HPP
#define WEAVER_CONFIG_CONFIG_HPP

#ifndef WEAVER_NOEXCEPT
#	define WEAVER_NOEXCEPT noexcept
#endif

#ifndef WEAVER_ASSERT
#	include <cassert>
#	define WEAVER_ASSERT(condition) assert(condition)
#endif

#ifndef WEAVER_DECIMAL_TYPE
#	define WEAVER_DECIMAL_TYPE double
#endif

#ifndef WEAVER_VOXEL_ID_TYPE
#	include <cstdint>
#	define WEAVER_VOXEL_ID_TYPE uint32_t
#endif

#ifndef WEAVER_VOXEL_UNSET_ID
#	include <cstdint>
#	define WEAVER_VOXEL_UNSET_ID UINT32_MAX
#endif

namespace tc
{
namespace weaver
{
using decimal_t = WEAVER_DECIMAL_TYPE;
using voxel_id_t = WEAVER_VOXEL_ID_TYPE;

static constexpr voxel_id_t unset_voxel_id{ WEAVER_VOXEL_UNSET_ID };
} /// namespace weaver
} // namespace tc

#endif // WEAVER_CONFIG_CONFIG_HPP
