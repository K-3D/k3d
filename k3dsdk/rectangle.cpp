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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "rectangle.h"

#include <cmath>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// rectangle

rectangle::rectangle(const double Left, const double Right, const double Top, const double Bottom) :
	left(Left),
	right(Right),
	top(Top),
	bottom(Bottom)
{
}

rectangle::rectangle(const point2& TopLeft, const point2& BottomRight) :
	left(TopLeft[0]),
	right(BottomRight[0]),
	top(TopLeft[1]),
	bottom(BottomRight[1])
{
}

rectangle::rectangle(const point2& Center, const double Width, const double Height) :
	left(Center[0] - (Width / 2)),
	right(Center[0] + (Width / 2)),
	top(Center[1] - (Height / 2)),
	bottom(Center[1] + (Height / 2))
{
}

double rectangle::width() const
{
	return std::fabs(right - left);
}

double rectangle::height() const
{
	return std::fabs(bottom - top);
}

const point2 rectangle::top_left() const
{
	return point2(left, top);
}

const point2 rectangle::bottom_right() const
{
	return point2(right, bottom);
}

const point2 rectangle::center() const
{
	return point2(left+right, top+bottom) * 0.5;
}

bool rectangle::contains(const point2& Point) const
{
	return (left <= Point[0] && Point[0] <= right && top <= Point[1] && Point[1] <= bottom) ? true : false;
}

const rectangle normalize(const rectangle& Rectangle)
{
	return rectangle(
		std::min(Rectangle.left, Rectangle.right),
		std::max(Rectangle.left, Rectangle.right),
		std::min(Rectangle.top, Rectangle.bottom),
		std::max(Rectangle.top, Rectangle.bottom));
}

std::ostream& operator<<(std::ostream& Stream, const rectangle& Arg)
{
	Stream << Arg.left << " " << Arg.right << " " << Arg.top << " " << Arg.bottom;
	return Stream;
}

std::istream& operator>>(std::istream& Stream, rectangle& Arg)
{
	Stream >> Arg.left >> Arg.right >> Arg.top >> Arg.bottom;
	return Stream;
}

} // namespace k3d

