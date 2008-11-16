#ifndef K3DSDK_CUBIC_CURVE_H
#define K3DSDK_CUBIC_CURVE_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include "mesh.h"

namespace k3d
{

namespace cubic_curve
{

/// Gathers the member arrays of a cubic_curve primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const mesh::indices_t& FirstCurves,
		const mesh::counts_t& CurveCounts,
		const mesh::bools_t& PeriodicCurves,
		const mesh::materials_t& Materials,
		const mesh::indices_t& CurveFirstPoints,
		const mesh::counts_t& CurvePointCounts,
		const mesh::selection_t& CurveSelections,
		const mesh::indices_t& CurvePoints,
		const mesh::attribute_arrays_t& ConstantData,
		const mesh::attribute_arrays_t& UniformData,
		const mesh::attribute_arrays_t& VaryingData
		);

	const mesh::indices_t& first_curves;
	const mesh::counts_t& curve_counts;
	const mesh::bools_t& periodic_curves;
	const mesh::materials_t& materials;
	const mesh::indices_t& curve_first_points;
	const mesh::counts_t& curve_point_counts;
	const mesh::selection_t& curve_selections;
	const mesh::indices_t& curve_points;
	const mesh::attribute_arrays_t& constant_data;
	const mesh::attribute_arrays_t& uniform_data;
	const mesh::attribute_arrays_t& varying_data;
};

/// Gathers the member arrays of a cubic_curve primitive into a convenient package
class primitive
{
public:
	primitive(
		mesh::indices_t& FirstCurves,
		mesh::counts_t& CurveCounts,
		mesh::bools_t& PeriodicCurves,
		mesh::materials_t& Materials,
		mesh::indices_t& CurveFirstPoints,
		mesh::counts_t& CurvePointCounts,
		mesh::selection_t& CurveSelections,
		mesh::indices_t& CurvePoints,
		mesh::attribute_arrays_t& ConstantData,
		mesh::attribute_arrays_t& UniformData,
		mesh::attribute_arrays_t& VaryingData
		);

	mesh::indices_t& first_curves;
	mesh::counts_t& curve_counts;
	mesh::bools_t& periodic_curves;
	mesh::materials_t& materials;
	mesh::indices_t& curve_first_points;
	mesh::counts_t& curve_point_counts;
	mesh::selection_t& curve_selections;
	mesh::indices_t& curve_points;
	mesh::attribute_arrays_t& constant_data;
	mesh::attribute_arrays_t& uniform_data;
	mesh::attribute_arrays_t& varying_data;
};

/// Creates a new cubic_curve mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid cubic_curve primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid cubic_curve primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(mesh::primitive& GenericPrimitive);

} // namespace cubic_curve

} // namespace k3d

#endif // !K3DSDK_CUBIC_CURVE_H

