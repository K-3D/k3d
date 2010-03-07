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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/bicubic_patch.h>
#include <k3dsdk/bilinear_patch.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/cubic_curve.h>
#include <k3dsdk/data.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ienumeration_property.h>
#include <k3dsdk/imesh_selection_algorithm.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imetadata.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/inode_selection.h>
#include <k3dsdk/linear_curve.h>
#include <k3dsdk/log.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/ngui/selection_tool.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/property.h>
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
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::EDGE, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::FACE, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::PARAMETER, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::PATCH, 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::SURFACE, 0.0);

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
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::PARAMETER, component == k3d::selection::PARAMETER ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::PATCH, component == k3d::selection::PATCH ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::EDGE, component == k3d::selection::EDGE ? 1.0 : 0.0);
		geometry::primitive_selection::append(*primitive_selection, k3d::selection::SURFACE, component == k3d::selection::SURFACE ? 1.0 : 0.0);

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

/// Policy class that updates a mesh_selection to select the given edges
struct select_edges
{
	select_edges(const bool_t SelectAdjacent, const double_t Weight) :
		select_adjacent(SelectAdjacent),
		weight(Weight)
	{
	}

	const k3d::selection::set operator()(k3d::inode* const Node, const k3d::mesh& Mesh, const k3d::selection::set& CurrentSelection, const k3d::selection::records& InteractiveSelection) const
	{
		k3d::selection::set results = CurrentSelection;

		// Extract the set of edges to be selected ...
		mesh::indices_t edges;
		mesh::indices_t edge_primitives;
		for(k3d::selection::records::const_iterator record = InteractiveSelection.begin(); record != InteractiveSelection.end(); ++record)
		{
			if(k3d::selection::get_node(*record) != Node)
				continue;

			for(k3d::selection::record::tokens_t::const_iterator primitive_token = record->tokens.begin(); primitive_token != record->tokens.end(); ++primitive_token)
			{
				if(primitive_token->type != k3d::selection::PRIMITIVE)
					continue;

				for(k3d::selection::record::tokens_t::const_iterator edge_token = primitive_token + 1; edge_token != record->tokens.end(); ++edge_token)
				{
					if(edge_token->type != k3d::selection::EDGE)
						continue;

					edges.push_back(edge_token->id);
					edge_primitives.push_back(primitive_token->id);

					break;	
				}

				break;
			}
		}

		// Get the set of unique primitives ...
		std::set<uint_t> primitives(edge_primitives.begin(), edge_primitives.end());

		// Optionally select adjacent edges ...
		if(select_adjacent)
		{
			const uint_t edge_begin = 0;
			const uint_t edge_end = edge_begin + edges.size();

			for(std::set<uint_t>::const_iterator primitive = primitives.begin(); primitive != primitives.end(); ++primitive)
			{
				if(Mesh.primitives.size() <= *primitive)
					continue;

				boost::scoped_ptr<polyhedron::const_primitive> polyhedron(polyhedron::validate(Mesh, *Mesh.primitives[*primitive]));
				if(!polyhedron)
					continue;

				mesh::bools_t boundary_edges;
				mesh::indices_t adjacent_edges;
				polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

				std::set<uint_t> primitive_edges;
				for(uint_t edge = edge_begin; edge != edge_end; ++edge)
				{
					if(edge_primitives[edge] != *primitive)
						continue;
					primitive_edges.insert(edges[edge]);
				}

				for(std::set<uint_t>::const_iterator edge = primitive_edges.begin(); edge != primitive_edges.end(); ++edge)
				{
					if(boundary_edges[*edge])
						continue;
					if(primitive_edges.count(adjacent_edges[*edge]))
						continue;

					edges.push_back(adjacent_edges[*edge]);
					edge_primitives.push_back(*primitive);
				}
			}
		}

		// Add all our edges to the output selection ...
		if(edges.size())
		{
			const uint_t edge_begin = 0;
			const uint_t edge_end = edge_begin + edges.size();

			boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(results));

			for(std::set<uint_t>::const_iterator primitive = primitives.begin(); primitive != primitives.end(); ++primitive)
			{
				primitive_selection->primitive_begin.push_back(*primitive);
				primitive_selection->primitive_end.push_back(*primitive + 1);
				primitive_selection->primitive_selection_type.push_back(k3d::selection::EDGE);
				primitive_selection->primitive_first_range.push_back(primitive_selection->index_begin.size());
				primitive_selection->primitive_range_count.push_back(0);
				for(uint_t edge = edge_begin; edge != edge_end; ++edge)
				{
					if(edge_primitives[edge] != *primitive)
						continue;
					primitive_selection->primitive_range_count.back() += 1;
					primitive_selection->index_begin.push_back(edges[edge]);
					primitive_selection->index_end.push_back(edges[edge] + 1);
					primitive_selection->weight.push_back(weight);
				}
			}
		}

