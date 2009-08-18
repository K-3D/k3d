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
#include <k3dsdk/metadata_keys.h>
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

		const k3d::selection::set current_selection =
			boost::any_cast<k3d::selection::set>(mesh_selection_sink->mesh_selection_sink_input().property_internal_value());

		property::set_internal_value(mesh_selection_sink->mesh_selection_sink_input(), UpdatePolicy(*mesh, current_selection));
		property::set_internal_value(**node, "show_component_selection", VisibleSelection);
	}
}

/// Update policy for use with replace_selection() that selects all points in a mesh.
struct select_all_points
{
	const k3d::selection::set operator()(const mesh& Mesh, const k3d::selection::set& CurrentSelection) const
	{
		k3d::selection::set results;

		boost::scoped_ptr<geometry::point_selection::storage> point_selection(geometry::point_selection::create(results));
		geometry::point_selection::append(*point_selection, 1.0);

		boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(results));
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::CONSTANT, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::CURVE, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::FACE, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::PATCH, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::SPLIT_EDGE, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::UNIFORM, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::VARYING, 0.0);

		return results;
	}
};

/// Update policy for use with replace_selection() that selects all components of a given type in a mesh.
struct select_all_components
{
	select_all_components(const k3d::selection::type Component) :
		component(Component)
	{
	}

	const k3d::selection::set operator()(const mesh& Mesh, const k3d::selection::set& CurrentSelection) const
	{
		k3d::selection::set results;

		boost::scoped_ptr<geometry::point_selection::storage> point_selection(geometry::point_selection::create(results));
		geometry::point_selection::append(*point_selection, 0.0);

		boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(results));
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::CONSTANT, component == k3d::selection::CONSTANT ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::CURVE, component == k3d::selection::CURVE ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::FACE, component == k3d::selection::FACE ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::PATCH, component == k3d::selection::PATCH ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::SPLIT_EDGE, component == k3d::selection::SPLIT_EDGE ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::UNIFORM, component == k3d::selection::UNIFORM ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::VARYING, component == k3d::selection::VARYING ? 1.0 : 0.0);

		return results;
	}

	const k3d::selection::type component;
};

/// Update policy for use with replace_selection() that inverts the selection of all points in a mesh.
struct invert_points
{
	const k3d::selection::set operator()(const k3d::mesh& Mesh, const k3d::selection::set& CurrentSelection) const
	{
		const k3d::mesh::selection_t* current_point_selection = Mesh.point_selection.get();
		if(!current_point_selection)
			return CurrentSelection;

		k3d::selection::set results = CurrentSelection;

		boost::scoped_ptr<geometry::point_selection::storage> point_selection(geometry::point_selection::create(results));
		for(k3d::uint_t i = 0; i != current_point_selection->size(); ++i)
		{
			point_selection->index_begin.push_back(i);
			point_selection->index_end.push_back(i + 1);
			point_selection->weight.push_back((*current_point_selection)[i] ? 0.0 : 1.0);
		}

		return results;
	}
};

/// Update policy for use with replace_selection() that inverts the selection of a specific component type in a mesh.
struct invert_components
{
	invert_components(const k3d::selection::type Component) :
		component(Component)
	{
	}

	struct generate_selection
	{
		generate_selection(const uint_t Primitive, const k3d::selection::type Component, k3d::selection::set& Results) :
			primitive(Primitive),
			component(Component),
			component_string(string_cast(Component)),
			results(Results)
		{
		}

		void operator()(const string_t& StructureName, const table& Structure, const string_t& ArrayName, const pipeline_data<array>& Array)
		{
			if(StructureName != component_string)
				return;

			if(Array->get_metadata_value(metadata::key::role()) != metadata::value::selection_role())
				return;

			const mesh::selection_t* const array = dynamic_cast<const mesh::selection_t*>(Array.get());
			if(!array)
			{
				log() << error << "unexpected type for array [" << ArrayName << "]" << std::endl;
				return;
			}

			boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(results));
			primitive_selection->primitive_begin.push_back(primitive);
			primitive_selection->primitive_end.push_back(primitive + 1);
			primitive_selection->primitive_selection_type.push_back(component);
			primitive_selection->primitive_first_range.push_back(0);
			primitive_selection->primitive_range_count.push_back(array->size());

