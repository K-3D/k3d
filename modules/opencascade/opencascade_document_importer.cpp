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

/** \file A STEP CAD file format reader, using OpenCascade
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/iparentable.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/gprim_factory.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/properties.h>

#include <boost/assign/list_of.hpp>

#include "opencascade_to_k3d.h"

namespace module
{

namespace opencascade
{

namespace detail
{
	/// Creates a new FrozenMesh using the given name, and returns a pointer to the MeshInstance of the new node
	k3d::inode* create_frozen_mesh(k3d::idocument& Document, const std::string& Name, k3d::mesh* Mesh)
	{
		std::string unique_name = k3d::unique_name(Document.nodes(), Name);
		// The frozen mesh
		k3d::inode* const mesh_node = k3d::plugin::create<k3d::inode>(k3d::classes::FrozenMesh(), Document, unique_name);
		// The mesh instance
		k3d::inode* const mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), Document, unique_name + " Instance");
		// Set painters
		const k3d::nodes_t gl_nodes = k3d::find_nodes(Document.nodes(), "GL Default Painter");
		k3d::inode* gl_painter = (1 == gl_nodes.size()) ? *gl_nodes.begin() : 0;
		const k3d::nodes_t ri_nodes = k3d::find_nodes(Document.nodes(), "RenderMan Default Painter");
		k3d::inode* ri_painter = (1 == ri_nodes.size()) ? *ri_nodes.begin() : 0;
		k3d::property::set_internal_value(*mesh_instance, "gl_painter", gl_painter);
		k3d::property::set_internal_value(*mesh_instance, "ri_painter", ri_painter);
		// Connect the MeshInstance
		k3d::ipipeline::dependencies_t dependencies;
		k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(mesh_instance);
		k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(mesh_node);
		if(mesh_sink && mesh_node)
			dependencies.insert(std::make_pair(&mesh_sink->mesh_sink_input(), &mesh_source->mesh_source_output()));
		Document.pipeline().set_dependencies(dependencies);
		k3d::imesh_storage* mesh_storage = dynamic_cast<k3d::imesh_storage*>(mesh_node);
		return_val_if_fail(mesh_storage, 0);
		mesh_storage->reset_mesh(Mesh);
		
		// Make the mesh_instance visible (shamelessly stolen from document_state.cpp)
		const k3d::inode_collection::nodes_t::const_iterator doc_node_end = Document.nodes().collection().end();
		for(k3d::inode_collection::nodes_t::const_iterator doc_node = Document.nodes().collection().begin(); doc_node != doc_node_end; ++doc_node)
		{
			if(k3d::inode_collection_sink* const node_collection_sink = dynamic_cast<k3d::inode_collection_sink*>(*doc_node))
			{
				const k3d::inode_collection_sink::properties_t properties = node_collection_sink->node_collection_properties();
				for(k3d::inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				{
					if(k3d::inode_collection_property* const node_collection_property = dynamic_cast<k3d::inode_collection_property*>(*property))
					{
						k3d::inode_collection_property::nodes_t nodes = k3d::property::internal_value<k3d::inode_collection_property::nodes_t>(**property);
						if(node_collection_property->property_allow(*mesh_instance))
							nodes.push_back(mesh_instance);
						k3d::property::set_internal_value(**property, nodes);
					}
				}
			}
		}
		
		return mesh_instance;
	}
	
	/// Sets the parent of ChildNode to ParentNode
	void set_parent(k3d::inode* ParentNode, k3d::inode* ChildNode)
	{
		k3d::iparentable* child = dynamic_cast<k3d::iparentable*>(ChildNode);
		return_if_fail(child);
		k3d::property::set_internal_value(child->parent(), ParentNode);
	}

	void process_level(opencascade_document_processor& OpenCascadeDocument, k3d::idocument& Document, k3d::inode* Parent = 0)
	{
		for (; OpenCascadeDocument.more(); OpenCascadeDocument.next())
		{
			k3d::mesh* mesh = new k3d::mesh();
			k3d::gprim_factory factory(*mesh);
			std::string name;
			OpenCascadeDocument.process_current(factory, name);
			k3d::inode* frozen_mesh = create_frozen_mesh(Document, name, mesh);
			if (Parent)
				set_parent(Parent, frozen_mesh);
			if (OpenCascadeDocument.has_children())
			{
				OpenCascadeDocument.increase_level();
				process_level(OpenCascadeDocument, Document, frozen_mesh);
				OpenCascadeDocument.decrease_level();
			}
		}
	}
} // namespace detail

class opencascade_document_importer :
	public k3d::idocument_importer
{
public:
	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
	{
		try
		{
			opencascade_document_processor document(FilePath);
			detail::process_level(document, Document);
		}
		catch(...)
		{
			k3d::log() << error << "Error loading OpenCascade document" << std::endl;
			return false;
		}
		return true;
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<opencascade_document_importer, k3d::interface_list<k3d::idocument_importer> > factory(
			k3d::uuid(0x56b2a836, 0x8a4dd32d, 0x0b6d3a9c, 0x708dddbe),
			"OpenCascadeDocumentImporter",
			_("OpenCascade ( .step, .iges, .brep )"),
			"",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("k3d:mime-types", "application/x-step application/x-iges application/x-brep"));

		return factory;
	}
};

k3d::iplugin_factory& opencascade_document_importer_factory()
{
	return opencascade_document_importer::get_factory();
}

} // opencascade

} // module
