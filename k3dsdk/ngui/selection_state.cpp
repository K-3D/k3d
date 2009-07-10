// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/idocument.h>
#include <k3dsdk/imetadata.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/inode_selection.h>
#include <k3dsdk/ngui/selection_state.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/result.h>

#include <map>

namespace k3d
{

namespace ngui
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// state::implementation

class state::implementation
{
public:
	static implementation& instance(idocument& Document)
	{
		typedef std::map<idocument*, implementation*> cache_t;
		static cache_t cache;
		cache_t::iterator result = cache.find(&Document);
		if(result == cache.end())
			result = cache.insert(std::make_pair(&Document, new implementation(Document))).first;

		return *result->second;
	}

	inode_selection* node_selection()
	{
		if(!m_node_selection)
		{
			inode_collection::nodes_t nodes = find_nodes<inode_selection>(document.nodes(), "ngui:unique_node", "node_selection");
			if(nodes.size() != 1)
				return 0;
			m_node_selection = dynamic_cast<inode_selection*>(nodes.back());
			// Make sure the node gets updated whenever the metadata is changed or the node is deleted
			imetadata* metadata = dynamic_cast<imetadata*>(m_node_selection);
			m_node_selection_metadata_connection = metadata->connect_metadata_changed_signal(sigc::mem_fun(*this, &implementation::on_node_selection_node_changed));
			m_node_selection_deleted_connection = nodes.back()->deleted_signal().connect(sigc::mem_fun(*this, &implementation::on_node_selection_node_changed));
		}
		return m_node_selection;
	}
	
	idocument& document;

private:
	implementation(idocument& Document) :
		document(Document),
		m_node_selection(0)
	{
	}

	void on_node_selection_node_changed()
	{
		m_node_selection_metadata_connection.disconnect();
		m_node_selection_deleted_connection.disconnect();
		m_node_selection = 0;
	}
	
	/// Cached pointer to the document node selection node
	inode_selection* m_node_selection;
	sigc::connection m_node_selection_metadata_connection;
	sigc::connection m_node_selection_deleted_connection;
};

/////////////////////////////////////////////////////////////////////////////
// state

state::state(idocument& Document) :
	internal(implementation::instance(Document))
{
}

void state::select(inode& Node)
{
	return_if_fail(internal.node_selection());
	internal.node_selection()->select(Node, 1.0);	
}

void state::select_all()
{
	assert_not_implemented();
	select_all_nodes();
}

void state::select_all_nodes()
{
	const nodes_t& nodes = internal.document.nodes().collection();
	for(nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		select(**node);
}

void state::deselect(inode& Node)
{
	if(internal.node_selection())
		internal.node_selection()->select(Node, 0.0);	
}

void state::deselect_all()
{
	assert_not_implemented();
	deselect_all_nodes();
}

void state::deselect_all_nodes()
{
	if(internal.node_selection())
		internal.node_selection()->deselect_all();
}

} // namespace selection

} // namespace ngui

} // namespace k3d

