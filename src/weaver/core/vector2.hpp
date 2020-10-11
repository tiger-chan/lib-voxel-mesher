#ifndef WEAVER_CORE_VECTOR2_HPP
#define WEAVER_CORE_VECTOR2_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "fwd.hpp"
#include <utility>
#include <cstdint>
#include <type_traits>

namespace tc
{
template <typename data_t> struct WEAVER_API base_vector2 {
	constexpr base_vector2() = default;

	constexpr base_vector2(data_t x, data_t y) : x{ x }, y{ y }
	{
	}

	template <typename Other>
	constexpr base_vector2(const base_vector2<Other> &other)
		: x{ static_cast<data_t>(other.x) }, y{ static_cast<data_t>(other.y) }
	{
	}

	template <typename Other>
	constexpr base_vector2(const Other &other)
		: x{ static_cast<data_t>(other.X) }, y{ static_cast<data_t>(other.Y) }
	{
	}

	constexpr const data_t &operator[](size_t i) const
	{
		WEAVER_ASSERT(i > 1);
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		}
	}

	constexpr data_t &operator[](size_t i)
	{
		return const_cast<data_t &>(std::as_const(*this)[i]);
	}

	constexpr base_vector2 &operator+=(const base_vector2 &other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	constexpr base_vector2 operator+(const base_vector2 &other) const
	{
		auto orig = *this;
		return orig += other;
	}

	constexpr base_vector2 &operator-=(const base_vector2 &other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	constexpr base_vector2 operator-(const base_vector2 &other) const
	{
		auto orig = *this;
		return orig -= other;
	}

	constexpr base_vector2 &operator*=(data_t scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	constexpr base_vector2 operator*(data_t scalar) const
	{
		auto orig = *this;
		orig *= scalar;
		return orig;
	}

	constexpr base_vector2 &operator/=(data_t scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	constexpr base_vector2 operator/(data_t scalar) const
	{
		auto orig = this;
		orig /= scalar;
		return orig;
	}

	constexpr data_t dot(const base_vector2 &other) const
	{
		return x * other.x + y * other.y;
	}

	constexpr base_vector2 cross(const base_vector2 &other) const
	{
		auto orig = *this;
		orig.x = (y * other.z) - (z * other.y);
		orig.y = (x * other.z) - (z * other.x);
		return orig;
	}

	constexpr double magnitude_sqrd() const
	{
		return x * x + y * y;
	}

	double magnitude() const
	{
		auto mag = magnitude_sqrd();
		return mag > 0.0 ? sqrt(mag) : 0;
	}

	base_vector2 &normalize()
	{
		auto mag = magnitude();
		if (mag == 0.0) {
			return *this;
		}

		*this /= mag;
		return *this;
	}

	base_vector2 &normalize_quick()
	{
		auto mag = magnitude();
		WEAVER_ASSERT(mag > 0);
		*this /= mag;
		return *this;
	}

	base_vector2 normalized() const
	{
		auto orig = *this;
		orig.normalize();
		return orig;
	}

	data_t x{ 0 };
	data_t y{ 0 };
};

using vector2d = base_vector2<double>;
using vector2f = base_vector2<float>;
using vector2i = base_vector2<int32_t>;
} // namespace tc

#endif // WEAVER_CORE_VECTOR2_HPP
