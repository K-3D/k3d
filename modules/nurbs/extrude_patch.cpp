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

#include <k3dsdk/axis.h>
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
class extrude_patch :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	extrude_patch(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name(_("distance")) + init_label(_("Distance")) + init_description(_("How far to extrude the curve")) + init_step_increment(0.5) + init_units(typeid(k3d::measurement::scalar)) + init_value(1.0)),
		m_segments(init_owner(*this) + init_name("segments") + init_label(_("segments")) + init_description(_("Segments")) + init_value(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_along(init_owner(*this) + init_name("along") + init_label(_("Extrude along")) + init_description(_("Axis along which the curve gets extruded")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values()))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_distance.changed_signal().connect(make_update_mesh_slot());
		m_segments.changed_signal().connect(make_update_mesh_slot());
		m_along.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		double distance = m_distance.pipeline_value();
		k3d::axis axis = m_along.pipeline_value();
		int segments = m_segments.pipeline_value();

		// Determine the start and end point of the extrusion vector
		const k3d::point3 startpoint(0,0,0);
		k3d::point3 endpoint;
		switch (axis)
		{
		case k3d::X:
			endpoint = k3d::point3(distance, 0.0, 0.0);
			break;
		case k3d::Y:
			endpoint = k3d::point3(0.0, distance, 0.0);
			break;
		case k3d::Z:
			endpoint = k3d::point3(0.0, 0.0, distance);
			break;
		}

		// Create a straight curve from this vector to traverse along
		k3d::mesh extrusion_vector_mesh;
		extrusion_vector_mesh.point_selection.create();
		extrusion_vector_mesh.points.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> extrusion_vector_primitive(k3d::nurbs_curve::create(extrusion_vector_mesh));
		straight_line(startpoint, endpoint, segments, *extrusion_vector_primitive, extrusion_vector_mesh);
		extrusion_vector_primitive->curve_selections[0] = 1.0;
		extrusion_vector_primitive->material.push_back(0);

		const k3d::uint_t prim_count = Output.primitives.size();
		for(k3d::uint_t prim_idx = 0; prim_idx != prim_count; ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_patch::primitive> patches(k3d::nurbs_patch::validate(Output, Output.primitives[prim_idx]));
			if(!patches)
				continue;
			const k3d::mesh::selection_t patch_selections = patches->patch_selections;
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> const_patches(k3d::nurbs_patch::validate(Output, *Output.primitives[prim_idx]));

			for(k3d::uint_t patch = 0; patch != patch_selections.size(); ++patch)
			{
				if(!patch_selections[patch])
					continue;
				// storage for the patch boundary curves
				k3d::mesh tmp_mesh;
				tmp_mesh.points.create();
				tmp_mesh.point_selection.create();
				boost::scoped_ptr<k3d::nurbs_curve::primitive> tmp_curves(k3d::nurbs_curve::create(tmp_mesh));
				extract_patch_curve_by_number(tmp_mesh, *tmp_curves, Output, *const_patches, patch, 0, true);
				extract_patch_curve_by_number(tmp_mesh, *tmp_curves, Output, *const_patches, patch, 0, false);
				extract_patch_curve_by_number(tmp_mesh, *tmp_curves, Output, *const_patches, patch, const_patches->patch_v_point_counts[patch] - 1, true);
				extract_patch_curve_by_number(tmp_mesh, *tmp_curves, Output, *const_patches, patch, const_patches->patch_u_point_counts[patch] - 1, false);
				tmp_curves->curve_selections.assign(4, 1.0);
				tmp_curves->material.push_back(const_patches->patch_materials[patch]);

				traverse_curve(tmp_mesh, 0, extrusion_vector_mesh, Output, *patches);
			}
		}
		replace_duplicate_points(Output);

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<extrude_patch, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xfadb3ef0, 0xdc41c0f7, 0xe6f1caa6, 0x810ddfa5),
		  "NurbsExtrudePatch",
		  _("Extrudes the selected patch along a coordinate axis"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_distance;
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_along;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_segments;
};

//Create connect_curve factory
k3d::iplugin_factory& extrude_patch_factory()
{
	return extrude_patch::get_factory();
}

} //namespace nurbs

} //namespace module

