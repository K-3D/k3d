// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// mesh_selection

class mesh_selection :
	public k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> > base;

public:
	mesh_selection(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.property_changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
	}
	
	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_selection,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x8e7ee49b, 0x0c40f479, 0x4c90abb0, 0x784c36c1),
				"MeshSelection",
				_("Applies a selection to a mesh."),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// mesh_selection_factory

k3d::iplugin_factory& mesh_selection_factory()
{
	return mesh_selection::get_factory();
}

} // namespace selection

} // namespace module

