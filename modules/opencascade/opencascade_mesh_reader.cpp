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

/** \file A STEP CAD file format reader, using OpenCascade
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/gprim_factory.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/path.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/shared_pointer.h>

#include "opencascade_to_k3d.h"

namespace module
{

namespace opencascade
{

namespace detail
{
	void process_level(opencascade_document_processor& Document, k3d::gprim_factory& Factory)
	{
		for (; Document.more(); Document.next())
		{
			std::string name;
			Document.process_current(Factory, name);
			if (Document.has_children())
			{
				Document.increase_level();
				process_level(Document, Factory);
				Document.decrease_level();
			}
		}
	}
}

class opencascade_mesh_reader_implementation :
	public k3d::mesh_source<k3d::persistent<k3d::node> >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::persistent<k3d::node> > base;

public:
	opencascade_mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("dxf_files"))
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

		k3d::gprim_factory factory(Mesh);
		try
		{
			opencascade_document_processor document(path);
			detail::process_level(document, factory);
		}
		catch(...)
		{
			k3d::log() << error << "Error loading OpenCascade document" << std::endl;
		}
		
		// OpenCascade works in mm, but K-3D works in m, so scale everything
		k3d::mesh::points_t& points = *k3d::make_unique(Mesh.points);
		for (k3d::uint_t point = 0; point != points.size(); ++point)
			points[point] *= 0.001;
	}
	
	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<opencascade_mesh_reader_implementation,
	              k3d::interface_list<k3d::imesh_source,
	              k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0x8e9c8376, 0x9f44e539, 0x90f9f7b7, 0x02ab8a1f),
			"OpenCascadeMeshReader",
			_("Mesh reader that loads external STEP (.stp, .step) files into the document by reference"),
			"MeshReader");
	
		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
		
};

k3d::iplugin_factory& opencascade_mesh_reader_factory()
{
	return opencascade_mesh_reader_implementation::get_factory();
}

} // opencascade

} // module
