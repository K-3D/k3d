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
#include <cmath>

namespace k3d
{

/// Convert a double to an integer
const boost::int64_t to_integer(const double Value)
{
	const boost::int64_t value = *(boost::int64_t*)&Value;
	return value < 0 ? 0x8000000000000000LL - value : value;
}

/// Given two doubles, returns their difference in terms of the number of uniquely-representable floating-point values that separate them
const boost::int64_t representable_difference(const double A, const double B)
{
	return to_integer(B) - to_integer(A);
}

/// Return true iff two doubles are nearly the same
const bool almost_equal(const double A, const double B, const boost::uint64_t Epsilon)
{
	const boost::int64_t difference = representable_difference(A, B);
	return difference < 0 ? -difference < Epsilon : difference < Epsilon;
}

} // namespace k3d

#endif // K3DSDK_ALMOST_EQUAL_H

