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

namespace tc
{
namespace woven
{
using decimal_t = WEAVER_DECIMAL_TYPE;
}
} // namespace tc

#endif // WEAVER_CONFIG_CONFIG_HPP
