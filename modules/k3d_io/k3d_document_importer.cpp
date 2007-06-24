// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Implements the DocumentReader K-3D plugin, which imports the K-3D native file format
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/data.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/ipersistent.h>
#include <k3dsdk/log.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/serialization.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/vectors.h>
#include <k3dsdk/xml.h>

#include <iostream>

using namespace k3d::xml;

namespace libk3dk3dio
{

/////////////////////////////////////////////////////////////////////////////
// k3d_document_importer

class k3d_document_importer :
	public k3d::ifile_format,
	public k3d::idocument_importer,
	public k3d::ideletable
{
public:
	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& FilePath)
	{
		return k3d::filesystem::extension(FilePath).lowercase().raw() == ".k3d";
	}

	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
	{
		k3d::log() << info << "Reading " << FilePath.native_console_string() << " using " << get_factory().name() << std::endl;

		element xml("k3dml");
		try
		{
			k3d::filesystem::ifstream stream(FilePath);
			hide_progress progress;
			parse(xml, stream, FilePath.native_utf8_string().raw(), progress);
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
			return false;
		}

		// Make sure it's a K3D document ...
		return_val_if_fail(xml.name == "k3dml", false);

		// Look for a document version ...
		std::stringstream version(attribute_text(xml, "version"));
		unsigned long major_version = 0;
		unsigned long minor_version = 0;
		unsigned long revision = 0;
		unsigned long build = 0;
		char point;
		version >> major_version >> point >> minor_version >> point >> revision >> point >> build;

		const k3d::filesystem::path root_path = FilePath.branch_path();
		k3d::persistent_lookup persistent_lookup;
		k3d::ipersistent::load_context context(root_path, persistent_lookup);

		// Load per-node data ...
		if(element* xml_document = find_element(xml, "document"))
		{
			// Handle documents from older versions of the software by modifying the XML
			k3d::upgrade_document(*xml_document);

			// Load nodes
			if(element* xml_nodes = find_element(*xml_document, "nodes"))
			{
				k3d::inode_collection::nodes_t nodes;
				std::vector<k3d::ipersistent*> persistent_nodes;
				std::vector<element*> node_storage;

				for(element::elements_t::iterator xml_node = xml_nodes->children.begin(); xml_node != xml_nodes->children.end(); ++xml_node)
				{
					if(xml_node->name != "node")
						continue;

					if(attribute_value<bool>(*xml_node, "do_not_load", false))
						continue;

					const std::string name = attribute_text(*xml_node, "name");
					const k3d::uuid class_id = attribute_value<k3d::uuid>(*xml_node, "class", k3d::uuid::null());
					if(class_id == k3d::uuid::null())
					{
						k3d::log() << error << "node [" << name << "] with unspecified class ID will not be loaded" << std::endl;
						continue;
					}

					const k3d::ipersistent_lookup::id_type node_id = attribute_value<k3d::ipersistent_lookup::id_type>(*xml_node, "id", 0);
					if(node_id == 0)
					{
						k3d::log() << error << "node [" << name << "] with unspecified ID will not be loaded" << std::endl;
						continue;
					}

					k3d::iplugin_factory* const plugin_factory = k3d::plugin(class_id);
					if(!plugin_factory)
					{
						k3d::log() << error << "node [" << name << "] with unknown class ID [" << class_id << "] will not be loaded" << std::endl;
						continue;
					}

					k3d::idocument_plugin_factory* const document_plugin_factory = dynamic_cast<k3d::idocument_plugin_factory*>(plugin_factory);
					if(!document_plugin_factory)
					{
						k3d::log() << error << "Non-document plugin [" << name << "] will not be loaded" << std::endl;
						continue;
					}

					k3d::inode* const node = document_plugin_factory->create_plugin(*plugin_factory, Document);
					if(!node)
					{
						k3d::log() << error << "Error creating node [" << name << "] instance" << std::endl;
						continue;
					}

					k3d::ipersistent* const persistent = dynamic_cast<k3d::ipersistent*>(node);
					if(!persistent)
					{
						k3d::log() << error << "node [" << name << "] does not support persistence" << std::endl;

						delete dynamic_cast<k3d::ideletable*>(node);
						continue;
					}

					k3d::undoable_new(dynamic_cast<k3d::ideletable*>(node), Document);

					nodes.push_back(node);
					persistent_nodes.push_back(persistent);
					node_storage.push_back(&(*xml_node));

					persistent_lookup.insert_lookup(node_id, node);
				}

				Document.nodes().add_nodes(nodes);

				for(unsigned long i = 0; i != persistent_nodes.size(); ++i)
				{
					persistent_nodes[i]->load(*node_storage[i], context);
				}
			}

			// Load the DAG ...
			k3d::load_dag(Document, *xml_document, context);
		}

		// Load per-plugin-type data ....
		if(element* const xml_application = find_element(xml, "application"))
		{
			if(element* const xml_plugins = find_element(*xml_application, "plugins"))
			{
				for(element::elements_t::iterator xml_plugin = xml_plugins->children.begin(); xml_plugin != xml_plugins->children.end(); ++xml_plugin)
				{
					if(xml_plugin->name != "plugin")
						continue;

					if(attribute_value<bool>(*xml_plugin, "do_not_load", false))
						continue;

					const k3d::uuid class_id = attribute_value<k3d::uuid>(*xml_plugin, "class", k3d::uuid::null());
					if(class_id == k3d::uuid::null())
					{
						k3d::log() << error << "Plugin with unspecified class ID will not be loaded" << std::endl;
						continue;
					}

					if(!Document.plugin_serialization_handlers().count(class_id))
					{
						k3d::log() << error << "Unknown plugin type [" << class_id << "] will not be loaded" << std::endl;
						continue;
					}

					k3d::ipersistent* const handler = Document.plugin_serialization_handlers().find(class_id)->second;
					if(!handler)
					{
						k3d::log() << error << "Invalid serialization handler for plugin type [" << class_id << "] will not be used" << std::endl;
						continue;
					}

					handler->load(*xml_plugin, context);
				}
			}
		}

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<k3d_document_importer, k3d::interface_list<k3d::idocument_importer> > factory(
			k3d::classes::DocumentImporter(),
			"K3DDocumentImporter",
			_("K-3D Native ( .k3d )"),
			"");

		return factory;
	}
};

k3d::iplugin_factory& k3d_document_importer_factory()
{
	return k3d_document_importer::get_factory();
}

} // namespace libk3dk3dio

