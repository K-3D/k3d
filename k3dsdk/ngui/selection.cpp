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

/// Uses an update policy to alter the set of all selected MeshInstance mesh selections
template<typename UpdatePolicyT>
void set_component_selection(const nodes_t& Nodes, const UpdatePolicyT& UpdatePolicy, const bool_t VisibleSelection)
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

		k3d::selection::set selection =
			boost::any_cast<k3d::selection::set>(mesh_selection_sink->mesh_selection_sink_input().property_internal_value());

		UpdatePolicy(*mesh, selection);

		property::set_internal_value(mesh_selection_sink->mesh_selection_sink_input(), selection);
		property::set_internal_value(**node, "show_component_selection", VisibleSelection);
	}
}

struct select_all_points
{
	void operator()(const mesh& Mesh, k3d::selection::set& Selection) const
	{
assert_not_implemented();
/*
		Selection.points = mesh_selection::component_select_all();
		Selection.edges = mesh_selection::component_deselect_all();
		Selection.faces = mesh_selection::component_deselect_all();
*/
	}
};

struct select_all_split_edges
{
	void operator()(const mesh& Mesh, k3d::selection::set& Selection) const
	{
assert_not_implemented();
/*
		Selection.points = mesh_selection::component_deselect_all();
		Selection.edges = mesh_selection::component_select_all();
		Selection.faces = mesh_selection::component_deselect_all();
*/
	}
};

struct select_all_uniform
{
	void operator()(const mesh& Mesh, k3d::selection::set& Selection) const
	{
assert_not_implemented();
/*
		Selection.points = mesh_selection::component_deselect_all();
		Selection.edges = mesh_selection::component_deselect_all();
		Selection.faces = mesh_selection::component_select_all();
*/
	}
};

/*
void invert(k3d::mesh_selection::records_t& Records)
{
	for(k3d::mesh_selection::records_t::iterator record = Records.begin(); record != Records.end(); ++record)
		record->weight = record->weight ? 0.0 : 1.0;
}
*/

struct invert_points
{
	void operator()(const k3d::mesh& Mesh, k3d::selection::set& Selection) const
	{
assert_not_implemented();
//		invert(Selection.points);
	}
};

struct invert_split_edges
{
	void operator()(const k3d::mesh& Mesh, k3d::selection::set& Selection) const
	{
assert_not_implemented();
//		invert(Selection.edges);
	}
};

struct invert_uniform
{
	void operator()(const k3d::mesh& Mesh, k3d::selection::set& Selection) const
	{
assert_not_implemented();
//		invert(Selection.faces);
	}
};

/// Policy class that updates a mesh_selection to select the given points
struct select_points
{
	select_points(const double_t Weight, const mesh::indices_t& Companions) :
		weight(Weight)
	{
	}

	void operator()(const k3d::selection::record& Record, k3d::selection::set& Selection) const
	{
assert_not_implemented();
/*
		for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
		{
			switch(token->type)
			{
				case selection::POINT:
					Selection.points.push_back(mesh_selection::record(token->id, token->id+1, weight));
					return;
				default:
					break;
			}
		}
*/
	}

	const double_t weight;
};

/// Policy class that updates a mesh_selection to select the given lines
struct select_split_edges
{
	select_split_edges(const double_t Weight, const mesh::indices_t& Companions) :
		weight(Weight), companions(Companions)
	{
	}

	void operator()(const k3d::selection::record& Record, k3d::selection::set& Selection) const
	{
assert_not_implemented();
/*
		for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
		{
			const uint_t edge = token->id;
			switch(token->type)
			{
				case selection::ABSOLUTE_SPLIT_EDGE:
					Selection.edges.push_back(mesh_selection::record(edge, edge+1, weight));
					if(edge < companions.size() && companions[edge] != edge)
					{
						const uint_t companion = companions[edge];
						Selection.edges.push_back(mesh_selection::record(companion, companion+1, weight));
					}
					return;

				case selection::ABSOLUTE_LINEAR_CURVE:
					Selection.linear_curves.push_back(mesh_selection::record(token->id, token->id+1, weight));
					return;

				case selection::ABSOLUTE_CUBIC_CURVE:
					Selection.cubic_curves.push_back(mesh_selection::record(token->id, token->id+1, weight));
					return;
				case selection::ABSOLUTE_NURBS_CURVE:
					Selection.nurbs_curves.push_back(mesh_selection::record(token->id, token->id+1, weight));
					return;

				default:
					break;
			}
		}
*/
	}

	const double_t weight;
	const mesh::indices_t& companions;
};

/// Policy class that updates a mesh_selection to select the given uniform components
struct select_uniform
{
	select_uniform(const double_t Weight, const mesh::indices_t& Companions) :
		weight(Weight)
	{
	}

