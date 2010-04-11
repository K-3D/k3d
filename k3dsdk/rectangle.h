#ifndef K3DSDK_RECTANGLE_H
#define K3DSDK_RECTANGLE_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/point2.h>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// rectangle

/// Provides storage for a two-dimensional rectangle.
class rectangle
{
public:
	rectangle();
	rectangle(const double_t X1, const double_t X2, const double_t Y1, const double_t Y2);
	rectangle(const point2& Center, const double_t Width, const double_t Height);

	/// Returns the rectangle width, which will always be >= 0.
	double_t width() const;
	/// Returns the rectangle height, which will always be >= 0.
	double_t height() const;
	/// Returns the center of the rectangle.
	const point2 center() const;
	/// Tests a point to see if it is contained within the rectangle
	bool_t contains(const point2& Point) const;
	/// Stores the rectangle's first vertical edge.
	double_t x1;
	/// Stores the rectangle's second vertical edge.
	double_t x2;
	/// Stores the rectangle's first horizontal edge.
	double_t y1;
	/// Stores the rectangle's second horizontal edge.
	double_t y2;

	bool_t operator==(const rectangle& Other) const;
	bool_t operator!=(const rectangle& Other) const;

	/// Returns a rectangle where x1 <= x2 and y1 <= y2.
	static const rectangle normalize(const rectangle& Rectangle);
};

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const rectangle& Value);
/// Serialization
std::istream& operator>>(std::istream& Stream, rectangle& Value);


} // namespace k3d

#endif // !K3DSDK_RECTANGLE_H

