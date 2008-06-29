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
	\author Barbiero Mattia
*/


#include <dae.h>
#include <dom/domCOLLADA.h>
#include <dom/domConstants.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/gprim_factory.h>
#include "intGeometry.h"
#include "integration.h"

namespace module
{

namespace collada
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
		const k3d::filesystem::path path = m_file.pipeline_value();
		if(path.empty())
			return;

		// Instantiate the reference implementation
		DAE dae;

		k3d::log() << info << "Loading .dae file: " << path.native_console_string() << std::endl;
		domCOLLADA* root = dae.open(k3d::string_cast<k3d::filesystem::path>(path));
		if(!root) {
            		k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "]" << std::endl;
			return;
		}
		// Do the conversion. The conversion process throws an exception on error, so
		// we'll include a try/catch handler.
		//convertModel(*root, Mesh);
		daeParser dae_file(*root);
		Mesh = dae_file.get_mesh();
	
		// destroy the objects we created during the conversion process
		freeConversionObjects<Node, domNode>(dae);
		freeConversionObjects<intGeometry, domGeometry>(dae);
		//freeConversionObjects<Material, domMaterial>(dae);
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader,k3d::interface_list<k3d::imesh_source,k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0xf8bbd8fb, 0x7d47911e, 0xbf64f3a3, 0x25652955),
			"ColladaMeshReader",
			_("Mesh reader that loads external COLLADA (.dae) files into the document by reference"),
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

} // namespace collada

} // namespace module

