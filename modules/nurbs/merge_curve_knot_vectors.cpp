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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "nurbs_curves.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{

class merge_curve_knot_vectors :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	merge_curve_knot_vectors(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);
		k3d::double_t order;
		visit_selected_curves(Output, max_order_calculator(order));
		k3d::mesh elevated_mesh = Output;
		modify_selected_curves(Input, elevated_mesh, degree_elevator(order));
		k3d::mesh::knots_t knots;
		visit_selected_curves(elevated_mesh, knot_vector_calculator(knots));
		modify_selected_curves(elevated_mesh, Output, knot_vector_merger(knots, order));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<merge_curve_knot_vectors, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xf0080968, 0xeb4096c5, 0x33d94c96, 0x3c8af308),
		  "NurbsMergeCurveKnotVectors",
		  _("Inserts a new Knot with the specified u-value"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	struct max_order_calculator
	{
		max_order_calculator(k3d::double_t& Order) : order(Order)
		{
			order = 0;
		}
		void operator()(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t& Curve)
		{
			order = Curves.curve_orders[Curve] > order ? Curves.curve_orders[Curve] : order;
		}
		/// The maximum order of all curves visited
		k3d::double_t& order;
	};

	struct knot_vector_calculator
	{
		knot_vector_calculator(k3d::mesh::knots_t& Knots) : knots(Knots) {}
		void operator()(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t& Curve)
		{
			append_common_knot_vector(knots, Curves, Curve);
		}
		/// The common knot vector for the visited curves
		k3d::mesh::knots_t& knots;
	};

	struct degree_elevator
	{
		degree_elevator(const k3d::uint_t Order) : order(Order) {}
		const k3d::double_t order;
		void operator()(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t& Curve)
		{
			elevate_curve_degree(OutputMesh, OutputCurves, InputMesh, InputCurves, Curve, order - InputCurves.curve_orders[Curve]);
		}
	};

	struct knot_vector_merger
	{
		knot_vector_merger(const k3d::mesh::knots_t& UnifiedKnots, const k3d::uint_t Order) : unified_knots(UnifiedKnots), order(Order) {}
		const k3d::mesh::knots_t& unified_knots;
		const k3d::uint_t order;
		void operator()(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t& Curve)
		{
			k3d::mesh::points_t points;
			k3d::mesh::knots_t knots;
			k3d::mesh::weights_t weights;
			extract_curve_arrays(points, knots, weights, InputMesh, InputCurves, Curve, true);
			for(k3d::uint_t knot_idx = 0; knot_idx != unified_knots.size(); ++knot_idx)
			{
				const k3d::double_t u = unified_knots[knot_idx];
				const k3d::uint_t old_mul = multiplicity(knots, u, 0, knots.size());
				const k3d::uint_t new_mul = multiplicity(unified_knots, u, 0, unified_knots.size());
				if(old_mul > new_mul)
					throw std::runtime_error("Error inserting knots when creating a common knot vector");
				if(old_mul < new_mul)
					insert_knot(points, knots, weights, u, 1, order);
			}
			k3d::nurbs_curve::add_curve(OutputMesh, OutputCurves, order, points, weights, knots);
		}
	};
};

k3d::iplugin_factory& merge_curve_knot_vectors_factory()
{
	return merge_curve_knot_vectors::get_factory();
}

} //namespace nurbs

} //namespace module
