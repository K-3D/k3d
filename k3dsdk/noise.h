#ifndef K3DSDK_NOISE_H
#define K3DSDK_NOISE_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include "color.h"
#include "vectors.h"

namespace k3d
{

template<typename T>
const T noise(const double U);
template<typename T>
const T noise(const double U, const double V);
template<typename T>
const T noise(const point3& Pt);

const double noise(const double U);
const double noise(const double U, const double V);
const double noise(const point3& Pt);

template<>
inline const point3 noise(const double U)
{
	return point3(noise(U + 0.34), noise(U + 0.011), noise(U + 0.34));
}

template<>
inline const point3 noise(const double U, const double V)
{
	return point3(noise(U + 0.34, V + 0.66), noise(U + 0.011, V + 0.845), noise(U + 0.34, V + 0.12));
}

template<>
inline const point3 noise(const point3& Pt)
{
	return point3(
		noise(point3(Pt[0] + 0.34, Pt[1] + 0.66, Pt[2] + 0.237)),
		noise(point3(Pt[0] + 0.011, Pt[1] + 0.845, Pt[2] + 0.037)),
		noise(point3(Pt[0] + 0.34, Pt[1] + 0.12, Pt[2] + 0.9)));
}

template<>
inline const color noise(const double U)
{
	return color(noise(U + 0.34), noise(U + 0.011), noise(U + 0.34));
}

template<>
inline const color noise(const double U, const double V)
{
	return color(noise(U + 0.34, V + 0.66), noise(U + 0.011, V + 0.845), noise(U + 0.34, V + 0.12));
}

template<>
inline const color noise(const point3& Pt)
{
	return color(
		noise(point3(Pt[0] + 0.34, Pt[1] + 0.66, Pt[2] + 0.237)),
		noise(point3(Pt[0] + 0.011, Pt[1] + 0.845, Pt[2] + 0.037)),
		noise(point3(Pt[0] + 0.34, Pt[1] + 0.12, Pt[2] + 0.9)));
}

} // namespace k3d

#endif // K3DSDK_NOISE_H

