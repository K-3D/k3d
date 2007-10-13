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

#include "create_plugins.h"
#include "file_helpers.h"
#include "fstream.h"
#include "iapplication_plugin_factory.h"
#include "ideletable.h"
#include "iscript_engine.h"
#include "plugins.h"
#include "node.h"
#include "nodes.h"
#include "persistent.h"
#include "scripted_node.h"
#include "scripted_node_collection.h"
#include "xml.h"

namespace k3d
{

	namespace detail
	{

	/// Returns the set of all available scripting languages (cached) (taken from scripting.cpp)
	const factories_t& script_engine_factories()
	{
		static factories_t factories;
		if(factories.empty())
			factories = plugins<iscript_engine>();
	
		return factories;
	}
	
	/// Loads the script located at Path into Script
	void load_script(const filesystem::path& Path, std::string& Script)
	{
		size_t n = Path.native_console_string().size();
		if (Path.native_console_string().substr(n-2, 2) != "py")
			return;
		Script.clear();
		k3d::filesystem::ifstream stream(Path);
		std::string line;
		for(k3d::getline(stream, line); !stream.eof(); k3d::getline(stream, line))
		{
			Script += line + "\n";
		}
	}
	
	} // namespace detail

	void scripted_node_collection::load_scripts(const filesystem::path& Path, const bool Recursive)
	{
		m_scripted_nodes.clear();
		
		for(k3d::filesystem::directory_iterator path(Path); path != k3d::filesystem::directory_iterator(); ++path)
		{
			if(k3d::filesystem::is_directory(*path))
			{
				if(Recursive)
					load_scripts(*path, Recursive);
			}
			else
			{
				load_script(*path);
			}
		}
	}
	
	void scripted_node_collection::load_script(const filesystem::path& Path)
	{
		// Get the set of all script engine factories ...
		const factories_t& factories(detail::script_engine_factories());
		
		std::string name = "noname";
		std::string category = "nocategory";
		
		// For every scripting engine, check if it can run the script, and if so extract script metadata
		for(factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
		{
			iscript_engine* const script_engine = dynamic_cast<iscript_engine*>(create_plugin(**factory));
			if(!script_engine)
				continue;
			
			std::string script;
			detail::load_script(Path, script);
			const bool can_execute = script_engine->can_execute(script);
			if(can_execute)
			{
				xml::element metaxml("scriptdata");
				script_engine->get_metadata(script, metaxml);
				if (metaxml.name == "scriptdata")
				{
					for(xml::element::elements_t::iterator element = metaxml.children.begin(); element != metaxml.children.end(); ++element)
					{
						if (element->name == "name")
							name = element->text;
						if (element->name == "category")
							category = element->text;
					}
				}
			}
			
			delete dynamic_cast<ideletable*>(script_engine);
		
			if(can_execute)
			{
				m_scripted_nodes.push_back(scripted_node_stub(Path, name, category));
				break;
			}
		}
	}
	
k3d::inode* create_scripted_node(k3d::idocument& Document, const scripted_node_stub& Stub)
{
	if(k3d::inode* node = k3d::create_plugin<k3d::inode>("NullOutputScript", Document, k3d::unique_name(Document.nodes(), Stub.name)))
	{
		typedef k3d::scripted_node<k3d::persistent<k3d::node> > scripted_node_t;
		scripted_node_t* scripted_node = dynamic_cast<scripted_node_t*>(node);
		return_val_if_fail(scripted_node, node);
		std::string script;
		detail::load_script(Stub.path, script);
		scripted_node->set_script(script);
		return node;
	}
	else
	{
		k3d::log() << error << "Could not load plugin NullOutputScript to create scripted node " << Stub.name << std::endl;
		return 0;
	}
}

} // namespace k3d
