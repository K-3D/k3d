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

#ifndef MODULE_NURBS_CURVE_OPERATIONS_H
#define MODULE_NURBS_CURVE_OPERATIONS_H

namespace module{
	namespace nurbs{
		int selected_curve(k3d::mesh& Output);
		void print_knot_vector(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::knots_t& knots, size_t curve);
		void replace_point(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::indices_t& indices, k3d::mesh::knots_t& knots, size_t newIndex, size_t curve, size_t point, bool continuous);
		void flip_curve(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::indices_t& point_indices, k3d::mesh::knots_t& knots, size_t curve);
		void normalize_knot_vector(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::knots_t& knots, size_t curve);
		int count_all_curves_in_groups(k3d::mesh::nurbs_curve_groups_t& groups);
		int get_curve_group(k3d::mesh::nurbs_curve_groups_t& groups, size_t curve);
		void remove_empty_groups(k3d::mesh::nurbs_curve_groups_t& groups);
		void offset_firsts(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::indices_t& firsts, size_t curve, int offset);
		void delete_curve(k3d::mesh::nurbs_curve_groups_t& groups, size_t curve);
		void join_curves(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::indices_t& indices, k3d::mesh::knots_t& knots, size_t point1, size_t curve1, size_t point2, size_t curve2);
		bool point3_float_equal(const k3d::point3& p1, const k3d::point3& p2, float threshold);
		int find_span(k3d::mesh& input, size_t curve, double u);
		std::vector<double> basis_functions(k3d::mesh& input, size_t curve, double u, size_t span);
		k3d::point4 curve_point(k3d::mesh& input, size_t curve, double u);
		void curve_knot_insertion(k3d::mesh& input, size_t curve, double u, size_t r);
		void nurbs_close_curve(k3d::mesh& input, size_t curve, bool keep_ends);
	}//namespace nurbs
}//namespace module

#endif // MODULE_NURBS_CURVE_OPERATIONS_H
