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
		///Simple representation of a nurbs curve, used to pass data between curves and patches
		typedef struct nurbs_curve{
			k3d::mesh::knots_t curve_knots;
			k3d::mesh::weights_t curve_point_weights;
			k3d::mesh::points_t control_points;
		};

		///A trimming curve data struct
		typedef struct nurbs_trim_curve{
			k3d::mesh::knots_t curve_knots;
			k3d::mesh::weights_t curve_point_weights;
			k3d::mesh::points_2d_t control_points;
		};

		///A nurbs patch data struct
		typedef struct nurbs_patch{
			size_t u_order;
			size_t v_order;
			k3d::mesh::knots_t u_knots;
			k3d::mesh::knots_t v_knots;
			k3d::mesh::weights_t point_weights;
			k3d::mesh::points_t control_points;
		};

		///NurbsPatchModifier bundles all functionality that works on NURBS Patches
		class nurbs_patch_modifier
		{
			public:
				///Create a nurbs_patch_modifier from the given mesh, working on its patches
				///\param input The mesh to operate on
				nurbs_patch_modifier(k3d::mesh& input);

				///Add a trimming curve to the chosen patch
				///\param patch The patch we want to trim
				///\param curve The curve (still a common NURBS curve) we're using as trimming curve, it will be transformed to a trimming curve
				///\param scale Scales the coordinates of the trim curve
				///\param offset_u Offset the trim curve
				///\param offset_v Offset the trim curve
				void add_trim_curve(size_t patch, size_t curve, double scale, double offset_u, double offset_v);

                ///Deletes the selected patch from this mesh, unused points will be removed
                void delete_patch(size_t patch);

				///Extract the patch with the given index, returns this patch
				nurbs_patch extract_patch(size_t patch);

				///Extract a curve from a patch in u direction and return it
				///\param patch The patch where we want the curve from
				///\param v Which of the v curves do we want? Must be smaller than patch_v_point_counts
				nurbs_curve extract_u_curve(size_t patch, size_t v);

				///Extract a curve from a patch in v direction and return it
				///\param patch The patch where we want the curve from
				///\param u Which of the u curves do we want? Must be smaller than patch_u_point_counts
				nurbs_curve extract_v_curve(size_t patch, size_t u);

                ///Extrude the selected surface along the given axis
                ///\param patch the patch to extrude
                ///\param axis Along which Axis shall we extrude
                ///\param distance How far to extrude the face
                ///\param cap Whether to have caps at both ends or just at one
                void extrude_patch(size_t patch, k3d::axis axis, double distance, bool cap);

				///Returns the number of patches in this mesh
				int get_patch_count();

				///Adds a patch to the mesh, existing points may be used
				///\param patch The patch to insert
				///\param share_points Whether to use existing vertices at the same position
				void insert_patch(const nurbs_patch& patch, bool share_points);

                ///Returns the evaluated point on the surface at (u,v)
                k3d::point4 patch_point(size_t patch, double u, double v);

				///Inserts a knot into all u curves of the patch
				///\param patch The patch we're going to work on
				///\param u The u value (between 0.0 and 1.0) where to insert the knot
				///\param multiplicity How often we want to insert that knot
				void patch_u_knot_insertion(size_t patch, double u, size_t multiplicity);

				///Inserts a knot into all v curves of the patch
				///\param patch The patch we're going to work on
				///\param v The v value (between 0.0 and 1.0) where to insert the knot
				///\param multiplicity How often we want to insert that knot
				void patch_v_knot_insertion(size_t patch, double v, size_t multiplicity);

				///DegreeElevates all u-curves to the given amount
				///\param patch The patch to degree elevate
				///\param degree If the patch had u-degree 2 and you specify 2 here, it'll get a new degree of 4
				void patch_u_degree_elevation(size_t patch, size_t degree);

				///DegreeElevates all v-curves to the given amount
				///\param patch The patch to degree elevate
				///\param degree If the patch had u-degree 2 and you specify 2 here, it'll get a new degree of 4
				void patch_v_degree_elevation(size_t patch, size_t degree);

				///Create a polygonal grid representing the NurbsPatch
				///\param patch
				///\param segments How many segments in each direction, if this is n then the resulting patch is n*n
				///\param delete_orig Whether to delete the former patch
				void polygonize_patch(size_t patch, size_t segments, bool delete_orig, bool flip);

                ///Selects this patch and deselects all others
                ///\param patch The patch to select
                void select_patch(size_t patch);

				///Split a patch at the seleted u-value (so itgets split in u-direction)
				///adds a new patch to the end
				///\param patch
				///\param u
				void split_patch_u(size_t patch, double u);

				///Split a patch at the seleted v-value (so itgets split in v-direction)
				///adds a new patch to the end
				///\param patch
				///\param v
				void split_patch_v(size_t patch, double u);

                ///Returns the index of the selected patch
                int get_selected_patch();

                ///Returns the indices of the selected patches
                std::vector<size_t> get_selected_patches();

			private:
				///Adds this point to the mesh's points_t instance, and returns the index to its position
				///\param point The point to add
				///\param shared If this is true then we'll try to use an existing point at the same position
				size_t insert_point(const k3d::point3& point, bool shared);

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
				k3d::mesh::points_t *m_mesh_points;
				k3d::mesh::selection_t *m_point_selections;

		};
	}
}

#endif //_MODULE_NURBS_PATCH_MODIFIER
