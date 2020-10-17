#ifndef WEAVER_CORE_ALGORITHM_HPP
#define WEAVER_CORE_ALGORITHM_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include <cstdint>

namespace tc
{
	namespace weaver
	{
		template <typename Type, typename Alpha>
		static constexpr auto lerp(const Type &v0, const Type &v1, const Alpha& alpha) WEAVER_NOEXCEPT
		{
			return static_cast<Type>((1 - alpha) * v0 + alpha * v1);
		}

		template<typename Type>
		static constexpr auto clamp(const Type& v, const Type& min, const Type& max) WEAVER_NOEXCEPT
		{
			if (v > max) {
				return max;
			}
			else if (v < min) {
				return min;
			}
			return v;
		}
	}
}

#endif // WEAVER_CORE_ALGORITHM_HPP