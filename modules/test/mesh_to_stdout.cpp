// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/node.h>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// mesh_to_stdout

class mesh_to_stdout :
	public k3d::node,
	public k3d::imesh_sink
{
	typedef k3d::node base;

public:
	mesh_to_stdout(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<k3d::mesh*>(0))
	{
		m_input_mesh.changed_signal().connect(sigc::mem_fun(*this, &mesh_to_stdout::mesh_changed));
	}

	void mesh_changed(iunknown* const Hint)
	{
		if(const k3d::mesh* const input_mesh = m_input_mesh.pipeline_value())
			std::cout << *input_mesh << std::endl;
	}
	
	k3d::iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_to_stdout,
			k3d::interface_list<k3d::imesh_sink> > factory(
				k3d::uuid(0x45ee8c94, 0x223d4ba6, 0xb7eec188, 0xcc9cff65),
				"MeshToStdout",
				_("Prints mesh information to stdout"),
				"Test",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input_mesh;
};

/////////////////////////////////////////////////////////////////////////////
// mesh_to_stdout_factory

k3d::iplugin_factory& mesh_to_stdout_factory()
{
	return mesh_to_stdout::get_factory();
}

} // namespace test

} // namespace module