		return results;
	}

	const bool_t select_adjacent;
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
		case EDGE:
			Stream << "edge";
			break;
		case SURFACE:
			Stream << "surface";
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
	else if(text == "edge")
		RHS = EDGE;
	else if(text == "surface")
		RHS = SURFACE;
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
			const std::vector<inode_selection*> nodes = node::lookup<inode_selection>(document, "ngui:unique_node", "node_selection");
			if(nodes.size() != 1)
				return 0;
			m_node_selection = nodes[0];
			// Make sure the node gets updated whenever the metadata is changed or the node is deleted
			imetadata* metadata = dynamic_cast<imetadata*>(nodes[0]);
			m_node_selection_metadata_connection = metadata->connect_metadata_changed_signal(sigc::mem_fun(*this, &implementation::on_node_selection_node_changed));
			m_node_selection_deleted_connection = dynamic_cast<inode*>(m_node_selection)->deleted_signal().connect(sigc::mem_fun(*this, &implementation::on_node_selection_node_changed));
		}
		return m_node_selection;
	}

	const nodes_t selected_nodes()
	{
		nodes_t result;
		return_val_if_fail(node_selection(), result);
		
		const inode_selection::selected_nodes_t nodes = node_selection()->selected_nodes();
		result.assign(nodes.begin(), nodes.end());
		result.erase(std::remove(result.begin(), result.end(), static_cast<inode*>(0)), result.end());

		return result;
	}

	inode* rubber_band()
	{
		if(!m_rubber_band)
		{
			std::vector<inode*> nodes = node::lookup(document, uuid(0x72d36e00, 0x0a4621f4, 0xfdc8e69d, 0x621eadc8));
			if(nodes.empty())
			{
				nodes.push_back(plugin::create<inode>(uuid(0x72d36e00, 0x0a4621f4, 0xfdc8e69d, 0x621eadc8), document, "RubberBand"));
			}

			nodes.erase(std::remove(nodes.begin(), nodes.end(), static_cast<inode*>(0)), nodes.end());
			if(nodes.empty())
				return 0;

			m_rubber_band = nodes[0];
		}

		return m_rubber_band;
	}

	idocument& document;

	/// Defines storage for the current document-wide selection mode
	/** \todo Restore undo/redo capability */
//	k3d_data(selection::mode, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, no_property, no_serialization) current_mode;
	k3d_data(selection::mode, immutable_name, explicit_change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) current_mode;

	/// Defines storage for the document-wide keep-selection mode
	k3d_data(bool_t, immutable_name, explicit_change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) keep_selection;

	/// Defines storage for the document-wide convert-selection mode
	k3d_data(bool_t, immutable_name, explicit_change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) convert_selection;

	/// Defines storage for the document-wide select-adjacent-edges state
	k3d_data(bool_t, immutable_name, explicit_change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) select_adjacent_edges;

