// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "imaterial.h"
#include "mesh_operations.h"
#include "mesh_selection.h"
#include "polyhedron.h"
#include "vectors.h"

namespace k3d
{

const bool_t validate_nurbs_curve_groups(const mesh& Mesh)
{
	if(!Mesh.nurbs_curve_groups)
		return false;

	return_val_if_fail(Mesh.nurbs_curve_groups->first_curves, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_orders, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_weights, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_selection, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->materials, false);

	return true;
}

const bool_t validate_nurbs_patches(const mesh& Mesh)
{
	if(!Mesh.nurbs_patches)
		return false;

	return_val_if_fail(Mesh.nurbs_patches->patch_first_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_point_weights, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_materials, false);

	return true;
}

} // namespace k3d

