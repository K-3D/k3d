#ifndef MODULE_NURBS_CURVE_MODIFIER_H
#define MODULE_NURBS_CURVE_MODIFIER_H
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
#include <k3dsdk/gprim_factory.h>

#include "nurbs_patch_modifier.h"

#define MODULE_NURBS_DEBUG 1
#define nurbs_debug __FILE__ << ": " << __LINE__ << " "
#define MY_DEBUG if(MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug

namespace module{
	namespace nurbs{

        struct nurbs_curve;
        struct nurbs_patch;
	    class nurbs_patch_modifier;

        class nurbs_curve_modifier{
            public:
                nurbs_curve_modifier(k3d::mesh& input);
                nurbs_curve_modifier(nurbs_curve& curve);

                int selected_curve();
                void print_knot_vector(size_t curve);
                void replace_point(size_t newIndex, size_t curve, size_t point, bool continuous);
                void flip_curve(size_t curve);
                void normalize_knot_vector(size_t curve);
                int count_all_curves_in_groups();
                int get_curve_group(size_t curve);
                void remove_empty_groups();
                void remove_unused_points();
                void delete_curve(size_t curve);
                void join_curves(size_t point1, size_t curve1, size_t point2, size_t curve2);
                bool point3_float_equal(const k3d::point3& p1, const k3d::point3& p2, float threshold);
                int find_span(size_t curve,double u);
                std::vector<double> basis_functions(size_t curve, double u, size_t span);
                k3d::point4 curve_point(size_t curve,double u);
                void curve_knot_insertion(size_t curve,double u, size_t r);
                void close_curve(size_t curve,bool keep_ends);
                int curve_degree_elevate(size_t curve);
                k3d::point4 get_homogenous_point(size_t point);
                void split_curve_at(size_t curve, double u);

                nurbs_curve extract_curve(size_t curve);
                void knot_vector_adaption(std::vector<size_t> curves);

                //the following methods create surfaces from the curves this mesh contains
                void traverse_curve(size_t curve1, size_t curve2);
                void revolve_curve(size_t curve, double angle, int segments);
                void ruled_surface(size_t curve1, size_t curve2);
                bool create_cap(size_t curve);

            private:
                int factorial(int n);
                double binomial_coefficient(int n, int k);
                int Min(int a, int b);
                int Max(int a, int b);
                void fill_bezalfs(std::vector<std::vector<double> >& bezalfs, int power, int t);
                std::string output_point(const k3d::point4& point);
                std::string output_point(const k3d::point3& point);
                k3d::mesh::indices_t create_curve_points(std::vector<k3d::point4>& points);
                k3d::mesh::weights_t create_curve_point_weights(std::vector<k3d::point4>& points);
                bool find_point_inside(k3d::mesh::indices_t *points, size_t index);
                void offset_all_after(k3d::mesh::indices_t *points, size_t first, int offset);

                k3d::mesh *m_instance;
                k3d::mesh::nurbs_curve_groups_t *groups;
                k3d::mesh::knots_t *curve_knots;
                k3d::mesh::indices_t *curve_points;
                k3d::mesh::weights_t *curve_point_weights;
                k3d::mesh::counts_t *curve_counts;
                k3d::mesh::orders_t *curve_orders;
                k3d::mesh::counts_t *curve_point_counts;
                k3d::mesh::indices_t *curve_first_points;
                k3d::mesh::indices_t *curve_first_knots;
                k3d::mesh::indices_t *first_curves;
                k3d::mesh::selection_t *curve_selection;
                k3d::mesh::selection_t *point_selection;
                k3d::mesh::points_t *mesh_points;
                k3d::mesh::materials_t *materials;
        };
	}//namespace nurbs
}//namespace module

#endif // MODULE_NURBS_CURVE_OPERATIONS_H
