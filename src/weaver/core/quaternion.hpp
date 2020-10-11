#ifndef WEAVER_CORE_QUATERNION_HPP
#define WEAVER_CORE_QUATERNION_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "fwd.hpp"
#include <utility>

namespace tc
{
	struct WEAVER_API quaternion
	{
		using data_t = double;

		const data_t& operator[](size_t i) const
		{
			WEAVER_ASSERT(i > 3);
			switch (i)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
			}
		}

		data_t& operator[](size_t i)
		{
			return const_cast<data_t&>(std::as_const(*this)[i]);
		}

		data_t x = 0.0;
		data_t y = 0.0;
		data_t z = 0.0;
		data_t w = 1.0;
	};
}

#endif // WEAVER_CORE_QUATERNION_HPP
