// MixKit -- Code library for multiresolution surface modeling
// Copyright (c) 1998, Michael Garland
//
// Contact: http://graphics.cs.uiuc.edu/~garland/software/qslim.html
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief Vectors and Matrices
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include "MxMath.h"

double Mat3::det()
{
	return row[0] * (row[1] ^ row[2]);
}

Mat3 Mat3::transpose()
{
	return Mat3(col(0), col(1), col(2));
}

Mat3 Mat3::adjoint()
{
	return Mat3(row[1]^row[2],
		row[2]^row[0],
		row[0]^row[1]);
}

double Mat3::invert(Mat3& inv)
{
	Mat3 A = adjoint();
	double d = A.row[0] * row[0];

	if(d == 0)
		return 0;

	inv = A.transpose() / d;
	return d;
}


