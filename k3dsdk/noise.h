#ifndef K3DSDK_NOISE_H
#define K3DSDK_NOISE_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
	\brief Functions by Ken Perlin, from "Texturing and Modeling - A Procedural Approach"
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/point3.h>
#include <k3dsdk/types.h>
#include <boost/static_assert.hpp>

namespace k3d
{

namespace noise
{

/// Noise generator that returns "classic" Perlin 3D noise in the range (-1, 1).
class classic3
{
public:
	classic3();

	/// Returns a noise value in the range (-1, 1) for the given point in 3-space.
	double_t operator()(double_t X, double_t Y, double_t Z) const;

private:
	static int32_t grad3[][3];
	static int32_t p[];
	static int32_t perm[512];
};

/// Helper function that maps 3D noise to 3D objects including point3, vector3, normal3, and texture3.
template<typename ValueT, typename GeneratorT>
const ValueT map3(const GeneratorT& Generator, double_t X, double_t Y, double_t Z)
{
	return ValueT(
		Generator(X + 0.34, Y + 0.66, Z + 0.237),
		Generator(X + 0.011, Y + 0.845, Z + 0.037),
		Generator(X + 0.34, Y + 0.12, Z + 0.9)
		);
}

} // namespace noise

} // namespace k3d

#endif // !K3DSDK_NOISE_H

