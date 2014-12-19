#ifndef K3DSDK_DIFFERENCE_H
#define K3DSDK_DIFFERENCE_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/types.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/math/special_functions/next.hpp>
#include <boost/static_assert.hpp>

namespace k3d
{

namespace difference
{

/// Stores the results of the difference::test() function.
class accumulator
{
public:
	/// Stores statistics for comparisons of exact (string and integer) types, including the number of tests, and the min and max test values.
	boost::accumulators::accumulator_set<bool_t, boost::accumulators::stats<boost::accumulators::tag::count, boost::accumulators::tag::min, boost::accumulators::tag::max> > exact;
	/// Stores statistics for comparisons of inexact (floating-point) types using Units in the Last Place (ULPS), including the number of tests, min, max, mean, median, and variance.
	boost::accumulators::accumulator_set<double_t, boost::accumulators::stats<boost::accumulators::tag::min, boost::accumulators::tag::mean, boost::accumulators::tag::max, boost::accumulators::tag::median, boost::accumulators::tag::lazy_variance> > ulps;
};

/// Function that tests the difference between two objects, returning separate results for exact (integer and string) and inexact (floating-point) types.
/// See "Comparing floating point numbers" by Bruce Dawson at http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
template<typename T>
const accumulator test(const T& A, const T& B)
{
	accumulator result;
	test(A, B, result);
	return result;
};

/// Function that tests the difference between two objects, returning separate results for exact (integer and string) and inexact (floating-point) types.
/// See "Comparing floating point numbers" by Bruce Dawson at http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
template<typename T>
void test(const T& A, const T& B, accumulator& Result)
{
	// This will be triggered if this template is ever instantiated
	BOOST_STATIC_ASSERT(sizeof(T) == 0);
};

/// Specialization of test() that tests bool_t
inline void test(const bool_t& A, const bool_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

#ifdef _LIBCPP_VERSION
// For libc++
inline void test(const std::vector<bool>::reference& A, const std::vector<bool>::reference& B, accumulator& Result)
{
  Result.exact(A == B);
};

inline void test(const std::vector<bool>::const_reference& A, const std::vector<bool>::const_reference& B, accumulator& Result)
{
  Result.exact(A == B);
};
#endif

/// Specialization of test that tests int8_t
inline void test(const int8_t& A, const int8_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests int16_t
inline void test(const int16_t& A, const int16_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests int32_t
inline void test(const int32_t& A, const int32_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests int64_t
inline void test(const int64_t& A, const int64_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests uint8_t
inline void test(const uint8_t& A, const uint8_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests uint16_t
inline void test(const uint16_t& A, const uint16_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests uint32_t
inline void test(const uint32_t& A, const uint32_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests uint64_t
inline void test(const uint64_t& A, const uint64_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

/// Specialization of test that tests double_t
inline void test(const double_t& A, const double_t& B, accumulator& Result)
{
	Result.ulps(std::fabs(boost::math::float_distance(A, B)));
};

/// Specialization of test that tests string_t
inline void test(const string_t& A, const string_t& B, accumulator& Result)
{
	Result.exact(A == B);
};

#ifdef K3D_API_DARWIN

/// Specialization of test for use with unsigned long (required on OSX)
/// \deprecated New code must use the sized K-3D types instead of unsigned long
inline void test(const unsigned long& A, const unsigned long& B, accumulator& Result)
{
	Result.exact(A == B);
};

#endif // K3D_API_DARWIN

/// Given iterators designating two sequences, calls the test() function for each pair of values,
/// and confirms that both sequences are the same length.
template<typename IteratorT>
void range_test(IteratorT A, IteratorT LastA, IteratorT B, IteratorT LastB, accumulator& Result)
{
	for(; A != LastA && B != LastB; ++A, ++B)
		test(*A, *B, Result);

	Result.exact(A == LastA && B == LastB);
};

} // namespace difference

} // namespace k3d

#endif // !K3DSDK_DIFFERENCE_H

