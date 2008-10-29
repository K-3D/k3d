#ifndef K3DSDK_RECTANGLE_H
#define K3DSDK_RECTANGLE_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\brief 2D geometry routines
		\author Tim Shead (tshead@k-3d.com)
*/

#include "point2.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// rectangle

/// A two-dimensional rectangle particularly suitable for use with window systems i.e. Y coordinates are reversed so that top <= bottom
class rectangle
{
public:
	rectangle(const double Left, const double Right, const double Top, const double Bottom);
	rectangle(const point2& TopLeft, const point2& BottomRight);
	rectangle(const point2& Center, const double Width, const double Height);
	/// Returns the rectangle width
	double width() const;
	/// Returns the rectangle height
	double height() const;
	/// Returns the top-left corner of the rectangle
	const point2 top_left() const;
	/// Returns the bottom-right corner of the rectangle
	const point2 bottom_right() const;
	/// Returns the center of the rectangle
	const point2 center() const;
	/// Tests a point to see if it is contained within the rectangle
	bool contains(const point2& Point) const;
	/// Stores the rectangle's left edge
	double left;
	/// Stores the rectangle's right edge
	double right;
	/// Stores the rectangle's top edge
	double top;
	/// Stores the rectangle's bottom edge
	double bottom;
};

/// Returns a rectangle where left <= right and top <= bottom
const rectangle normalize(const rectangle& Rectangle);

/// Output inserter
std::ostream& operator<<(std::ostream& Stream, const rectangle& Arg);
/// Input extractor
std::istream& operator>>(std::istream& Stream, rectangle& Arg);


} // namespace k3d

#endif // !K3DSDK_RECTANGLE_H