			for(uint_t i = 0; i != array->size(); ++i)
			{
				primitive_selection->index_begin.push_back(i);
				primitive_selection->index_end.push_back(i+1);
				primitive_selection->weight.push_back((*array)[i] ? 0.0 : 1.0);
			}
		}

		const uint_t primitive;
		const k3d::selection::type component;
		const string_t component_string;
		k3d::selection::set& results;
	};
	
	const k3d::selection::set operator()(const k3d::mesh& Mesh, const k3d::selection::set& CurrentSelection) const
	{
		k3d::selection::set results = CurrentSelection;

		for(k3d::uint_t primitive = 0; primitive != Mesh.primitives.size(); ++primitive)
		{
			k3d::mesh::visit_arrays(*Mesh.primitives[primitive], generate_selection(primitive, component, results));
		}

		return results;
	}

	const k3d::selection::type component;
};

/// Update policy for use with replace_selection() that deselects all points and primitives in a mesh.
struct deselect_all
{
	const k3d::selection::set operator()(const mesh& Mesh, const k3d::selection::set& CurrentSelection) const
	{
		return geometry::selection::create(0);
	}
};

/// Uses an update policy to convert the supplied selection into updates to MeshInstance mesh selections
template<typename UpdatePolicyT>
void merge_interactive_selection(const nodes_t& Nodes, const UpdatePolicyT& UpdatePolicy, const k3d::selection::records& InteractiveSelection)
{
std::copy(InteractiveSelection.begin(), InteractiveSelection.end(), std::ostream_iterator<k3d::selection::record>(k3d::log(), "\n"));

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

/// Policy class that updates a mesh_selection to select the given components
struct select_component
{
	select_component(const k3d::selection::type& Component, const double_t Weight) :
		component(Component),
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
				if(token->type == k3d::selection::PRIMITIVE)
				{
					found_primitive = true;
					if(primitive_selection->primitive_begin.empty() || primitive_selection->primitive_begin.back() != token->id)
					{
						primitive_selection->primitive_begin.push_back(token->id);
						primitive_selection->primitive_end.push_back(token->id + 1);
						primitive_selection->primitive_selection_type.push_back(component);
						primitive_selection->primitive_first_range.push_back(primitive_selection->index_begin.size());
						primitive_selection->primitive_range_count.push_back(0);
					}
				}
				else if(token->type == component)
				{
					if(!found_primitive)
					{
						log() << error << "Selection records missing primitive token" << std::endl;
						break;
					}
					primitive_selection->primitive_range_count.back() += 1;
					primitive_selection->index_begin.push_back(token->id);
					primitive_selection->index_end.push_back(token->id + 1);
					primitive_selection->weight.push_back(weight);
				}
			}
		}

		return results;
	}

	const k3d::selection::type component;
	const double_t weight;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// mode

