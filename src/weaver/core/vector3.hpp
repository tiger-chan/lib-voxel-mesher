#ifndef WEAVER_CORE_VECTOR3_HPP
#define WEAVER_CORE_VECTOR3_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "algorithm.hpp"
#include "fwd.hpp"
#include <utility>
#include <cstdint>
#include <type_traits>

namespace tc
{
template <typename Data> struct WEAVER_API base_vector3 {
	constexpr base_vector3() = default;
	explicit constexpr base_vector3(Data x, Data y = 0, Data z = 0) : x{ x }, y{ y }, z{ z }
	{
	}

	template <typename Other>
	constexpr base_vector3(const base_vector3<Other> &other)
		: x{ static_cast<Data>(other.x) }, y{ static_cast<Data>(other.y) }, z{
			  static_cast<Data>(other.z)
		  }
	{
	}

	template <typename Other>
	explicit constexpr base_vector3(const Other &other)
		: x{ static_cast<Data>(other.X) }, y{ static_cast<Data>(other.Y) }, z{
			  static_cast<Data>(other.Z)
		  }
	{
	}

	constexpr const Data &operator[](size_t i) const
	{
		WEAVER_ASSERT(i > 2);
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}

		// cannot happen
		return x;
	}

	constexpr Data &operator[](size_t i)
	{
		return const_cast<Data &>(std::as_const(*this)[i]);
	}

	constexpr base_vector3 operator-() const
	{
		return base_vector3{ -x, -y, -z };
	}

	constexpr base_vector3 &operator+=(const Data &scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;

		return *this;
	}

	constexpr base_vector3 &operator-=(const Data &scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;

		return *this;
	}

	constexpr base_vector3 &operator/=(const Data &scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;

		return *this;
	}

	constexpr base_vector3 &operator*=(const Data &scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	constexpr base_vector3 &operator+(const Data &scalar) const
	{
		auto orig = *this;
		return orig *= scalar;
	}

	constexpr base_vector3 &operator-(const Data &scalar) const
	{
		auto orig = *this;
		return orig -= scalar;
	}

	constexpr base_vector3 &operator/(const Data &scalar) const
	{
		auto orig = *this;
		return orig /= scalar;
	}

	constexpr base_vector3 &operator*(const Data &scalar) const
	{
		auto orig = *this;
		return orig *= scalar;
	}

	constexpr base_vector3 &operator+=(const base_vector3 &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	constexpr base_vector3 operator+(const base_vector3 &other) const
	{
		auto orig = *this;
		return orig += other;
	}

	constexpr base_vector3 &operator-=(const base_vector3 &other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	constexpr base_vector3 operator-(const base_vector3 &other) const
	{
		auto orig = *this;
		return orig -= other;
	}

	constexpr base_vector3 &operator*=(const base_vector3 &other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;

		return *this;
	}

	constexpr base_vector3 operator*(const base_vector3 &other) const
	{
		auto orig = *this;
		return orig *= other;
	}

	constexpr base_vector3 &operator/=(const base_vector3 &other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;

		return *this;
	}

	constexpr base_vector3 operator/(const base_vector3 &other) const
	{
		auto orig = *this;
		return orig /= other;
	}

	constexpr Data dot(const base_vector3 &other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	constexpr base_vector3 cross(const base_vector3 &other) const
	{
		auto orig = *this;
		orig.x = (y * other.z) - (z * other.y);
		orig.y = (x * other.z) - (z * other.x);
		orig.z = (x * other.y) - (y * other.x);
		return orig;
	}

	constexpr double magnitude_sqrd() const
	{
		return x * x + y * y + z * z;
	}

	double magnitude() const
	{
		auto mag = magnitude_sqrd();
		return mag > 0.0 ? sqrt(mag) : 0;
	}

	base_vector3 &normalize()
	{
		auto mag = magnitude();
		if (mag == 0.0) {
			return *this;
		}

		*this /= mag;
		return *this;
	}

	base_vector3 &normalize_quick()
	{
		auto mag = magnitude();
		WEAVER_ASSERT(mag > 0);
		*this /= mag;
		return *this;
	}

	base_vector3 normalized() const
	{
		auto orig = *this;
		orig.normalize();
		return orig;
	}

	Data x{ 0 };
	Data y{ 0 };
	Data z{ 0 };
};

using vector3d = base_vector3<double>;
using vector3i = base_vector3<int32_t>;

template<typename Data, typename Numeric, std::enable_if_t<std::is_arithmetic_v<Numeric>, int32_t> = 0>
static base_vector3<Data> operator-(const Numeric& n, const base_vector3<Data>& v) {
	auto o = -v;
	return o += n;
}

template<typename Data, typename Numeric, std::enable_if_t<std::is_arithmetic_v<Numeric>, int32_t> = 0>
static base_vector3<Data> operator+(const Numeric& n, const base_vector3<Data>& v) {
	auto o = v;
	return o += n;
}

template<typename Data, typename Numeric, std::enable_if_t<std::is_arithmetic_v<Numeric>, int32_t> = 0>
static base_vector3<Data> operator*(const Numeric& n, const base_vector3<Data>& v) {
	auto o = v;
	return o *= n;
}

template<typename Data, typename Numeric, std::enable_if_t<std::is_arithmetic_v<Numeric>, int32_t> = 0>
static base_vector3<Data> operator/(const Numeric& n, const base_vector3<Data>& v) {
	auto o = v;
	o.x = n / o.x;
	o.y = n / o.y;
	o.z = n / o.z;
	return o = n;
}

template<typename Data>
static base_vector3<Data> clamp(const base_vector3<Data>& v, const base_vector3<Data>& min, const base_vector3<Data>& max)
{
	base_vector3<Data> r{};
	r.x = weaver::clamp(v.x, min.x, max.x);
	r.y = weaver::clamp(v.y, min.y, max.y);
	r.z = weaver::clamp(v.z, min.z, max.z);
	return r;
}
} // namespace tc

#endif // WEAVER_CORE_VECTOR3_HPP