private:
	implementation(idocument& Document) :
		document(Document),
		current_mode(init_name("selection_mode") + init_label(_("Selection Type")) + init_description(_("Sets selection mode (nodes, faces, edges, points, etc)")) + init_value(selection::NODE) + init_values(mode_values())),
		keep_selection(init_name("keep_selection") + init_label(_("Keep Selection")) + init_description(_("Keep the current selection when changing the selection mode.")) + init_value(false)),
		convert_selection(init_name("convert_selection") + init_label(_("Convert Selection")) + init_description(_("Convert the current selection when changing the selection mode.")) + init_value(true)),
		select_adjacent_edges(init_name("select_adjacent_edges") + init_label(_("Select Adjacent Edges")) + init_description(_("When selecting edges, automatically expand the selection to include adjacent edges.")) + init_value(true)),
		m_node_selection(0),
		m_rubber_band(0)
	{
		current_mode.connect_explicit_change_signal(sigc::mem_fun(*this, &implementation::on_selection_mode_changed));
	}

	/// Called by the signal system when the selection mode changes
	void on_selection_mode_changed(k3d::iunknown*)
	{
		const nodes_t nodes = selected_nodes();
		for(nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
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

			k3d::selection::set new_selection;

			if(keep_selection.internal_value())
			{
				new_selection = boost::any_cast<k3d::selection::set>(mesh_selection_sink->mesh_selection_sink_input().property_internal_value());
			}
			else
			{
				boost::scoped_ptr<geometry::point_selection::storage> point_selection(geometry::point_selection::create(new_selection));
				geometry::point_selection::append(*point_selection, 0.0);

				boost::scoped_ptr<geometry::primitive_selection::storage> primitive_selection(geometry::primitive_selection::create(new_selection));
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::CONSTANT, 0.0);
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::CURVE, 0.0);
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::EDGE, 0.0);
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::FACE, 0.0);
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::PARAMETER, 0.0);
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::PATCH, 0.0);
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::SURFACE, 0.0);
				geometry::primitive_selection::append(*primitive_selection, k3d::selection::VERTEX, 0.0);
			}

			if(convert_selection.internal_value())
			{
				switch(current_mode.internal_value())
				{
					case CURVE:
					{
						static imesh_selection_algorithm* conversion = plugin::create<imesh_selection_algorithm>("MakeCurveSelection");
						return_if_fail(conversion);
						k3d::selection::set::append(conversion->create_mesh_selection(*mesh), new_selection);
						break;
					}
					case EDGE:
					{
						static imesh_selection_algorithm* conversion = plugin::create<imesh_selection_algorithm>("MakeEdgeSelection");
						return_if_fail(conversion);
						k3d::selection::set::append(conversion->create_mesh_selection(*mesh), new_selection);
						break;
					}
					case FACE:
					{
						static imesh_selection_algorithm* conversion = plugin::create<imesh_selection_algorithm>("MakeFaceSelection");
						return_if_fail(conversion);
						k3d::selection::set::append(conversion->create_mesh_selection(*mesh), new_selection);
						break;
					}
					case NODE:
					{
						break;
					}
					case PATCH:
					{
						static imesh_selection_algorithm* conversion = plugin::create<imesh_selection_algorithm>("MakePatchSelection");
						return_if_fail(conversion);
						k3d::selection::set::append(conversion->create_mesh_selection(*mesh), new_selection);
						break;
						break;
					}
					case POINT:
					{
						static imesh_selection_algorithm* conversion = plugin::create<imesh_selection_algorithm>("MakePointSelection");
						return_if_fail(conversion);
						k3d::selection::set::append(conversion->create_mesh_selection(*mesh), new_selection);
						break;
					}
					case SURFACE:
					{
						break;
					}
				}
			}

			property::set_internal_value(mesh_selection_sink->mesh_selection_sink_input(), new_selection);
			property::set_internal_value(**node, "show_component_selection", true);
		}
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

	/// Cached pointer to the rubber band highlighting node
	inode* m_rubber_band;
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

bool_t state::keep_selection()
{
	return internal.keep_selection.internal_value();
}

void state::set_keep_selection(const bool_t Keep)
{
	internal.keep_selection.set_value(Keep);
}

sigc::connection state::connect_keep_selection_changed_signal(const sigc::slot<void, ihint*>& Slot)
{
	return internal.keep_selection.changed_signal().connect(Slot);
}

