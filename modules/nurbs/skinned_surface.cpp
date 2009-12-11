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

#include "nurbs_curves.h"
#include "nurbs_patches.h"
#include "utility.h"

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
class skinned_surface :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	skinned_surface(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curves")) + init_description(_("Delete the original curves")) + init_value(true)),
		m_order(init_owner(*this) + init_name("order") + init_label(_("Order")) + init_description(_("Order in the direction normal to the curves (V)")) + init_value(3) + init_constraint(constraint::minimum(2)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
		m_order.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		if(!Output.points.get())
			return;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);
		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(Output));

		// Get the selected curves
		k3d::mesh selected_curves_mesh;
		selected_curves_mesh.points.create();
		selected_curves_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> selected_curves(k3d::nurbs_curve::create(selected_curves_mesh));
		selected_curves->material.push_back(0);
		visit_selected_curves(Output, selected_curve_extractor(selected_curves_mesh, *selected_curves));

		// Make them compatible
		k3d::double_t order;
		visit_selected_curves(selected_curves_mesh, max_order_calculator(order));
		k3d::mesh elevated_mesh = selected_curves_mesh;
		modify_selected_curves(selected_curves_mesh, elevated_mesh, degree_elevator(order));
		k3d::mesh::knots_t knots;
		visit_selected_curves(elevated_mesh, knot_vector_calculator(knots));
		k3d::mesh compatible_mesh;
		compatible_mesh.points.create();
		compatible_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> compatible_curves(k3d::nurbs_curve::create(compatible_mesh));
		knot_vector_merger merger(knots, order);
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> input_curves(k3d::nurbs_curve::validate(elevated_mesh, *elevated_mesh.primitives.back()));
		compatible_curves->material = input_curves->material;
		const k3d::uint_t curve_count = input_curves->curve_first_points.size();
		for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
		{
			merger(compatible_mesh, *compatible_curves, elevated_mesh, *input_curves, curve);
		}

		const k3d::uint_t v_order = m_order.pipeline_value();
		return_if_fail(curve_count >= v_order);
		k3d::mesh::knots_t v_knots;
		k3d::nurbs_curve::add_open_uniform_knots(v_order, curve_count, v_knots);
		skin_curves(Output, *output_patches, compatible_mesh, *compatible_curves, v_knots, v_order);

		if(m_delete_original.pipeline_value())
		{
			delete_selected_curves(Output);
		}
		delete_empty_primitives(Output);
		k3d::mesh::delete_unused_points(Output);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<skinned_surface, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xe6e5899a, 0x9c445204, 0xa7758297, 0x86cd38e4),
		  "NurbsSkinnedSurface",
		  _("Creates a NURBS surface stretched along all selected curves"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_order;
};

k3d::iplugin_factory& skinned_surface_factory()
{
	return skinned_surface::get_factory();
}

} // namespace nurbs

} // namespace module

