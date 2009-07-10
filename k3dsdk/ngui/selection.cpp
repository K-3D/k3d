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

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/data.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imetadata.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/inode_selection.h>
#include <k3dsdk/log.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/result.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <map>
#include <iostream>

namespace k3d
{

namespace ngui
{

namespace selection
{

namespace detail
{

/// Uses an update policy to replace each selection in a set of MeshInstance nodes.
/// Each update policy must generate a k3d::selection::set based on the current state of the MeshInstance output mesh.
/// Use replace_selection() to apply selection algorithms to an existing mesh.
template<typename UpdatePolicyT>
void replace_selection(const nodes_t& Nodes, const UpdatePolicyT& UpdatePolicy, const bool_t VisibleSelection)
{
	for(nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
	{
		if(classes::MeshInstance() != (*node)->factory().factory_id())
			continue;

		imesh_selection_sink* const mesh_selection_sink = dynamic_cast<imesh_selection_sink*>(*node);
		if(!mesh_selection_sink)
			continue;

		imesh_source* const mesh_source = dynamic_cast<imesh_source*>(*node);
		if(!mesh_source)
			continue;

		const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source->mesh_source_output().property_internal_value());
		if(!mesh)
			continue;

		property::set_internal_value(mesh_selection_sink->mesh_selection_sink_input(), UpdatePolicy(*mesh));
		property::set_internal_value(**node, "show_component_selection", VisibleSelection);
	}
}

/// Update policy for use with replace_selection() that selects all points in a mesh.
const k3d::selection::set select_all_points(const mesh& Mesh)
{
	k3d::selection::set results;

	boost::scoped_ptr<geometry::point_selection::storage> point_selection(geometry::point_selection::create(results));
	geometry::point_selection::reset(*point_selection, 1.0);

	boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(results));
	geometry::primitive_selection::reset(*primitive_selection, 0.0);

	return results;
}

/// Update policy for use with replace_selection() that selects all split edges in a mesh.
const k3d::selection::set select_all_split_edges(const mesh& Mesh)
{
	k3d::selection::set results;

	boost::scoped_ptr<geometry::point_selection::storage> point_selection(geometry::point_selection::create(results));
	geometry::point_selection::reset(*point_selection, 0.0);

	boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(results));
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::CONSTANT, 0.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::UNIFORM, 0.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::VARYING, 0.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::FACE_VARYING, 0.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::SPLIT_EDGE, 1.0);

	return results;
}

/// Update policy for use with replace_selection() that selects all uniform primitives in a mesh.
const k3d::selection::set select_all_uniform(const mesh& Mesh)
{
	k3d::selection::set results;

	boost::scoped_ptr<geometry::point_selection::storage> point_selection(geometry::point_selection::create(results));
	geometry::point_selection::reset(*point_selection, 0.0);

	boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(results));
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::CONSTANT, 0.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::UNIFORM, 1.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::VARYING, 0.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::FACE_VARYING, 0.0);
	geometry::primitive_selection::reset(*primitive_selection, k3d::selection::SPLIT_EDGE, 0.0);

	return results;
}

/*
void invert(k3d::mesh_selection::records_t& Records)
{
	for(k3d::mesh_selection::records_t::iterator record = Records.begin(); record != Records.end(); ++record)
		record->weight = record->weight ? 0.0 : 1.0;
}
*/

/// Update policy for use with replace_selection() that inverts the selection of all points in a mesh.
const k3d::selection::set invert_points(const k3d::mesh& Mesh)
{
assert_not_implemented();
return k3d::selection::set();
//		invert(Selection.points);
}

/// Update policy for use with replace_selection() that inverts the selection of all split edges in a mesh.
const k3d::selection::set invert_split_edges(const k3d::mesh& Mesh)
{
assert_not_implemented();
return k3d::selection::set();
//		invert(Selection.edges);
}

