// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "gprim_factory.h"
#include "obj_parser.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <k3dsdk/fstream.h>

namespace libk3dobj
{

/////////////////////////////////////////////////////////////////////////////
// obj_mesh_reader_implementation

class obj_mesh_reader_implementation :
	public k3d::mesh_source<k3d::persistent<k3d::node> >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::persistent<k3d::node> > base;

public:
	obj_mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("obj_files")),
		m_texture_u(init_owner(*this) + init_name("texture_u") + init_label(_("Texture U")) + init_description(_("Texture U")) + init_value(std::string("s"))),
		m_texture_v(init_owner(*this) + init_name("texture_v") + init_label(_("Texture V")) + init_description(_("Texture V")) + init_value(std::string("t"))),
		m_texture_w(init_owner(*this) + init_name("texture_w") + init_label(_("Texture W")) + init_description(_("Texture W")) + init_value(std::string("w")))
	{
		m_file.changed_signal().connect(make_topology_changed_slot());
//		m_texture_u.changed_signal().connect(make_reset_mesh_slot());
//		m_texture_v.changed_signal().connect(make_reset_mesh_slot());
//		m_texture_w.changed_signal().connect(make_reset_mesh_slot());
	}

	void reset_mesh(k3d::mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::filesystem::path path = m_file.value();
		if(path.empty())
			return;

		k3d::log() << info << "Loading .obj file: " << path.native_console_string() << std::endl;
		k3d::filesystem::ifstream file(path);
		if(!file)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "]" << std::endl;
			return;
		}

		my_parser parser(Mesh);
		parser.parse(file);
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<obj_mesh_reader_implementation,
                k3d::interface_list<k3d::imesh_source,
                k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0x650f96b9, 0xe1c145d5, 0x9eda410d, 0x13f4e98f),
			"OBJMeshReader",
			_("Mesh reader that loads external Wavefront (.obj) files into the document by reference"),
			"MeshReader");

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_texture_u;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_texture_v;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_texture_w;

	/// Implementation of obj_parser that instantiates just points and faces to start ...
	class my_parser :
		public obj_parser
	{
	public:
		my_parser(k3d::mesh& Mesh) :
			factory(Mesh)
		{
		}

	private:
		gprim_factory factory;

		void on_vertex(const k3d::point4& Vertex)
		{
			factory.add_point(k3d::point3(Vertex[0] / Vertex[3], Vertex[1] / Vertex[3], Vertex[2] / Vertex[3]));
		}

		void on_face(const k3d::mesh::indices_t& Points, const k3d::mesh::indices_t& TexturePoints, const k3d::mesh::indices_t& Normals)
		{
			factory.add_polygon(Points);
		}
	};
};

k3d::iplugin_factory& obj_mesh_reader_factory()
{
	return obj_mesh_reader_implementation::get_factory();
}

} // namespace libk3dobj

