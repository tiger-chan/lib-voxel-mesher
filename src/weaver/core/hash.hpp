#ifndef WEAVER_CORE_HASH_HPP
#define WEAVER_CORE_HASH_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include <cstdint>
#include <string_view>

namespace tc
{
	namespace weaver
	{
		namespace internal
		{
			template<typename> struct fnv;

			template<> struct WEAVER_API fnv<uint32_t> {
				static constexpr uint32_t prime = 0x01000193;
				static constexpr uint32_t offset = 0x811c9dc5;
			};

			template<> struct WEAVER_API fnv<uint64_t> {
				static constexpr uint64_t prime = 0x00000100000001B3;
				static constexpr uint64_t offset = 0xcbf29ce484222325;
			};
		}

		template<typename Char, typename Size = uint64_t>
		WEAVER_API static constexpr Size fnv1a(const Char* str) {
			auto h = internal::fnv<Size>::offset;
			while (*str)
			{
				h ^= static_cast<Size>(*str++);
				h *= internal::fnv<Size>::prime;
			}

			return h;
		}

		template<typename Char, typename Size = uint64_t>
		WEAVER_API static constexpr Size fnv1a(std::basic_string_view<Char> str) {
			return fnv1a<Char, Size>(str.data());
		}
	}
}

#endif // WEAVER_CORE_HASH_HPP
