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

#include "line2.h"
#include "vectors.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// line2

line2::line2(const vector2& Direction, const point2& Point) :
	direction(Direction),
	point(Point)
{
}

line2::line2(const point2& Point1, const point2& Point2) :
	direction(Point2 - Point1),
	point(Point1)
{
}

} // namespace k3d