bool_t state::convert_selection()
{
	return internal.convert_selection.internal_value();
}

void state::set_convert_selection(const bool_t Convert)
{
	internal.convert_selection.set_value(Convert);
}

sigc::connection state::connect_convert_selection_changed_signal(const sigc::slot<void, ihint*>& Slot)
{
	return internal.convert_selection.changed_signal().connect(Slot);
}

bool_t state::select_adjacent_edges()
{
	return internal.select_adjacent_edges.internal_value();
}

void state::set_select_adjacent_edges(const bool_t Expand)
{
	internal.select_adjacent_edges.set_value(Expand);
}

sigc::connection state::connect_select_adjacent_edges_changed_signal(const sigc::slot<void, ihint*>& Slot)
{
	return internal.select_adjacent_edges.changed_signal().connect(Slot);
}

const nodes_t state::selected_nodes()
{
	return internal.selected_nodes();
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
		case EDGE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_edges(internal.select_adjacent_edges.internal_value(), 1.0), Selection);
			break;
		case SURFACE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::SURFACE, 1.0), Selection);
			break;
	}
}

void state::select_nodes(const k3d::selection::records& Selection)
{
	for(k3d::selection::records::const_iterator record = Selection.begin(); record != Selection.end(); ++record)
	{
		if(inode* const node = k3d::selection::get_node(*record))
		{
			select(*node);
		}
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
		case EDGE:
			detail::replace_selection(selected_nodes(), detail::select_all_components(k3d::selection::EDGE), true);
			break;
		case SURFACE:
			detail::replace_selection(selected_nodes(), detail::select_all_components(k3d::selection::SURFACE), true);
			break;
	}
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
		case EDGE:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_components(k3d::selection::EDGE), true);
			break;
		case SURFACE:
			detail::replace_selection(internal.document.nodes().collection(), detail::invert_components(k3d::selection::SURFACE), true);
			break;
	}
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

static const bool_t is_curve_selected(const k3d::selection::record& Record)
{
	k3d::selection::id node_id = k3d::selection::null_id();
	k3d::inode* node = 0;
	k3d::selection::id mesh_id = k3d::selection::null_id();
	k3d::mesh* mesh = 0;
	k3d::selection::id primitive_id = k3d::selection::null_id();
	const k3d::mesh::primitive* primitive = 0;

	for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
	{
		switch(token->type)
		{
			case k3d::selection::NODE:
			{
				node_id = token->id;
				node = k3d::selection::get_node(Record);
				return_val_if_fail(node, false);
				break;
			}
			case k3d::selection::MESH:
			{
				mesh_id = token->id;
				mesh = selection::get_mesh(node, mesh_id);
				return_val_if_fail(mesh, false);
				break;
			}
			case k3d::selection::PRIMITIVE:
			{
				primitive_id = token->id;
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive_id < mesh->primitives.size(), false);
				primitive = mesh->primitives[primitive_id].get();
				break;
			}
			case k3d::selection::CURVE:
			{
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive, false);

				const k3d::selection::id curve = token->id;

				boost::scoped_ptr<k3d::linear_curve::const_primitive> linear_curve(k3d::linear_curve::validate(*mesh, *primitive));
				if(linear_curve)
				{
					return_val_if_fail(curve < linear_curve->curve_selections.size(), false);
					return linear_curve->curve_selections[curve];
				}

				boost::scoped_ptr<k3d::cubic_curve::const_primitive> cubic_curve(k3d::cubic_curve::validate(*mesh, *primitive));
				if(cubic_curve)
				{
					return_val_if_fail(curve < cubic_curve->curve_selections.size(), false);
					return cubic_curve->curve_selections[curve];
				}

				boost::scoped_ptr<k3d::nurbs_curve::const_primitive> nurbs_curve(k3d::nurbs_curve::validate(*mesh, *primitive));
				if(nurbs_curve)
				{
					return_val_if_fail(curve < nurbs_curve->curve_selections.size(), false);
					return nurbs_curve->curve_selections[curve];
				}
			}
		}
	}

	return false;
}

