#ifndef K3DSDK_ALMOST_EQUAL_H
#define K3DSDK_ALMOST_EQUAL_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <k3d-platform-config.h>
#include "types.h"
#include <boost/static_assert.hpp>

namespace k3d
{

/// Functor for testing objects for "loose" equality - specializations for exact types including integers and strings test for exact equality, while specializations for floating-point types test for equality within a caller-specified tolerance
template<typename T>
class almost_equal
{
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const
	{
		// This will be triggered if this template is ever instantiated
		BOOST_STATIC_ASSERT(sizeof(T) == 0);
		return false;
	}
};

/// Specialization of almost_equal that tests bool_t for equality
template<>
class almost_equal<bool_t>
{
	typedef bool_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests int8_t for equality
template<>
class almost_equal<int8_t>
{
	typedef int8_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests int16_t for equality
template<>
class almost_equal<int16_t>
{
	typedef int16_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests int32_t for equality
template<>
class almost_equal<int32_t>
{
	typedef int32_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests int64_t for equality
template<>
class almost_equal<int64_t>
{
	typedef int64_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests uint8_t for equality
template<>
class almost_equal<uint8_t>
{
	typedef uint8_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests uint16_t for equality
template<>
class almost_equal<uint16_t>
{
	typedef uint16_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests uint32_t for equality
template<>
class almost_equal<uint32_t>
{
	typedef uint32_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests uint64_t for equality
template<>
class almost_equal<uint64_t>
{
	typedef uint64_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests two double_t values for near-equality - based on "Comparing floating point numbers" by Bruce Dawson
template<>
class almost_equal<double_t>
{
	typedef double_t T;

public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline bool_t operator()(const T A, const T B) const
	{
		return representable_difference(A, B) <= threshold;
	}

private:
	/// Convert a double_t to a lexicographically-ordered twos-complement integer
	inline static int64_t to_integer(const double_t Value)
	{
		const int64_t value = *(int64_t*)&Value;
		return value < 0 ? 0x8000000000000000LL - value : value;
	}

	/// Given two double_t, returns their difference expressed as the number of uniquely-representable floating-point values that separate them
	inline static uint64_t representable_difference(const double_t A, const double_t B)
	{
		const int64_t a = to_integer(A);
		const int64_t b = to_integer(B);
		return a > b ? a - b : b - a;
	}

	const uint64_t threshold;
};

/// Specialization of almost_equal that tests string_t for equality
template<>
class almost_equal<string_t>
{
	typedef string_t T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

#ifdef K3D_API_DARWIN

/// Specialization of almost_equal for use with unsigned long (required on OSX)
/// \deprecated New code must use the sized K-3D types instead of unsigned long
template<>
class almost_equal<unsigned long>
{
	typedef unsigned long T;
public:
	almost_equal(const uint64_t) { }
	inline bool_t operator()(const T A, const T B) const { return A == B; }
};

#endif // K3D_API_DARWIN

} // namespace k3d

#endif // !K3DSDK_ALMOST_EQUAL_H

