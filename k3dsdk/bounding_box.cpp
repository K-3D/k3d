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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "bounding_box.h"

#include "algebra.h"

namespace k3d
{

bounding_box3& bounding_box3::operator*=(const matrix4 Transformation)
{
	bounding_box3 transformed;
	transformed.insert(Transformation * point3(px, py, pz));
	transformed.insert(Transformation * point3(nx, py, pz));
	transformed.insert(Transformation * point3(nx, py, nz));
	transformed.insert(Transformation * point3(px, py, nz));
	transformed.insert(Transformation * point3(px, ny, pz));
	transformed.insert(Transformation * point3(nx, ny, pz));
	transformed.insert(Transformation * point3(nx, ny, nz));
	transformed.insert(Transformation * point3(px, ny, nz));

	*this = transformed;

	return *this;
}

bounding_box3 operator*(const bounding_box3& BBox, const matrix4& Transformation)
{
	bounding_box3 new_box = BBox;
	new_box *= Transformation;

	return new_box;
}

bounding_box3 operator*(const matrix4& Transformation, const bounding_box3& BBox)
{
	bounding_box3 new_box = BBox;
	new_box *= Transformation;

	return new_box;
}

bool operator==(const bounding_box3& LHS, const bounding_box3& RHS)
{
	return LHS.nx == RHS.nx && LHS.px == RHS.px && LHS.ny == RHS.ny && LHS.py == RHS.py && LHS.nz == RHS.nz && LHS.pz == RHS.pz;
}

bool operator!=(const bounding_box3& LHS, const bounding_box3& RHS)
{
	return !(LHS == RHS);
}

std::ostream& operator<<(std::ostream& Stream, const bounding_box3& RHS)
{
	Stream << RHS.nx << " " << RHS.px << " " << RHS.ny << " " << RHS.py << " " << RHS.nz << " " << RHS.pz;
	return Stream;
}

std::istream& operator>>(std::istream& Stream, bounding_box3& RHS)
{
	Stream >> RHS.nx >> RHS.px >> RHS.ny >> RHS.py >> RHS.nz >> RHS.pz;
	return Stream;
}

} // namespace k3d

