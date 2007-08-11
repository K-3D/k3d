#ifndef K3DSDK_LINE3_H
#define K3DSDK_LINE3_H

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

#include "vectors.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// line3

/// Encapsulates a 3D line
class line3
{
public:
	/// Constructs a line from its direction vector and a point on the line
	line3(const vector3& Direction, const point3& Point);
	/// Constructs a line from two points on the line
	line3(const point3& Point1, const point3& Point2);

	/// Stores the line's direction vector
	vector3 direction;
	/// Stores a point on the line
	point3 point;
};

} // namespace k3d

#endif // K3DSDK_LINE3_H

