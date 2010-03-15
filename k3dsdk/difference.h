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

#include <boost/math/special_functions/next.hpp>
#include <boost/math/tools/test.hpp>
#include <boost/static_assert.hpp>

namespace k3d
{

namespace difference
{

/// Stores the results of the difference::test() function.
class test_result
{
public:
	test_result() :
		count(0),
		equal(true),
		ulps(0),
		relative_error(0)
	{
	}

	void insert(const bool_t& Equal)
	{
		++count;

		if(!Equal)
			equal = false;
	}

	uint64_t count;
	bool_t equal;
	uint64_t ulps;
	double_t relative_error;
};

/// Function that tests the difference between two objects, returning separate results for exact (integer and string) and inexact (floating-point) types.
/// Specializations provide the actual implementations.  For exact types, the boolean value will be set to 'false' if the objects are not equal.
/// For inexact types, their difference, specified as the maximum number of Units in the Last Place is returned, along with the maximum relative error - see
/// "Comparing floating point numbers" by Bruce Dawson at http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
template<typename T>
const test_result test(const T& A, const T& B)
{
	test_result result;
	test(A, B, result);
	return result;
};

/// Function that tests the difference between two objects, returning separate results for exact (integer and string) and inexact (floating-point) types.
/// Specializations provide the actual implementations.  For exact types, the boolean value will be set to 'false' if the objects are not equal.
/// For inexact types, their difference, specified as the maximum number of Units in the Last Place is returned, along with the maximum relative error - see
/// "Comparing floating point numbers" by Bruce Dawson at http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
template<typename T>
void test(const T& A, const T& B, test_result& Result)
{
	// This will be triggered if this template is ever instantiated
	BOOST_STATIC_ASSERT(sizeof(T) == 0);
};

/// Specialization of test() that tests bool_t
inline void test(const bool_t& A, const bool_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests int8_t
inline void test(const int8_t& A, const int8_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests int16_t
inline void test(const int16_t& A, const int16_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests int32_t
inline void test(const int32_t& A, const int32_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests int64_t
inline void test(const int64_t& A, const int64_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests uint8_t
inline void test(const uint8_t& A, const uint8_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests uint16_t
inline void test(const uint16_t& A, const uint16_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests uint32_t
inline void test(const uint32_t& A, const uint32_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests uint64_t
inline void test(const uint64_t& A, const uint64_t& B, test_result& Result)
{
	Result.insert(A == B);
};

/// Specialization of test that tests double_t
inline void test(const double_t& A, const double_t& B, test_result& Result)
{
	++Result.count;
	Result.ulps = std::max(Result.ulps, static_cast<uint64_t>(std::fabs(boost::math::float_distance(A, B))));
	Result.relative_error = std::max(Result.relative_error, boost::math::tools::relative_error(A, B));
};

/// Specialization of test that tests string_t
inline void test(const string_t& A, const string_t& B, test_result& Result)
{
	Result.insert(A == B);
};

#ifdef K3D_API_DARWIN

/// Specialization of test for use with unsigned long (required on OSX)
/// \deprecated New code must use the sized K-3D types instead of unsigned long
inline void test(const unsigned long& A, const unsigned long& B, test_result& Result)
{
	Result.insert(A == B);
};

#endif // K3D_API_DARWIN

/// Given iterators designating two sequences, calls the test() function for each pair of values,
/// and confirms that both sequences are the same length.
template<typename IteratorT>
void range_test(IteratorT A, IteratorT LastA, IteratorT B, IteratorT LastB, test_result& Result)
{
	for(; A != LastA && B != LastB; ++A, ++B)
		test(*A, *B, Result);

	Result.insert(A == LastA && B == LastB);
};

} // namespace difference

} // namespace k3d

#endif // !K3DSDK_DIFFERENCE_H