static const bool_t is_edge_selected(const k3d::selection::record& Record)
{
	k3d::selection::id node_id = k3d::selection::null_id();
	k3d::inode* node = 0;
	k3d::selection::id mesh_id = k3d::selection::null_id();
	k3d::mesh* mesh = 0;
	k3d::selection::id primitive_id = k3d::selection::null_id();
	const k3d::mesh::primitive* primitive = 0;

	for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
	{
		switch(token->type)
		{
			case k3d::selection::NODE:
			{
				node_id = token->id;
				node = k3d::selection::get_node(Record);
				return_val_if_fail(node, false);
				break;
			}
			case k3d::selection::MESH:
			{
				mesh_id = token->id;
				mesh = selection::get_mesh(node, mesh_id);
				return_val_if_fail(mesh, false);
				break;
			}
			case k3d::selection::PRIMITIVE:
			{
				primitive_id = token->id;
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive_id < mesh->primitives.size(), false);
				primitive = mesh->primitives[primitive_id].get();
				break;
			}
			case k3d::selection::EDGE:
			{
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive, false);
				boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(*mesh, *primitive));
				return_val_if_fail(polyhedron, false);

				const k3d::selection::id edge = token->id;
				return_val_if_fail(edge < polyhedron->clockwise_edges.size(), false);
				return polyhedron->edge_selections[edge];
			}
		}
	}

	return false;
}

static const bool_t is_face_selected(const k3d::selection::record& Record)
{
	k3d::selection::id node_id = k3d::selection::null_id();
	k3d::inode* node = 0;
	k3d::selection::id mesh_id = k3d::selection::null_id();
	k3d::mesh* mesh = 0;
	k3d::selection::id primitive_id = k3d::selection::null_id();
	const k3d::mesh::primitive* primitive = 0;

	for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
	{
		switch(token->type)
		{
			case k3d::selection::NODE:
			{
				node_id = token->id;
				node = k3d::selection::get_node(Record);
				return_val_if_fail(node, false);
				break;
			}
			case k3d::selection::MESH:
			{
				mesh_id = token->id;
				mesh = selection::get_mesh(node, mesh_id);
				return_val_if_fail(mesh, false);
				break;
			}
			case k3d::selection::PRIMITIVE:
			{
				primitive_id = token->id;
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive_id < mesh->primitives.size(), false);
				primitive = mesh->primitives[primitive_id].get();
				break;
			}
			case k3d::selection::FACE:
			{
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive, false);
				boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(*mesh, *primitive));
				return_val_if_fail(polyhedron, false);

				const k3d::selection::id face = token->id;
				return_val_if_fail(face < polyhedron->face_first_loops.size(), false);
				return polyhedron->face_selections[face];
			}
		}
	}

	return false;
}

static const bool_t is_patch_selected(const k3d::selection::record& Record)
{
	k3d::selection::id node_id = k3d::selection::null_id();
	k3d::inode* node = 0;
	k3d::selection::id mesh_id = k3d::selection::null_id();
	k3d::mesh* mesh = 0;
	k3d::selection::id primitive_id = k3d::selection::null_id();
	const k3d::mesh::primitive* primitive = 0;

	for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
	{
		switch(token->type)
		{
			case k3d::selection::NODE:
			{
				node_id = token->id;
				node = k3d::selection::get_node(Record);
				return_val_if_fail(node, false);
				break;
			}
			case k3d::selection::MESH:
			{
				mesh_id = token->id;
				mesh = selection::get_mesh(node, mesh_id);
				return_val_if_fail(mesh, false);
				break;
			}
			case k3d::selection::PRIMITIVE:
			{
				primitive_id = token->id;
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive_id < mesh->primitives.size(), false);
				primitive = mesh->primitives[primitive_id].get();
				break;
			}
			case k3d::selection::PATCH:
			{
				return_val_if_fail(mesh, false);
				return_val_if_fail(primitive, false);

				const k3d::selection::id patch = token->id;

				boost::scoped_ptr<k3d::bilinear_patch::const_primitive> bilinear_patch(k3d::bilinear_patch::validate(*mesh, *primitive));
				if(bilinear_patch)
				{
					return_val_if_fail(patch < bilinear_patch->patch_selections.size(), false);
					return bilinear_patch->patch_selections[patch];
				}

				boost::scoped_ptr<k3d::bicubic_patch::const_primitive> bicubic_patch(k3d::bicubic_patch::validate(*mesh, *primitive));
				if(bicubic_patch)
				{
					return_val_if_fail(patch < bicubic_patch->patch_selections.size(), false);
					return bicubic_patch->patch_selections[patch];
				}

				boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs_patch(k3d::nurbs_patch::validate(*mesh, *primitive));
				if(nurbs_patch)
				{
					return_val_if_fail(patch < nurbs_patch->patch_selections.size(), false);
					return nurbs_patch->patch_selections[patch];
				}
			}
		}
	}

	return false;
}

