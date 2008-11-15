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
		const uint_t_array& FirstCurves,
		const uint_t_array& CurveCounts,
		const typed_array<bool_t>& PeriodicCurves,
		const typed_array<imaterial*>& Materials,
		const uint_t_array& CurveFirstPoints,
		const uint_t_array& CurvePointCounts,
		const typed_array<double_t>& CurveSelections,
		const uint_t_array& CurvePoints,
		const attribute_arrays& ConstantData,
		const attribute_arrays& UniformData,
		const attribute_arrays& VaryingData
		);

	const uint_t_array& first_curves;
	const uint_t_array& curve_counts;
	const typed_array<bool_t>& periodic_curves;
	const typed_array<imaterial*>& materials;
	const uint_t_array& curve_first_points;
	const uint_t_array& curve_point_counts;
	const typed_array<double_t>& curve_selections;
	const uint_t_array& curve_points;
	const attribute_arrays& constant_data;
	const attribute_arrays& uniform_data;
	const attribute_arrays& varying_data;
};

/// Gathers the member arrays of a cubic_curve primitive into a convenient package
class primitive
{
public:
	primitive(
		uint_t_array& FirstCurves,
		uint_t_array& CurveCounts,
		typed_array<bool_t>& PeriodicCurves,
		typed_array<imaterial*>& Materials,
		uint_t_array& CurveFirstPoints,
		uint_t_array& CurvePointCounts,
		typed_array<double_t>& CurveSelections,
		uint_t_array& CurvePoints,
		attribute_arrays& ConstantData,
		attribute_arrays& UniformData,
		attribute_arrays& VaryingData
		);

	uint_t_array& first_curves;
	uint_t_array& curve_counts;
	typed_array<bool_t>& periodic_curves;
	typed_array<imaterial*>& materials;
	uint_t_array& curve_first_points;
	uint_t_array& curve_point_counts;
	typed_array<double_t>& curve_selections;
	uint_t_array& curve_points;
	attribute_arrays& constant_data;
	attribute_arrays& uniform_data;
	attribute_arrays& varying_data;
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

