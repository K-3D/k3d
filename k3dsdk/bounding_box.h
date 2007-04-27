#ifndef K3DSDK_BOUNDING_BOX_H
#define K3DSDK_BOUNDING_BOX_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Anders Dahnielson (anders@dahnielson.com)
*/

#include "algebra.h"
#include "vectors.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iosfwd>
#include <limits>

namespace k3d
{

/// Convenience type for storing a bounding-box in three dimensions
class bounding_box3
{
public:
	/// Initializes an "empty" bounding box (note: "empty" means "undefined" - it's not the same as having all values zero)
	explicit bounding_box3() :
		px(-std::numeric_limits<double>::max()),
		nx(std::numeric_limits<double>::max()),
		py(-std::numeric_limits<double>::max()),
		ny(std::numeric_limits<double>::max()),
		pz(-std::numeric_limits<double>::max()),
		nz(std::numeric_limits<double>::max())
	{
	}

	bounding_box3(double PX, double NX, double PY, double NY, double PZ, double NZ) :
		px(std::max(PX, NX)),
		nx(std::min(PX, NX)),
		py(std::max(PY, NY)),
		ny(std::min(PY, NY)),
		pz(std::max(PZ, NZ)),
		nz(std::min(PZ, NZ))
	{
	}

	/// Grows the bounding box as needed to contain the given point
	void insert(const point3& Point)
	{
		px = std::max(px, Point[0]);
		nx = std::min(nx, Point[0]);
		py = std::max(py, Point[1]);
		ny = std::min(ny, Point[1]);
		pz = std::max(pz, Point[2]);
		nz = std::min(nz, Point[2]);
	}

	/// Grows the bounding box as needed to contain the given bounding box
	void insert(const bounding_box3& Box)
	{
		px = std::max(px, Box.px);
		nx = std::min(nx, Box.nx);
		py = std::max(py, Box.py);
		ny = std::min(ny, Box.ny);
		pz = std::max(pz, Box.pz);
		nz = std::min(nz, Box.nz);
	}

	/// Returns true iff the bounding box contains the given point
	bool contains(const point3& Point) const
	{
		return nx <= Point[0] && Point[0] <= px && ny <= Point[1] && Point[1] <= py && nz <= Point[2] && Point[2] <= pz;
	}

	/// Returns true iff the box is empty (undefined)
	bool empty() const
	{
		return px < nx || py < ny || pz < nz;
	}

	/// Returns the width of the bounding box, or zero if the box is empty
	double width() const
	{
		return empty() ? 0.0 : px-nx;
	}

	/// Returns the height of the bounding box, or zero if the box is empty
	double height() const
	{
		return empty() ? 0.0 : py-ny;
	}

	/// Returns the depth of the bounding box, or zero if the box is empty
	double depth() const
	{
		return empty() ? 0.0 : pz-nz;
	}

	/// Returns the center of the bounding box, asserts if the box is empty
	const point3 center() const
	{
		assert(!empty());
		return point3((px + nx) / 2, (py + ny) / 2, (pz + nz) / 2);
	}

	bounding_box3& operator*=(const matrix4 Transformation);

	/// Stores the maximum x value (not necessarily positive)
	double px;
	/// Stores the minimum x value (not necessarily negative)
	double nx;
	/// Stores the maximum y value (not necessarily positive)
	double py;
	/// Stores the minimum y value (not necessarily negative)
	double ny;
	/// Stores the maximum z value (not necessarily positive)
	double pz;
	/// Stores the minimum z value (not necessarily negative)
	double nz;
};

bounding_box3 operator*(const bounding_box3& BBox, const matrix4& Transformation);
bounding_box3 operator*(const matrix4& Transformation, const bounding_box3& BBox);
bool operator==(const bounding_box3& LHS, const bounding_box3& RHS);
bool operator!=(const bounding_box3& LHS, const bounding_box3& RHS);

std::ostream& operator<<(std::ostream& Stream, const bounding_box3& RHS);
std::istream& operator>>(std::istream& Stream, bounding_box3& RHS);

} // namespace k3d

#endif // !K3DSDK_BOUNDING_BOX_H

