#ifndef K3DSDK_BITMAP_H
#define K3DSDK_BITMAP_H

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

/** \file
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "Half/half.h"
#include "result.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>

#include <boost/gil/gil_all.hpp>

namespace boost
{

namespace gil
{

typedef half bits16f;

template<>
struct channel_traits<bits16f>
{
	typedef half value_type;
	typedef half& reference;
	typedef half* pointer;
	typedef const half& const_reference;
	typedef const half* const_pointer;
	BOOST_STATIC_CONSTANT(bool, is_mutable=true);

	static value_type min_value()
	{
		return bits16f(0.0f);
	}

	static value_type max_value()
	{
		return bits16f(1.0f);
	}
};

GIL_DEFINE_BASE_TYPEDEFS(16f,gray)
GIL_DEFINE_BASE_TYPEDEFS(16f,bgr)
GIL_DEFINE_BASE_TYPEDEFS(16f,argb)
GIL_DEFINE_BASE_TYPEDEFS(16f,abgr)
GIL_DEFINE_BASE_TYPEDEFS(16f,bgra)

GIL_DEFINE_ALL_TYPEDEFS(16f,rgb)
GIL_DEFINE_ALL_TYPEDEFS(16f,rgba)
GIL_DEFINE_ALL_TYPEDEFS(16f,cmyk)

template <int N> struct devicen_t;
template <int N> struct devicen_layout_t;
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev2n, devicen_t<2>, devicen_layout_t<2>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev3n, devicen_t<3>, devicen_layout_t<3>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev4n, devicen_t<4>, devicen_layout_t<4>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16f,dev5n, devicen_t<5>, devicen_layout_t<5>)

} // namespace gil

} // namespace boost

namespace k3d
{

/// Defines a standard pixel as half-precision floating-point RGBA
typedef boost::gil::pixel<boost::gil::bits16f, boost::gil::rgba_layout_t> pixel;
/// Defines a standard bitmap as half-precision floating-point RGBA
typedef boost::gil::image<pixel, false> bitmap;
/// Define storage for a pixel coordinate
typedef bitmap::coord_t pixel_size_t;

/// Fills a bitmap with a checkerboard pattern with given check dimensions and colors
template<typename bitmap_t, typename even_color_t, typename odd_color_t>
void checkerboard_fill(bitmap_t& Bitmap, const pixel_size_t CheckWidth, const pixel_size_t CheckHeight, const even_color_t EvenColor, const odd_color_t OddColor)
{
	typename bitmap_t::iterator target = Bitmap.begin();

	for(pixel_size_t y = 0; y < Bitmap.height(); ++y)
	{
		const bool even_row = (y / CheckHeight) % 2;

		for(pixel_size_t x = 0; x < Bitmap.width(); ++x)
		{
			const bool even_column = (x / CheckWidth) % 2;

			if((even_row + even_column) % 2)
				*target = EvenColor;
			else
				*target = OddColor;

			++target;
		}
	}
}

} // namespace k3d

#endif // !K3DSDK_BITMAP_H