/// Update policy for use with replace_selection() that inverts the selection of all uniform primitives in a mesh.
const k3d::selection::set invert_uniform(const k3d::mesh& Mesh)
{
assert_not_implemented();
return k3d::selection::set();
//		invert(Selection.faces);
}

/// Update policy for use with replace_selection() that deselects all points and primitives in a mesh.
const k3d::selection::set deselect_all(const mesh& Mesh)
{
	k3d::selection::set results;
	geometry::reset_selection(results, 0.0);
	return results;
}

/// Uses an update policy to convert the supplied selection into updates to MeshInstance mesh selections
template<typename UpdatePolicyT>
void merge_interactive_selection(const nodes_t& Nodes, const UpdatePolicyT& UpdatePolicy, const k3d::selection::records& InteractiveSelection)
{
	for(nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
	{
		if(classes::MeshInstance() != (*node)->factory().factory_id())
			continue;

		imesh_selection_sink* const mesh_selection_sink = dynamic_cast<imesh_selection_sink*>(*node);
		if(!mesh_selection_sink)
			continue;

		imesh_source* const mesh_source = dynamic_cast<imesh_source*>(*node);
		if(!mesh_source)
			continue;

		const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source->mesh_source_output().property_internal_value());
		if(!mesh)
			continue;

		const k3d::selection::set current_selection =
			boost::any_cast<k3d::selection::set>(mesh_selection_sink->mesh_selection_sink_input().property_internal_value());

		property::set_internal_value(mesh_selection_sink->mesh_selection_sink_input(), UpdatePolicy(*node, *mesh, current_selection, InteractiveSelection));
		property::set_internal_value(**node, "show_component_selection", true);
	}
}

/// Policy class that updates a mesh_selection to select the given points
struct select_points
{
	select_points(const double_t Weight) :
		weight(Weight)
	{
	}

	const k3d::selection::set operator()(k3d::inode* const Node, const k3d::mesh& Mesh, const k3d::selection::set& CurrentSelection, const k3d::selection::records& InteractiveSelection) const
	{
		k3d::selection::set results = CurrentSelection;

		boost::scoped_ptr<geometry::point_selection::storage> point_selection;
		for(k3d::selection::records::const_iterator record = InteractiveSelection.begin(); record != InteractiveSelection.end(); ++record)
		{
			if(k3d::selection::get_node(*record) != Node)
				continue;

			if(!point_selection)
				point_selection.reset(geometry::point_selection::create(results));				

			for(k3d::selection::record::tokens_t::const_iterator token = record->tokens.begin(); token != record->tokens.end(); ++token)
			{
				switch(token->type)
				{
					case k3d::selection::POINT:
						point_selection->index_begin.push_back(token->id);
						point_selection->index_end.push_back(token->id + 1);
						point_selection->weight.push_back(weight);
						continue;
					default:
						continue;
				}
			}
		}

		return results;
	}

	const double_t weight;
};

/// Policy class that updates a mesh_selection to select the given lines
struct select_split_edges
{
	select_split_edges(const double_t Weight) :
		weight(Weight)
	{
	}

