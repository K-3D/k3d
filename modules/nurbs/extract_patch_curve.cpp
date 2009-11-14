// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

class extract_patch_curve :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	extract_patch_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_parameter(init_owner(*this) + init_name(_("parameter")) + init_label(_("Parameter Value")) + init_description(_("Parameter value (u or v) for which a curve will be extracted")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.0)),
		m_insert_to_v(init_owner(*this) + init_name(_("extract_v_curve")) + init_label(_("Extract V curve")) + init_description(_("If true, a curve in the V direction is extracted (iso-U curve), otherwise it's a U-curve")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_parameter.changed_signal().connect(make_update_mesh_slot());
		m_insert_to_v.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);
		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(Output));

		boost::scoped_ptr<k3d::nurbs_curve::primitive> output_curves(k3d::nurbs_curve::create(Output));
		output_curves->material.push_back(0);
		for(k3d::uint_t prim_idx = 0; prim_idx != Input.primitives.size(); ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> input_patches(k3d::nurbs_patch::validate(Output, *Output.primitives[prim_idx]));
			for(k3d::uint_t patch = 0; patch != input_patches->patch_first_points.size(); ++patch)
			{
				if(input_patches->patch_selections[patch])
				{
					extract_patch_curve_by_parameter(Output, *output_curves, Output, *input_patches, patch, m_parameter.pipeline_value(), !m_insert_to_v.pipeline_value());
					output_curves->material[0] = input_patches->patch_materials[patch];
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<extract_patch_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xd987a77b, 0xb84a96d5, 0x60291eba, 0xe9feb8fc),
		  "NurbsExtractPatchCurve",
		  _("Extract a curve from a patch at a given parameter in  given direction"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_parameter;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_insert_to_v;
};

//Create connect_curve factory
k3d::iplugin_factory& extract_patch_curve_factory()
{
	return extract_patch_curve::get_factory();
}

} //namespace nurbs

} //namespace module