static const bool_t is_point_selected(const k3d::selection::record& Record)
{
	k3d::selection::id node_id = k3d::selection::null_id();
	k3d::inode* node = 0;
	k3d::selection::id mesh_id = k3d::selection::null_id();
	k3d::mesh* mesh = 0;

	for(k3d::selection::record::tokens_t::const_iterator token = Record.tokens.begin(); token != Record.tokens.end(); ++token)
	{
		switch(token->type)
		{
			case k3d::selection::NODE:
			{
				node_id = token->id;
				node = k3d::selection::get_node(Record);
				return_val_if_fail(node, false);
				break;
			}
			case k3d::selection::MESH:
			{
				mesh_id = token->id;
				mesh = selection::get_mesh(node, mesh_id);
				return_val_if_fail(mesh, false);
				return_val_if_fail(mesh->point_selection, false);
				break;
			}
			case k3d::selection::POINT:
			{
				return_val_if_fail(mesh, false);

				const k3d::selection::id point = token->id;
				return_val_if_fail(point < mesh->point_selection->size(), false);
				return (*mesh->point_selection)[point];
			}
		}
	}

	return false;
}

const bool_t state::is_selected(const k3d::selection::record& Record)
{
	switch(internal.current_mode.internal_value())
	{
		case CURVE:
			return is_curve_selected(Record);
		case EDGE:
			return is_edge_selected(Record);
		case FACE:
			return is_face_selected(Record);
		case NODE:
		{
			k3d::inode* const node = k3d::selection::get_node(Record);
			return node ? is_selected(*node) : false;
		}
		case PATCH:
			return is_patch_selected(Record);
		case POINT:
			return is_point_selected(Record);
		case SURFACE:
			assert_not_implemented();
			return false;
	}

	return false;
}

void state::deselect(inode& Node)
{
	if(internal.node_selection())
		internal.node_selection()->select(Node, 0.0);	
}

void state::deselect(const k3d::selection::record& Selection)
{
	deselect(k3d::selection::records(1, Selection));
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
		case EDGE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_edges(internal.select_adjacent_edges.internal_value(), 0.0), Selection);
			break;
		case SURFACE:
			detail::merge_interactive_selection(selected_nodes(), detail::select_component(k3d::selection::SURFACE, 0.0), Selection);
			break;
	}
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
		case EDGE:
		case SURFACE:
			detail::replace_selection(internal.document.nodes().collection(), detail::deselect_all(), false);
			break;
	}
}

void state::deselect_all_nodes()
{
	if(internal.node_selection())
		internal.node_selection()->deselect_all();
}

inode* state::rubber_band()
{
	return internal.rubber_band();
}

mesh* get_mesh(inode* Node, const k3d::selection::id& MeshID)
{
	return_val_if_fail(Node, 0);
	return_val_if_fail(MeshID == 0, 0); // Should never get a node with more than one mesh!

	imesh_source* const mesh_source = dynamic_cast<imesh_source*>(Node);
	return_val_if_fail(mesh_source, 0);

	return boost::any_cast<k3d::mesh*>(mesh_source->mesh_source_output().property_internal_value());
}

} // namespace selection

} // namespace ngui

} // namespace k3d

