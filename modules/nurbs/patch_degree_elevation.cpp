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

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
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
class patch_degree_elevation :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	patch_degree_elevation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_degree(init_owner(*this) + init_name(_("degree")) + init_label(_("Degree")) + init_description(_("The current degree will be increased by the amount you specified here")) + init_constraint(constraint::minimum(1 , constraint::maximum(3))) + init_value(1)),
		m_insert_to_v(init_owner(*this) + init_name(_("insert_to_v")) + init_label(_("Elevate v? Otherwise u")) + init_description(_("By default the u direction gets elevated, by checking this box you choose to elevate v")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_degree.changed_signal().connect(make_update_mesh_slot());
		m_insert_to_v.changed_signal().connect(make_update_mesh_slot());
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

		std::vector<k3d::uint_t> my_patches = mod.get_selected_patches();

		if (my_patches.size() == 0)
		{
			k3d::log() << error << "You need to select at least one patch!" << std::endl;
			return;
		}

		int degree = m_degree.pipeline_value();

		for (int i = 0; i < my_patches.size(); i++)
		{
			if (!m_insert_to_v.pipeline_value())
				mod.patch_u_degree_elevation(my_patches.at(i), degree);
			else
				mod.patch_v_degree_elevation(my_patches.at(i), degree);
		}

		k3d::mesh::delete_unused_points(Output);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<patch_degree_elevation, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xe7f334cc, 0x55483bfc, 0x5f2ed290, 0xa0009176),
		  "NurbsPatchDegreeElevation",
		  _("Elevates the selected patches in either u or v direction"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_degree;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_insert_to_v;
};

k3d::iplugin_factory& patch_degree_elevation_factory()
{
	return patch_degree_elevation::get_factory();
}

} //namespace nurbs

} //namespace module

