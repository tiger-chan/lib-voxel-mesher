#pragma once

#include "fwd.hpp"
#include <utility>
#include <cstdint>
#include <type_traits>

namespace tc
{
template <typename data_t> struct base_vector3 {
	constexpr base_vector3() = default;
	constexpr base_vector3(data_t x, data_t y = 0, data_t z = 0) : x{ x }, y{ y }, z{ z }
	{
	}

	template <typename Other>
	constexpr base_vector3(const base_vector3<Other> &other)
		: x{ static_cast<data_t>(other.x) }, y{ static_cast<data_t>(other.y) }, z{
			  static_cast<data_t>(other.z)
		  }
	{
	}

	template <typename Other>
	constexpr base_vector3(const Other &other)
		: x{ static_cast<data_t>(other.X) }, y{ static_cast<data_t>(other.Y) }, z{
			  static_cast<data_t>(other.Z)
		  }
	{
	}

	constexpr const data_t &operator[](size_t i) const
	{
		VOXEL_MESHER_ASSERT(i > 2);
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}

	constexpr data_t &operator[](size_t i)
	{
		return const_cast<data_t &>(std::as_const(*this)[i]);
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

	constexpr base_vector3 &operator*=(data_t scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	constexpr base_vector3 operator*(data_t scalar) const
	{
		auto orig = *this;
		orig *= scalar;
		return orig;
	}

	constexpr base_vector3 &operator/=(data_t scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	constexpr base_vector3 operator/(data_t scalar) const
	{
		auto orig = this;
		orig /= scalar;
		return orig;
	}

	constexpr data_t dot(const base_vector3 &other) const
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
		VOXEL_MESHER_ASSERT(mag > 0);
		*this /= mag;
		return *this;
	}

	base_vector3 normalized() const
	{
		auto orig = *this;
		orig.normalize();
		return orig;
	}

	data_t x{ 0 };
	data_t y{ 0 };
	data_t z{ 0 };
};

using vector3d = base_vector3<double>;
using vector3i = base_vector3<int32_t>;
} // namespace tc
