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
#include <k3dsdk/mesh_reader.h>
#include <k3dsdk/node.h>
#include <k3dsdk/path.h>

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

class opencascade_mesh_reader_implementation : public k3d::mesh_reader<k3d::node>
{
	typedef k3d::mesh_reader<k3d::node> base;
public:
	opencascade_mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{}
	
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
	/// k3d::mesh_reader implementation
	void on_load_mesh(const k3d::filesystem::path& Path, k3d::mesh& Output)
	{
		k3d::gprim_factory factory(Output);
		try
		{
			opencascade_document_processor document(Path);
			detail::process_level(document, factory);
		}
		catch(...)
		{
			k3d::log() << error << "Error loading OpenCascade document" << std::endl;
		}
	}
		
};

k3d::iplugin_factory& opencascade_mesh_reader_factory()
{
	return opencascade_mesh_reader_implementation::get_factory();
}

} // opencascade

} // module
