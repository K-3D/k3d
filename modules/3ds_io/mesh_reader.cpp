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
	\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/gprim_factory.h>
#include "3ds.h"
#include "3dschunknames.h"

namespace module
{

namespace f3ds
{

namespace io
{



/////////////////////////////////////////////////////////////////////////////
// mesh_reader

class mesh_reader :
	public k3d::mesh_source<k3d::node >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::node > base;

public:
	mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) : base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("dae_files")),
		m_texture_u(init_owner(*this) + init_name("texture_u") + init_label(_("Texture U")) + init_description(_("Texture U")) + init_value(std::string("s"))),
		m_texture_v(init_owner(*this) + init_name("texture_v") + init_label(_("Texture V")) + init_description(_("Texture V")) + init_value(std::string("t"))),
		m_texture_w(init_owner(*this) + init_name("texture_w") + init_label(_("Texture W")) + init_description(_("Texture W")) + init_value(std::string("w")))
	{
		m_file.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
//		m_texture_u.changed_signal().connect(make_reset_mesh_slot());
//		m_texture_v.changed_signal().connect(make_reset_mesh_slot());
//		m_texture_w.changed_signal().connect(make_reset_mesh_slot());
	}

	void reset_mesh(k3d::mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}

	void on_update_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::filesystem::path path = m_file.pipeline_value();
		if(path.empty())
			return;

		C3dsParser c3dsfile(path.native_console_string().c_str());

		k3d::gprim_factory factory(Mesh);

		float *verts = NULL;
		unsigned short *polys;
		unsigned short chunkname;
		unsigned short numverts;
		unsigned short numpolys;
		unsigned long mem = 0;
		unsigned long mem_poly = 0;
		while(!c3dsfile.Eof()) {
			chunkname = c3dsfile.GetChunkName();
			switch (chunkname) {
				case MAIN3DS:
					c3dsfile.EnterChunk();
					break;
				case EDIT3DS:
					c3dsfile.EnterChunk();
					break;
				case EDIT_OBJECT:
					c3dsfile.SkipStrData();
					break;
				case OBJ_TRIMESH:
					c3dsfile.EnterChunk();
					break;
				case TRI_FACEL1:
					c3dsfile.GetChunkData(&numpolys,2);
					mem_poly = numpolys * 4 * 2;
					polys = (unsigned short *) malloc(mem_poly);
					c3dsfile.GetChunkData(polys,mem_poly,2);
					for (unsigned int x=0; x<4*(unsigned int)numpolys; x+=4)
					{
						k3d::mesh::indices_t vertex_indices;
						for(int i=0; i<3; i++)
						{
							vertex_indices.push_back(polys[x+i]);
							k3d::log() << debug << polys[x+i] << std::endl;
						}
						factory.add_polygon(vertex_indices);
					}
					c3dsfile.SkipChunk();
					break;
				case TRI_VERTEXL:
					c3dsfile.GetChunkData(&numverts, 2);
					mem = numverts * 3 * 4; // numverts vertices * 3 floats per vertex * 4 bytes per float
					verts = (float *)malloc(mem);
					if (verts == NULL)
					{
						k3d::log() << error << "No vertex data!" << std::endl;
						return;
					}
					c3dsfile.GetChunkData(verts, mem, 2);
					for (unsigned int x = 0; x < 3 * (unsigned int)numverts; x += 3) 
					{
						factory.add_point(k3d::point4(verts[x+0], verts[x+1], verts[x+2],1));
					}
				default:
					c3dsfile.SkipChunk();
					break;
			}
		}
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader,k3d::interface_list<k3d::imesh_source,k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0xf8bbd8fb, 0x7d47911e, 0xbf64f3a3, 0x25652955),
			"3DSMeshReader",
			_("Mesh reader that loads external 3ds (.3ds) files into the document by reference"),
			"MeshReader");

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_texture_u;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_texture_v;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_texture_w;
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader::get_factory();
}

} // namespace io

} // namespace f3ds

} // namespace module
