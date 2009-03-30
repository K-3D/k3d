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

#include "nurbs_patch_modifier.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>

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
		m_distance(init_owner(*this) + init_name(_("distance")) + init_label(_("Distance")) + init_description(_("How far to extrude the patch")) + init_step_increment(0.5) + init_units(typeid(k3d::measurement::scalar)) + init_value(1.0)),
		m_along(init_owner(*this) + init_name("along") + init_label(_("Extrude along")) + init_description(_("Axis along which the patch gets extruded")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values())),
		m_cap(init_owner(*this) + init_name(_("cap")) + init_label(_("Create Cap?")) + init_description(_("Extrusion can either create a cap or not")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_distance.changed_signal().connect(make_update_mesh_slot());
		m_along.changed_signal().connect(make_update_mesh_slot());
		m_cap.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		boost::scoped_ptr<k3d::nurbs_patch::primitive> nurbs(get_first_nurbs_patch(Output));
		if(!nurbs)
			return;

		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);

		nurbs_patch_modifier mod(Output, *nurbs);
		int my_patch = mod.get_selected_patch();

		if (my_patch < 0)
		{
			k3d::log() << error << nurbs_debug << "You need to select exactly one patch!" << std::endl;
			return;
		}

		mod.extrude_patch(my_patch, m_along.pipeline_value(), m_distance.pipeline_value(), m_cap.pipeline_value());
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
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap;
};

//Create connect_curve factory
k3d::iplugin_factory& extrude_patch_factory()
{
	return extrude_patch::get_factory();
}

} //namespace nurbs

} //namespace module

