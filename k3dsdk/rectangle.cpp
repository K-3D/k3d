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

#include <k3dsdk/rectangle.h>

#include <cmath>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// rectangle

rectangle::rectangle() :
	x1(0),
	x2(0),
	y1(0),
	y2(0)
{
}

rectangle::rectangle(const double_t X1, const double_t X2, const double_t Y1, const double_t Y2) :
	x1(X1),
	x2(X2),
	y1(Y1),
	y2(Y2)
{
}

rectangle::rectangle(const point2& Center, const double_t Width, const double_t Height) :
       x1(Center[0] - (Width / 2)),
       x2(Center[0] + (Width / 2)),
       y1(Center[1] - (Height / 2)),
       y2(Center[1] + (Height / 2))
{
}

double_t rectangle::width() const
{
	return std::fabs(x2 - x1);
}

double_t rectangle::height() const
{
	return std::fabs(y2 - y1);
}

const point2 rectangle::center() const
{
	return point2(x1 + x2, y1 + y2) * 0.5;
}

bool_t rectangle::contains(const point2& Point) const
{
       return std::min(x1, x2) <= Point[0] && Point[0] <= std::max(x1, x2) && std::min(y1, y2) <= Point[1] && Point[1] <= std::max(y1, y2);
}

bool_t rectangle::operator==(const rectangle& Other) const
{
	return x1 == Other.x1 && x2 == Other.x2 && y1 == Other.y1 && y2 == Other.y2;
}

bool_t rectangle::operator!=(const rectangle& Other) const
{
	return !(*this == Other);
}

const rectangle rectangle::normalize(const rectangle& Rectangle)
{
	return rectangle(
		std::min(Rectangle.x1, Rectangle.x2),
		std::max(Rectangle.x1, Rectangle.x2),
		std::min(Rectangle.y1, Rectangle.y2),
		std::max(Rectangle.y1, Rectangle.y2));
}

std::ostream& operator<<(std::ostream& Stream, const rectangle& Arg)
{
	Stream << Arg.x1 << " " << Arg.x2 << " " << Arg.y1 << " " << Arg.y2;
	return Stream;
}

std::istream& operator>>(std::istream& Stream, rectangle& Arg)
{
	Stream >> Arg.x1 >> Arg.x2 >> Arg.y1 >> Arg.y2;
	return Stream;
}

} // namespace k3d

