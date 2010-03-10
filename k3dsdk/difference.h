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
#include <boost/static_assert.hpp>

namespace k3d
{

/// Function that tests the difference between two objects, returning separate results for exact (integer and string) and inexact (floating-point) types.
/// Specializations provide the actual implementations.  For exact types, the boolean value will be set to 'false' if the objects are not equal.
/// For inexact types, their difference, specified as the maximum number of Units in the Last Place is returned - see
/// "Comparing floating point numbers" by Bruce Dawson at http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
template<typename T>
void difference(const T& A, const T& B, bool_t& Equal, uint64_t& ULPS)
{
	// This will be triggered if this template is ever instantiated
	BOOST_STATIC_ASSERT(sizeof(T) == 0);
};

/// Specialization of difference() that tests bool_t
inline void difference(const bool_t& A, const bool_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests int8_t
inline void difference(const int8_t& A, const int8_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests int16_t
inline void difference(const int16_t& A, const int16_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests int32_t
inline void difference(const int32_t& A, const int32_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests int64_t
inline void difference(const int64_t& A, const int64_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests uint8_t
inline void difference(const uint8_t& A, const uint8_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests uint16_t
inline void difference(const uint16_t& A, const uint16_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests uint32_t
inline void difference(const uint32_t& A, const uint32_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests uint64_t
inline void difference(const uint64_t& A, const uint64_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

/// Specialization of difference that tests double_t
inline void difference(const double_t& A, const double_t& B, bool_t& Equal, uint64_t& ULPS)
{
	const double_t ulps = std::fabs(boost::math::float_distance(A, B));
	if(ulps <= ULPS)
		return;

	ULPS = ulps;
};

/// Specialization of difference that tests string_t
inline void difference(const string_t& A, const string_t& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

#ifdef K3D_API_DARWIN

/// Specialization of difference for use with unsigned long (required on OSX)
/// \deprecated New code must use the sized K-3D types instead of unsigned long
inline void difference(const unsigned long& A, const unsigned long& B, bool_t& Equal, uint64_t& ULPS)
{
	if(A == B)
		return;

	Equal = false;
};

#endif // K3D_API_DARWIN

/// Given iterators designating to equal length sequences, calls the difference() function for each pair of values.
template<typename IteratorT>
void range_difference(IteratorT A, IteratorT LastA, IteratorT B, bool_t& Equal, uint64_t& ULPS)
{
	for(; A != LastA; ++A, ++B)
		difference(*A, *B, Equal, ULPS);
};

} // namespace k3d

#endif // !K3DSDK_DIFFERENCE_H

