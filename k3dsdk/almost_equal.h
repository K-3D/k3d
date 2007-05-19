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

#include <boost/cstdint.hpp>

namespace k3d
{

/// Functor for testing objects for equality - based on "Comparing floating point numbers" by Bruce Dawson
template<typename T>
class almost_equal
{
};

/// Specialization of almost_equal that tests bool for equality
template<>
class almost_equal<bool>
{
	typedef bool T;
public:
	almost_equal(const boost::uint64_t) { }
	inline const bool operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests int for equality
template<>
class almost_equal<int>
{
	typedef int T;
public:
	almost_equal(const boost::uint64_t) { }
	inline const bool operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests size_t for equality
template<>
class almost_equal<size_t>
{
	typedef size_t T;
public:
	almost_equal(const boost::uint64_t) { }
	inline const bool operator()(const T A, const T B) const { return A == B; }
};

/// Specialization of almost_equal that tests two double values for near-equality
template<>
class almost_equal<double>
{
	typedef double T;

public:
	almost_equal(const boost::uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool operator()(const T A, const T B) const
	{
		const boost::int64_t difference = representable_difference(A, B);
		return difference < 0 ? -difference <= threshold : difference <= threshold;
	}

private:
	/// Convert a double to a lexicographically-ordered twos-complement integer
	inline static const boost::int64_t to_integer(const double Value)
	{
		const boost::int64_t value = *(boost::int64_t*)&Value;
		return value < 0 ? 0x8000000000000000LL - value : value;
	}

	/// Given two doubles, returns their difference expressed as the number of uniquely-representable floating-point values that separate them
	inline static const boost::int64_t representable_difference(const double A, const double B)
	{
		return to_integer(B) - to_integer(A);
	}

	const boost::uint64_t threshold;
};

} // namespace k3d

#endif // K3DSDK_ALMOST_EQUAL_H

