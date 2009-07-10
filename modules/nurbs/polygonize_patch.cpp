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
class polygonize_patch :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	polygonize_patch(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
			base(Factory, Document),
			m_segments(init_owner(*this) + init_name(_("segments")) + init_label(_("Segments")) + init_description(_("The more segments the better the result")) + init_value(5) + init_constraint(constraint::minimum(3))),
			m_delete_orig(init_owner(*this) + init_name(_("delete_orig")) + init_label(_("Delete original?")) + init_description(_("Delete original NURBS curve?")) + init_value(true)),
			m_flip(init_owner(*this) + init_name(_("flip")) + init_label(_("Flip Vertex order?")) + init_description(_("A face is only visible if its vertices are ordered counter-clockwise. If you don't see the side of the mesh you need, just check this to make all faces look to the other side")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_segments.changed_signal().connect(make_update_mesh_slot());
		m_delete_orig.changed_signal().connect(make_update_mesh_slot());
		m_flip.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		MY_DEBUG << "polygonize_patch::on_update_mesh" << std::endl;
		Output = Input;

		boost::scoped_ptr<k3d::nurbs_patch::primitive> nurbs(get_first_nurbs_patch(Output));
		if(!nurbs)
			return;

		k3d::geometry::merge_selection(m_mesh_selection.pipeline_value(), Output);

		nurbs_patch_modifier mod(Output, *nurbs);
		int my_patch = mod.get_selected_patch();

		if (my_patch < 0)
		{
			k3d::log() << error << nurbs_debug << "You need to select exactly one patch!" << std::endl;
			return;
		}

		mod.polygonize_patch(my_patch, m_segments.pipeline_value(), m_delete_orig.pipeline_value(), m_flip.pipeline_value());
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<polygonize_patch, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x84f89545, 0x2746be1b, 0x045df3b0, 0x539eee95),
		  "NurbsPolygonizePatch",
		  _("Creates a Polyhedron representation of the selected NURBS patch"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_segments;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_orig;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_flip;
};

k3d::iplugin_factory& polygonize_patch_factory()
{
	return polygonize_patch::get_factory();
}

} //namespace nurbs

} //namespace module

