#ifndef K3DSDK_MULTI_ARRAY_H
#define K3DSDK_MULTI_ARRAY_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#define BOOST_MULTI_ARRAY_NO_GENERATORS 1
#include <boost/multi_array.hpp>
#include <boost/array.hpp>

#include "types.h"

namespace k3d
{

namespace multi_array
{

template<int Dimensions>
struct extents :
	public boost::array<k3d::uint_t, Dimensions>
{
};

template<>
struct extents<2> :
	public boost::array<k3d::uint_t, 2>
{
	extents(k3d::uint_t i, k3d::uint_t j)
	{
		operator[](0) = i;
		operator[](1) = j;
	}
};

template<>
struct extents<3> :
	public boost::array<k3d::uint_t, 3>
{
	extents(k3d::uint_t i, k3d::uint_t j, k3d::uint_t k)
	{
		operator[](0) = i;
		operator[](1) = j;
		operator[](1) = k;
	}
};

} // namespace multi_array

} // namespace k3d

#endif // K3DSDK_MULTI_ARRAY_H

