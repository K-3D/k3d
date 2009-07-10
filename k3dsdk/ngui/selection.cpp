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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "selection.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/data.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/imetadata.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/inode_selection.h>
#include <k3dsdk/log.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/result.h>

#include <map>
#include <iostream>

namespace k3d
{

namespace ngui
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// mode

std::ostream& operator<<(std::ostream& Stream, const mode& RHS)
{
        switch(RHS)
	{
		case NODES:
			Stream << "nodes";
			break;
		case POINTS:
			Stream << "points";
			break;
		case SPLIT_EDGES:
			Stream << "split_edges";
			break;
		case UNIFORM:
			Stream << "uniform";
			break;
	}

        return Stream;
}

std::istream& operator>>(std::istream& Stream, mode& RHS)
{
        std::string text;
        Stream >> text;

        if(text == "nodes")
                RHS = NODES;
        else if(text == "points")
                RHS = POINTS;
        else if(text == "split_edges")
                RHS = SPLIT_EDGES;
        else if(text == "uniform")
                RHS = UNIFORM;
        else
                k3d::log() << error << "Unknown enumeration [" << text << "]"<< std::endl;

        return Stream;
}

/// Provides human-readable labels for the selection::mode enumeration
static const ienumeration_property::enumeration_values_t& mode_values()
{
	static ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(ienumeration_property::enumeration_value_t(_("Nodes"), "nodes", _("Select Nodes")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Points"), "points", _("Select Points")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Lines"), "lines", _("Select Lines")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Faces"), "faces", _("Select Faces")));
	}

	return values;
}

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

	/// Defines storage for the current document-wide selection mode
	/** \todo Restore undo/redo capability */
//	k3d_data(selection::mode, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, no_property, no_serialization) current_mode;
	k3d_data(selection::mode, immutable_name, explicit_change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) current_mode;

private:
	implementation(idocument& Document) :
		document(Document),
		current_mode(init_name("selection_mode") + init_label(_("Selection Type")) + init_description(_("Sets selection mode (nodes, faces, edges, points, etc)")) + init_value(selection::NODES) + init_values(mode_values())),
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

const mode state::current_mode()
{
	return internal.current_mode.internal_value();
}

void state::set_current_mode(const mode Mode)
{
	internal.current_mode.set_value(Mode);
}

sigc::connection state::connect_current_mode_changed_signal(const sigc::slot<void, ihint*>& Slot)
{
	return internal.current_mode.changed_signal().connect(Slot);
}

const std::vector<inode*> state::selected_nodes()
{
	return_val_if_fail(internal.node_selection(), std::vector<inode*>());
	const k3d::inode_selection::selected_nodes_t nodes = internal.node_selection()->selected_nodes();
	return std::vector<inode*>(nodes.begin(), nodes.end());
}

void state::select(inode& Node)
{
	return_if_fail(internal.node_selection());
	internal.node_selection()->select(Node, 1.0);	
}

void state::select(const k3d::selection::record& Selection)
{
	select(k3d::selection::records(1, Selection));
}

void state::select(const k3d::selection::records& Selection)
{
	assert_not_implemented();
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

void state::invert_selection()
{
	assert_not_implemented();
/*
	switch(m_selection_mode.internal_value())
	{
		case SELECT_NODES:
			invert_node_selection();
			break;
		case SELECT_POINTS:
			detail::update_component_selection(m_document.nodes().collection(), detail::invert_points(), true);
			break;
		case SELECT_SPLIT_EDGES:
			detail::update_component_selection(m_document.nodes().collection(), detail::invert_split_edges(), true);
			break;
		case SELECT_UNIFORM:
			detail::update_component_selection(m_document.nodes().collection(), detail::invert_uniform(), true);
			break;
	}

	selection_changed();
*/
}

void state::deselect(inode& Node)
{
	if(internal.node_selection())
		internal.node_selection()->select(Node, 0.0);	
}

void state::deselect(const k3d::selection::record& Selection)
{
	select(k3d::selection::records(1, Selection));
}

void state::deselect(const k3d::selection::records& Selection)
{
	assert_not_implemented();
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