	const k3d::selection::set operator()(k3d::inode* const Node, const k3d::mesh& Mesh, const k3d::selection::set& CurrentSelection, const k3d::selection::records& InteractiveSelection) const
	{
		k3d::selection::set results = CurrentSelection;

		boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection;
		for(k3d::selection::records::const_iterator record = InteractiveSelection.begin(); record != InteractiveSelection.end(); ++record)
		{
			if(k3d::selection::get_node(*record) != Node)
				continue;

			if(!primitive_selection)
				primitive_selection.reset(geometry::primitive_selection::create(results));				

			bool_t found_primitive = false;
			for(k3d::selection::record::tokens_t::const_iterator token = record->tokens.begin(); token != record->tokens.end(); ++token)
			{
				switch(token->type)
				{
					case k3d::selection::PRIMITIVE:
						found_primitive = true;
						if(primitive_selection->primitive_begin.empty() || primitive_selection->primitive_begin.back() != token->id)
						{
							primitive_selection->primitive_begin.push_back(token->id);
							primitive_selection->primitive_end.push_back(token->id + 1);
							primitive_selection->primitive_selection_type.push_back(k3d::selection::SPLIT_EDGE);
							primitive_selection->primitive_first_range.push_back(primitive_selection->index_begin.size());
							primitive_selection->primitive_range_count.push_back(0);
						}
						break;
					case k3d::selection::SPLIT_EDGE:
						if(!found_primitive)
						{
							log() << error << "Selection records missing primitive token" << std::endl;
							break;
						}
						primitive_selection->primitive_range_count.back() += 1;
						primitive_selection->index_begin.push_back(token->id);
						primitive_selection->index_end.push_back(token->id + 1);
						primitive_selection->weight.push_back(weight);
						break;
					default:
						break;
				}
			}
		}

		return results;
	}

	const double_t weight;
};

/// Policy class that updates a mesh_selection to select the given uniform components
struct select_uniform
{
	select_uniform(const double_t Weight) :
		weight(Weight)
	{
	}

	const k3d::selection::set operator()(k3d::inode* const Node, const k3d::mesh& Mesh, const k3d::selection::set& CurrentSelection, const k3d::selection::records& InteractiveSelection) const
	{
		k3d::selection::set results = CurrentSelection;

		boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection;
		for(k3d::selection::records::const_iterator record = InteractiveSelection.begin(); record != InteractiveSelection.end(); ++record)
		{
			if(k3d::selection::get_node(*record) != Node)
				continue;

			if(!primitive_selection)
				primitive_selection.reset(geometry::primitive_selection::create(results));				

			bool_t found_primitive = false;
			for(k3d::selection::record::tokens_t::const_iterator token = record->tokens.begin(); token != record->tokens.end(); ++token)
			{
				switch(token->type)
				{
					case k3d::selection::PRIMITIVE:
						found_primitive = true;
						if(primitive_selection->primitive_begin.empty() || primitive_selection->primitive_begin.back() != token->id)
						{
							primitive_selection->primitive_begin.push_back(token->id);
							primitive_selection->primitive_end.push_back(token->id + 1);
							primitive_selection->primitive_selection_type.push_back(k3d::selection::UNIFORM);
							primitive_selection->primitive_first_range.push_back(primitive_selection->index_begin.size());
							primitive_selection->primitive_range_count.push_back(0);
						}
						break;
					case k3d::selection::UNIFORM:
						if(!found_primitive)
						{
							log() << error << "Selection records missing primitive token" << std::endl;
							break;
						}
						primitive_selection->primitive_range_count.back() += 1;
						primitive_selection->index_begin.push_back(token->id);
						primitive_selection->index_end.push_back(token->id + 1);
						primitive_selection->weight.push_back(weight);
						break;
					default:
						break;
				}
			}
		}

		return results;
	}

	const double_t weight;
};

} // namespace detail

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
                log() << error << "Unknown enumeration [" << text << "]"<< std::endl;

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

	void selection_changed()
	{
assert_not_implemented();
/*
		// Switch to node selection mode when there's no selected mesh in the document
		if(SELECT_NODES != m_selection_mode.internal_value())
		{
			unsigned long selected_mesh_count = 0;

			const nodes_t nodes = selected_nodes();
			for(nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
			{
//				if(dynamic_cast<imesh_source*>(*node))
				{
					++selected_mesh_count;
					break;
				}
			}

			if(!selected_mesh_count)
				set_selection_mode(SELECT_NODES);
		}

		m_active_tool->document_selection_changed();
		m_document_selection_change_signal.emit();
*/
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
	const inode_selection::selected_nodes_t nodes = internal.node_selection()->selected_nodes();
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
	switch(internal.current_mode.internal_value())
	{
		case NODES:
			select_nodes(Selection);
			break;
		case POINTS:
			detail::merge_interactive_selection(selected_nodes(), detail::select_points(1.0), Selection);
			break;
		case SPLIT_EDGES:
			detail::merge_interactive_selection(selected_nodes(), detail::select_split_edges(1.0), Selection);
			break;
		case UNIFORM:
			detail::merge_interactive_selection(selected_nodes(), detail::select_uniform(1.0), Selection);
			break;
	}

	internal.selection_changed();
}

