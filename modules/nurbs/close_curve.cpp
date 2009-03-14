
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

#include "nurbs_curve_modifier.h"

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
class close_curve :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	close_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_keep_ends(init_owner(*this) + init_name("keep_ends") + init_label(_("Keep end points as CVs")) + init_description(_("If enabled, the endpoints will work as CVs and a new endpoint gets added")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_keep_ends.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		boost::scoped_ptr<k3d::nurbs_curve::primitive> nurbs(k3d::nurbs_curve::validate(Output));
		if(!nurbs)
			return;

		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);

		nurbs_curve_modifier mod(Output);
		int my_curve = mod.selected_curve();

		if (my_curve < 0)
		{
			k3d::log() << error << "You need to select exactly curve!" << std::endl;
			return;
		}

		mod.close_curve(my_curve, m_keep_ends.pipeline_value());
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<close_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xc412ed0d, 0xbf48776e, 0x5c703091, 0x8b788353),
		  "NurbsCloseCurve",
		  _("Connects the 2 end points of the selected curve"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_keep_ends;
};

//Create connect_curve factory
k3d::iplugin_factory& close_curve_factory()
{
	return close_curve::get_factory();
}

}//namespace nurbs
} //namespace module
