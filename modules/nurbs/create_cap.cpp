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
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>

#include <iostream>
#include <vector>

#include "nurbs_patch_modifier.h"

namespace module
{

namespace nurbs
{
class create_cap :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	create_cap(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
			base(Factory, Document),
			m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the curve which was used to create the cap")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		if (!k3d::validate_nurbs_curve_groups(Output))
			return;

		merge_selection(m_mesh_selection.pipeline_value(), Output);


		nurbs_curve_modifier mod(Output);
		int my_curve = mod.selected_curve();

		if (my_curve < 0)
		{
			k3d::log() << error << nurbs_debug << "You need to select exactly one curve!" << std::endl;
			return;
		}

		if (!mod.create_cap(my_curve))
		{
			k3d::log() << error << nurbs_debug << "The selected curve is no loop!" << std::endl;
			return;
		}

		if (m_delete_original.pipeline_value())
		{
			mod.delete_curve(my_curve);
		}

		assert_warning(k3d::validate_nurbs_curve_groups(Output));
		assert_warning(k3d::validate_nurbs_patches(Output));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<create_cap, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x3f8d9448, 0x5d442835, 0x7d4d0f87, 0x4c0e0593),
		  "NurbsCreateCap",
		  _("Creates a NURBS surface filling out the given closed curve"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
};

//Create connect_curve factory
k3d::iplugin_factory& create_cap_factory()
{
	return create_cap::get_factory();
}

}//namespace nurbs
} //namespace module

