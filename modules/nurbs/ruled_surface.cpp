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
class ruled_surface :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	ruled_surface(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_flip(init_owner(*this) + init_name(_("flip")) + init_label(_("flip")) + init_description(_("Flip one of the curves before creating the surface")) + init_value(false)),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curves")) + init_description(_("Delete the original curves")) + init_value(true)),
		m_order(init_owner(*this) + init_name("order") + init_label(_("Order")) + init_description(_("Order in the linear direction")) + init_value(2) + init_constraint(constraint::minimum(2)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_segments(init_owner(*this) + init_name("segments") + init_label(_("Segments")) + init_description(_("Number of segments for the linear direction")) + init_value(1) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_flip.changed_signal().connect(make_update_mesh_slot());
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
		m_order.changed_signal().connect(make_update_mesh_slot());
		m_segments.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		return_if_fail(m_order.pipeline_value() <= (m_segments.pipeline_value() + 1));

		// Make sure the curves are compatible first
		k3d::double_t order;
		visit_selected_curves(Output, max_order_calculator(order));
		k3d::mesh elevated_mesh = Output;
		modify_selected_curves(Input, elevated_mesh, degree_elevator(order));
		k3d::mesh::knots_t knots;
		visit_selected_curves(elevated_mesh, knot_vector_calculator(knots));
		k3d::mesh compatible_mesh;
		compatible_mesh.points.create();
		compatible_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> compatible_curves(k3d::nurbs_curve::create(compatible_mesh));
		knot_vector_merger merger(knots, order);
		for(k3d::uint_t prim_idx = 0; prim_idx != elevated_mesh.primitives.size(); ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_curve::const_primitive> input_curves(k3d::nurbs_curve::validate(elevated_mesh, *elevated_mesh.primitives[prim_idx]));
			if(!input_curves)
				continue;
			if(compatible_curves->material.empty())
				compatible_curves->material.push_back(input_curves->material.back());
			for(k3d::uint_t curve = 0; curve != input_curves->curve_first_points.size(); ++curve)
			{
				if(input_curves->curve_selections[curve])
					merger(compatible_mesh, *compatible_curves, elevated_mesh, *input_curves, curve);
			}
		}
		if(compatible_curves->curve_first_points.size() != 2)
		{
			k3d::log() << error << "You must select exactly 2 curves. Number of selected curves: " << compatible_curves->curve_first_points.size() << std::endl;
			return;
		}

		if(m_flip.pipeline_value())
		{
			flip_curve(*compatible_curves, 1);
		}

		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(Output));
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_compatible_curves(k3d::nurbs_curve::validate(compatible_mesh, *compatible_mesh.primitives.front()));
		module::nurbs::ruled_surface(Output, *output_patches, compatible_mesh, *const_compatible_curves, m_order.pipeline_value(), m_segments.pipeline_value());

		if(m_delete_original.pipeline_value())
		{
			delete_selected_curves(Output);
			delete_empty_primitives(Output);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<ruled_surface, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x8114ebd9, 0xe74aa420, 0xf7503b98, 0x640f7e0d),
		  "NurbsRuledSurface",
		  _("Creates a NURBS surface stretched between the 2 curves"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_flip;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_order;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_segments;
};

k3d::iplugin_factory& ruled_surface_factory()
{
	return ruled_surface::get_factory();
}

} //namespace nurbs

} //namespace module

