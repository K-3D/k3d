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
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{
class add_trim_curve :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	add_trim_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_delete_curve(init_owner(*this) + init_name(_("delete_curve")) + init_label(_("Delete the Curve")) + init_description(_("Delete the curve which was used to create the trimming curve")) + init_value(false)),
		m_scale_u(init_owner(*this) + init_name(_("scale_u")) + init_label(_("U Scale")) + init_description(_("Scale factor in the U direction")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0)) + init_value(1.0)),
		m_scale_v(init_owner(*this) + init_name(_("scale-V")) + init_label(_("V Scale")) + init_description(_("Scale factor in the V direction")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0)) + init_value(1.0)),
		m_offset_u(init_owner(*this) + init_name(_("offset_u")) + init_label(_("U Offset")) + init_description(_("U position offset on the surface")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.0)),
		m_offset_v(init_owner(*this) + init_name(_("offset_v")) + init_label(_("V Offset")) + init_description(_("V position offset on the surface")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.0))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_delete_curve.changed_signal().connect(make_update_mesh_slot());
		m_scale_u.changed_signal().connect(make_update_mesh_slot());
		m_scale_v.changed_signal().connect(make_update_mesh_slot());
		m_offset_u.changed_signal().connect(make_update_mesh_slot());
		m_offset_v.changed_signal().connect(make_update_mesh_slot());
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
		
		k3d::mesh curves_mesh;
		curves_mesh.points.create();
		curves_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
		visit_selected_curves(Output, selected_curve_extractor(curves_mesh, *curves_prim));
		
		const k3d::double_t u_offset = m_offset_u.pipeline_value();
		const k3d::double_t v_offset = m_offset_v.pipeline_value();
		const k3d::double_t u_scale = m_scale_u.pipeline_value();
		const k3d::double_t v_scale = m_scale_v.pipeline_value();

		for(k3d::uint_t curve = 0; curve != curves_prim->curve_first_points.size(); ++curve)
		{
			k3d::bounding_box3 bbox;
			const k3d::uint_t points_begin = curves_prim->curve_first_points[curve];
			const k3d::uint_t points_end = curves_prim->curve_point_counts[curve] + points_begin;
			for(k3d::uint_t i = points_begin; i != points_end; ++i)
			{
				bbox.insert(curves_mesh.points->at(curves_prim->curve_points[i]));
			}
			const k3d::double_t x_offset = (bbox.nx + bbox.px)*0.5;
			const k3d::double_t y_offset = (bbox.ny + bbox.py)*0.5;
			const k3d::double_t x_scale = 1.0 / (bbox.px - bbox.nx);
			const k3d::double_t y_scale = 1.0 / (bbox.py - bbox.ny);
			k3d::mesh::points_2d_t trim_points;
			for(k3d::uint_t i = points_begin; i != points_end; ++i)
			{
				const k3d::point3& p = curves_mesh.points->at(curves_prim->curve_points[i]);
				trim_points.push_back(k3d::point2((p[0] - x_offset) * x_scale, (p[1] - y_offset) * y_scale));
			}
			k3d::mesh::points_t points;
			k3d::mesh::weights_t weights;
			k3d::mesh::knots_t knots;
			extract_curve_arrays(points, knots, weights, curves_mesh, *curves_prim, curve, true);
			for(k3d::uint_t prim_idx = 0; prim_idx != Output.primitives.size(); ++prim_idx)
			{
				boost::scoped_ptr<k3d::nurbs_patch::primitive> patch_prim(k3d::nurbs_patch::validate(Output, Output.primitives[prim_idx]));
				if(patch_prim)
				{
					for(k3d::uint_t patch = 0; patch != patch_prim->patch_first_points.size(); ++patch)
					{
						module::nurbs::add_trim_curve(*patch_prim, patch, trim_points, weights, knots, curves_prim->curve_orders[curve], u_offset, v_offset, u_scale, v_scale);
					}
				}
			}
		}

		if(m_delete_curve.pipeline_value())
		{
			delete_selected_curves(Output);
		}
		delete_empty_primitives(Output);

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add_trim_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xb76a0952, 0x7c467888, 0x172b1aa9, 0x99f7a455),
		  "NurbsAddTrimCurve",
		  _("Adds a trimming curve to the selected patch"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_curve;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_scale_u;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_scale_v;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_offset_u;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_offset_v;
};

//Create connect_curve factory
k3d::iplugin_factory& add_trim_curve_factory()
{
	return add_trim_curve::get_factory();
}

}//namespace nurbs
}//namespace module