std::ostream& operator<<(std::ostream& Stream, const mode& RHS)
{
        switch(RHS)
	{
		case CURVE:
			Stream << "curve";
			break;
		case FACE:
			Stream << "face";
			break;
		case NODE:
			Stream << "node";
			break;
		case PATCH:
			Stream << "patch";
			break;
		case POINT:
			Stream << "point";
			break;
		case SPLIT_EDGE:
			Stream << "split_edge";
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

        if(text == "curve")
                RHS = CURVE;
        else if(text == "face")
                RHS = FACE;
        else if(text == "node")
                RHS = NODE;
        else if(text == "patch")
                RHS = PATCH;
        else if(text == "point")
                RHS = POINT;
        else if(text == "split_edge")
                RHS = SPLIT_EDGE;
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
		values.push_back(ienumeration_property::enumeration_value_t(_("Curves"), "curve", _("Select Curves")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Faces"), "face", _("Select Faces")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Nodes"), "node", _("Select Nodes")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Patches"), "patch", _("Select Patches")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Points"), "point", _("Select Points")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Split Edges"), "split_edge", _("Select Split Edges")));
		values.push_back(ienumeration_property::enumeration_value_t(_("Uniform"), "uniform", _("Select Uniform")));
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
		current_mode(init_name("selection_mode") + init_label(_("Selection Type")) + init_description(_("Sets selection mode (nodes, faces, edges, points, etc)")) + init_value(selection::NODE) + init_values(mode_values())),
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

const nodes_t state::selected_nodes()
{
	return_val_if_fail(internal.node_selection(), nodes_t());
	const inode_selection::selected_nodes_t nodes = internal.node_selection()->selected_nodes();
	return nodes_t(nodes.begin(), nodes.end());
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
		case CURVE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::CURVE, 1.0), Selection);
			break;
		case FACE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::FACE, 1.0), Selection);
			break;
		case NODE:
			select_nodes(Selection);
			break;
		case PATCH:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::PATCH, 1.0), Selection);
			break;
		case POINT:
			detail::merge_interactive_selection(selected_nodes(), detail::select_points(1.0), Selection);
			break;
		case SPLIT_EDGE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::SPLIT_EDGE, 1.0), Selection);
			break;
		case UNIFORM:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::UNIFORM, 1.0), Selection);
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

void state::select_nodes(const nodes_t& Selection)
{
	for(nodes_t::const_iterator node = Selection.begin(); node != Selection.end(); ++node)
		select(**node);
}

void state::select_all()
{
	switch(internal.current_mode.internal_value())
	{
		case CURVE:
			detail::replace_selection(selected_nodes(), detail::select_all_components(k3d::selection::CURVE), true);
			break;
		case FACE:
			detail::replace_selection(selected_nodes(), detail::select_all_components(k3d::selection::FACE), true);
			break;
		case NODE:
			select_all_nodes();
			break;
		case PATCH:
			detail::replace_selection(selected_nodes(), detail::select_all_components(k3d::selection::PATCH), true);
			break;
		case POINT:
			detail::replace_selection(selected_nodes(), detail::select_all_points(), true);
			break;
		case SPLIT_EDGE:
			detail::replace_selection(selected_nodes(), detail::select_all_components(k3d::selection::SPLIT_EDGE), true);
			break;
		case UNIFORM:
			detail::replace_selection(selected_nodes(), detail::select_all_components(k3d::selection::UNIFORM), true);
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
		case CURVE:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_components(k3d::selection::CURVE), true);
			break;
		case FACE:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_components(k3d::selection::FACE), true);
			break;
		case NODE:
			invert_all_nodes();
			break;
		case PATCH:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_components(k3d::selection::PATCH), true);
			break;
		case POINT:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_points(), true);
			break;
		case SPLIT_EDGE:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_components(k3d::selection::SPLIT_EDGE), true);
			break;
		case UNIFORM:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_components(k3d::selection::UNIFORM), true);
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
		case CURVE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::CURVE, 0.0), Selection);
			break;
		case FACE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::FACE, 0.0), Selection);
			break;
		case NODE:
			deselect_nodes(Selection);
			break;
		case PATCH:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::PATCH, 0.0), Selection);
			break;
		case POINT:
			detail::merge_interactive_selection(selected_nodes(), detail::select_points(0.0), Selection);
			break;
		case SPLIT_EDGE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::SPLIT_EDGE, 0.0), Selection);
			break;
		case UNIFORM:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::UNIFORM, 0.0), Selection);
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

void state::deselect_nodes(const nodes_t& Selection)
{
	for(nodes_t::const_iterator node = Selection.begin(); node != Selection.end(); ++node)
		deselect(**node);
}

void state::deselect_all()
{
	switch(internal.current_mode.internal_value())
	{
		case NODE:
			deselect_all_nodes();
			break;
		case CURVE:
		case FACE:
		case PATCH:
		case POINT:
		case SPLIT_EDGE:
		case UNIFORM:
			detail::replace_selection(internal.document.nodes().collection(), detail::deselect_all(), false);
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

