#ifndef MODULES_NURBS_NURBS_CURVES_H
#define MODULES_NURBS_NURBS_CURVES_H
// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Carsten Haubold (CarstenHaubold@web.de)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/types.h>
#include <k3dsdk/mesh.h>

namespace k3d
{
namespace nurbs_curve { class primitive; class const_primitive; }
}

namespace module
{

namespace nurbs
{

/// Adds the given curve to the other primitive and mesh
void add_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurve, const k3d::uint_t& Curve, k3d::mesh::indices_t& PointMap, k3d::mesh::bools_t& AddedPoints);

/// Normalizes the knot vector of the given curve number in the given NURBS curve primitive
void normalize_knot_vector(k3d::nurbs_curve::primitive& NurbsCurve, const k3d::uint_t Curve);

///Replace a point on the curve with a new one (by changing the index)
/**\param newIndex the index of the point which shall be used from now on
 * \param curve The curve we're operating on
 * \param point The index of the point which shall be replaced
 * \param continuous If this is set to true, the curve's knot vector is changed so that the curve intersects the point
 **/
void replace_point(k3d::nurbs_curve::primitive& NurbsCurve, k3d::uint_t newIndex, k3d::uint_t curve, k3d::uint_t point, bool continuous);

///Close the selected curve either by adding a new point or by replacing the former end points by their average
/** \param curve The curve to close
 *  \param keep_ends If this is true then we're going to add a new point, otherwise the old end points get discarded
 **/
void close_curve(k3d::mesh& Mesh, k3d::nurbs_curve::primitive& NurbsCurve, k3d::uint_t curve, bool keep_ends);

/// Elevate the degree of the curves listed in Curves, storing the resulting mesh in OutputMesh and OutputCurves
void elevate_curve_degree(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurve, const k3d::mesh::indices_t Curves);

/// Connect the two input curves at their selected end points, storing the result in Output
//void connect_curves(k3d::nurbs_curve::primitive& Output, const k3d::uint_t Curve1, const k3d::uint_t CurvePoint1, const k3d::uint_t Curve2, const k3d::uint_t CurvePoint2);

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_CURVES_H
