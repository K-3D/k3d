#ifndef _MODULE_NURBS_PATCH_MODIFIER
#define _MODULE_NURBS_PATCH_MODIFIER

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/point4.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/shared_pointer.h>

#include "nurbs_curve_modifier.h"

namespace module
{
	namespace nurbs
	{
		class nurbs_patch_modifier
		{
			public:
				nurbs_patch_modifier(k3d::mesh& input);
                void add_patch(const size_t u_order, const size_t v_order, const k3d::mesh::indices_t& points, const k3d::mesh::knots_t& u_knots, const k3d::mesh::knots_t v_knots, const k3d::mesh::weights_t& weights);

			private:
				k3d::mesh *m_instance;
				k3d::mesh::nurbs_patches_t *m_nurbs_patches;
				k3d::mesh::indices_t *m_patch_first_points;
				k3d::mesh::counts_t *m_patch_u_point_counts;
				k3d::mesh::counts_t *m_patch_v_point_counts;
				k3d::mesh::orders_t *m_patch_u_orders;
				k3d::mesh::orders_t *m_patch_v_orders;
				k3d::mesh::indices_t *m_patch_u_first_knots;
				k3d::mesh::indices_t *m_patch_v_first_knots;
				k3d::mesh::selection_t *m_patch_selection;
				k3d::mesh::materials_t *m_patch_materials;
				k3d::mesh::indices_t *m_patch_points;
				k3d::mesh::weights_t *m_patch_point_weights;
				k3d::mesh::knots_t *m_patch_u_knots;
				k3d::mesh::knots_t *m_patch_v_knots;
				k3d::mesh::counts_t *m_patch_trim_curve_loop_counts;
                k3d::mesh::indices_t *m_patch_first_trim_curve_loops;
                k3d::mesh::points_2d_t *m_trim_points;
                k3d::mesh::selection_t *m_trim_point_selection;
                k3d::mesh::indices_t *m_first_trim_curves;
                k3d::mesh::counts_t *m_trim_curve_counts;
                k3d::mesh::selection_t *m_trim_curve_loop_selection;
                k3d::mesh::indices_t *m_trim_curve_first_points;
                k3d::mesh::counts_t *m_trim_curve_point_counts;
                k3d::mesh::orders_t *m_trim_curve_orders;
                k3d::mesh::indices_t *m_trim_curve_first_knots;
                k3d::mesh::selection_t *m_trim_curve_selection;
                k3d::mesh::indices_t *m_trim_curve_points;
                k3d::mesh::weights_t *m_trim_curve_point_weights;
                k3d::mesh::knots_t *m_trim_curve_knots;

		};
	}
}

#endif //_MODULE_NURBS_PATCH_MODIFIER