void state::select_nodes(const k3d::selection::records& Selection)
{
	unsigned long selected_nodes = 0;
	inode* selected_node = 0;

	for(k3d::selection::records::const_iterator record = Selection.begin(); record != Selection.end(); ++record)
	{
		if(inode* const node = k3d::selection::get_node(*record))
		{
			select(*node);

			++selected_nodes;
			selected_node = node;
		}
	}

	// Iff one node was selected, send "view node" signals
	if(1 == selected_nodes && selected_node)
	{
assert_not_implemented();
/*
		m_view_node_properties_signal.emit(selected_node);
		m_view_node_history_signal.emit(selected_node);
*/
	}
}

void state::select_all()
{
	switch(internal.current_mode.internal_value())
	{
		case NODES:
			select_all_nodes();
			break;
		case POINTS:
			detail::replace_selection(selected_nodes(), detail::select_all_points, true);
			break;
		case SPLIT_EDGES:
			detail::replace_selection(selected_nodes(), detail::select_all_split_edges, true);
			break;
		case UNIFORM:
			detail::replace_selection(selected_nodes(), detail::select_all_uniform, true);
			break;
	}

	internal.selection_changed();
}

void state::select_all_nodes()
{
	const nodes_t& nodes = internal.document.nodes().collection();
	for(nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		select(**node);
}

void state::invert_selection()
{
	switch(internal.current_mode.internal_value())
	{
		case NODES:
			invert_all_nodes();
			break;
		case POINTS:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_points, true);
			break;
		case SPLIT_EDGES:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_split_edges, true);
			break;
		case UNIFORM:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_uniform, true);
			break;
	}

	internal.selection_changed();
}

void state::invert_all_nodes()
{
	for(k3d::inode_collection::nodes_t::const_iterator node = internal.document.nodes().collection().begin(); node != internal.document.nodes().collection().end(); ++node)
	{
		if(is_selected(**node))
			deselect(**node);
		else
			select(**node);
	}
}

const bool_t state::is_selected(inode& Node)
{
	if(internal.node_selection())
		return internal.node_selection()->selection_weight(Node);
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
	switch(internal.current_mode.internal_value())
	{
		case NODES:
			deselect_nodes(Selection);
			break;
		case POINTS:
			detail::merge_interactive_selection(selected_nodes(), detail::select_points(0.0), Selection);
			break;
		case SPLIT_EDGES:
			detail::merge_interactive_selection(selected_nodes(), detail::select_split_edges(0.0), Selection);
			break;
		case UNIFORM:
			detail::merge_interactive_selection(selected_nodes(), detail::select_uniform(0.0), Selection);
			break;
	}

	internal.selection_changed();
}

void state::deselect_nodes(const k3d::selection::records& Selection)
{
	for(k3d::selection::records::const_iterator record = Selection.begin(); record != Selection.end(); ++record)
	{
		if(k3d::inode* const node = k3d::selection::get_node(*record))
			deselect(*node);
	}
}

void state::deselect_all()
{
	switch(internal.current_mode.internal_value())
	{
		case NODES:
			deselect_all_nodes();
			break;
		case POINTS:
		case SPLIT_EDGES:
		case UNIFORM:
			detail::replace_selection(internal.document.nodes().collection(), detail::deselect_all, false);
			break;
	}

	internal.selection_changed();
}

void state::deselect_all_nodes()
{
	if(internal.node_selection())
		internal.node_selection()->deselect_all();
}

} // namespace selection

} // namespace ngui

} // namespace k3d

