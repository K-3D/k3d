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
	\author Tim Shead (tshead@k-3d.com)
	\author Wladyslaw Strugala (fizws@julia.univ.gda.pl)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/material.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/serialization.h>

using namespace k3d::xml;

#include <k3dsdk/fstream.h>

namespace libk3dk3dio
{

/////////////////////////////////////////////////////////////////////////////
// k3d_mesh_reader

class k3d_mesh_reader :
	public k3d::mesh_source<k3d::persistent<k3d::node> >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::persistent<k3d::node> > base;

public:
	k3d_mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("obj_files"))
	{
		m_file.changed_signal().connect(make_topology_changed_slot());
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

		k3d::log() << info << "Reading " << path.native_console_string() << " using " << get_factory().name() << std::endl;
		k3d::filesystem::ifstream file(path);
		if(!file)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "]" << std::endl;
			return;
		}

		k3d::xml::element xml_document("k3dml");
		file >> xml_document;

		const k3d::filesystem::path root_path = path.branch_path();
		k3d::persistent_lookup lookup;
		k3d::ipersistent::load_context context(root_path, lookup);

		if(k3d::xml::element* const xml_mesh_arrays = k3d::xml::find_element(xml_document, "mesh_arrays"))
			k3d::load_mesh(Mesh, *xml_mesh_arrays, context);
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<k3d_mesh_reader,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_storage> > > factory(
				k3d::uuid(0x7dd19499, 0x965e4c06, 0xb970f545, 0x0e47855d),
				"K3DMeshReader",
				_("Mesh reader that loads external K-3D XML (.k3d) files"),
				"MeshReader");

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& k3d_mesh_reader_factory()
{
	return k3d_mesh_reader::get_factory();
}

} // namespace libk3dk3dio

