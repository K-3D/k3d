#ifndef SCRIPTED_NODE_COLLECTION_H_
#define SCRIPTED_NODE_COLLECTION_H_

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

/** \file \brief Declares a collection of scripted nodes, analogous to plugin_factory_collection
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "path.h"

#include <vector>

namespace k3d
{

class inode;

/// Stores the properties of a script prior to actual node creation
struct scripted_node_stub
{
	/// Constructor
	scripted_node_stub(const filesystem::path& Path, const std::string& Name, const std::string& Category) :
		path(Path),
		name(Name),
		category(Category)
	{}
	 
	/// Path to the script file
	filesystem::path path;
	
	/// Name of the script
	std::string name;
	
	/// Category of the script
	std::string category;
};

/// Stores a collection of scripted node stubs, read from scripts in a directory.
class scripted_node_collection
{
public:
	typedef std::vector<scripted_node_stub> collection_t;
	
	/// Singleton implementation
	static scripted_node_collection& instance()
	{
		static scripted_node_collection m_instance;
		return m_instance;
	}
	
	void load_scripts(const filesystem::path& Path, const bool Recursive);
	
	/// Returns the cached collection of scripts
	const collection_t& collection() const
	{
		return m_scripted_nodes;
	}
private:
	collection_t m_scripted_nodes;
	
	void load_script(const filesystem::path& Path);
	
	scripted_node_collection() {}
};

k3d::inode* create_scripted_node(k3d::idocument& Document, const scripted_node_stub& Stub);


} // namespace k3d

#endif /*SCRIPTED_NODE_COLLECTION_H_*/