	void operator()(const k3d::selection::record& Record, k3d::selection::set& Selection) const
	{
assert_not_implemented();
/*
		// Generic geometry selection behavior ...
		mesh_selection::component current_component(0, 0, selection::UNIFORM);
		for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
		{
			switch(token->type)
			{
				case selection::PRIMITIVE:
					if((current_component.primitive_begin != token->id) || (current_component.primitive_end != token->id + 1))
					{
						if(!current_component.empty())
						{
							Selection.add_component(current_component);
							current_component.clear();
						}
						current_component.primitive_begin = token->id;
						current_component.primitive_end = token->id + 1;
						current_component.type = selection::UNIFORM;
					}
					break;
				case selection::UNIFORM:
					current_component.add_range(token->id, token->id+1, weight);
					break;
				default:
					break;
			}
		}
		if(!current_component.empty())
		{
			Selection.add_component(current_component);
		}
*/
	}

	const double_t weight;
};

struct deselect_all
{
	void operator()(const mesh& Mesh, k3d::selection::set& Selection) const
	{
		assert_not_implemented();
		//Selection = mesh_selection::deselect_all();
	}
};

/// Defines a mapping of nodes to selection records
typedef std::multimap<inode*, const k3d::selection::record*> node_selection_map_t;
/// Given a set of selection records, generates a mapping of the corresponding nodes to each record
const node_selection_map_t map_nodes(const k3d::selection::records& Selection)
{
	node_selection_map_t results;

	/** \todo Improve the performance of this loop by cacheing nodes */
	for(k3d::selection::records::const_iterator record = Selection.begin(); record != Selection.end(); ++record)
		results.insert(std::make_pair(k3d::selection::get_node(*record), &*record));

	if(results.count(0))
		log() << warning << "Selection contained records without nodes" << std::endl;
	results.erase(0);

	return results;
}

/// Uses an update policy to convert the supplied selection into updates to MeshInstance mesh selections
template<typename UpdatePolicyT>
void select_components(const k3d::selection::records& Selection, const double_t Weight)
{
assert_not_implemented();
/*
	inode* node = 0;
	const mesh* mesh = 0;
	mesh_selection selection;
	imesh_selection_sink* sink = 0;
	mesh::bools_t boundary_edges;
	mesh::indices_t companions;

	const node_selection_map_t nodes = map_nodes(Selection);
	for(node_selection_map_t::const_iterator n = nodes.begin(); n != nodes.end(); ++n)
	{
		if(n->first != node)
		{
			if(sink && node)
			{
				property::set_internal_value(sink->mesh_selection_sink_input(), selection);
			}

			node = n->first;
			selection = mesh_selection::select_null();
			sink = dynamic_cast<imesh_selection_sink*>(node);
			if(sink)
				selection = boost::any_cast<mesh_selection>(sink->mesh_selection_sink_input().property_internal_value());
				
			companions.clear();
			boundary_edges.clear();
				
			imesh_source* const mesh_source = dynamic_cast<imesh_source*>(node);
			if(mesh_source)
				mesh = property::pipeline_value<mesh*>(mesh_source->mesh_source_output());

			if(mesh && mesh->polyhedra && mesh->polyhedra->edge_points && mesh->polyhedra->clockwise_edges)
			{
				polyhedron::create_edge_adjacency_lookup(*mesh->polyhedra->edge_points, *mesh->polyhedra->clockwise_edges, boundary_edges, companions);
    	}
		}

		if(!sink)
			continue;
		
		UpdatePolicyT policy(Weight, companions);
		policy(*n->second, selection);
	}

	if(sink && node)
	{
		property::set_internal_value(sink->mesh_selection_sink_input(), selection);
	}
*/
}

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
			detail::select_components<detail::select_points>(Selection, 1.0);
			break;
		case SPLIT_EDGES:
			detail::select_components<detail::select_split_edges>(Selection, 1.0);
			break;
		case UNIFORM:
			detail::select_components<detail::select_uniform>(Selection, 1.0);
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
			detail::set_component_selection(selected_nodes(), detail::select_all_points(), true);
			break;
		case SPLIT_EDGES:
			detail::set_component_selection(selected_nodes(), detail::select_all_split_edges(), true);
			break;
		case UNIFORM:
			detail::set_component_selection(selected_nodes(), detail::select_all_uniform(), true);
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
			detail::set_component_selection(internal.document.nodes().collection(), detail::invert_points(), true);
			break;
		case SPLIT_EDGES:
			detail::set_component_selection(internal.document.nodes().collection(), detail::invert_split_edges(), true);
			break;
		case UNIFORM:
			detail::set_component_selection(internal.document.nodes().collection(), detail::invert_uniform(), true);
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
	assert_not_implemented();
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
			detail::set_component_selection(internal.document.nodes().collection(), detail::deselect_all(), true);
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

