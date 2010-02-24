// K-3D
// Copyright (c) 2005-2008 Timothy M. Shead
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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace selection
{

namespace detail
{

void select_connected_faces(k3d::mesh::bools_t& TaggedEdges, k3d::mesh::selection_t& FaceSelections, k3d::mesh::indices_t& EdgeFaces, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::indices_t& AdjacentEdges, const k3d::uint_t Edge)
{
	if(TaggedEdges[Edge])
		return;

	TaggedEdges[Edge] = true;
	FaceSelections[EdgeFaces[Edge]] = 1.0;
	select_connected_faces(TaggedEdges, FaceSelections, EdgeFaces, ClockwiseEdges, AdjacentEdges, AdjacentEdges[Edge]);
	select_connected_faces(TaggedEdges, FaceSelections, EdgeFaces, ClockwiseEdges, AdjacentEdges, ClockwiseEdges[Edge]);
}

void select_connected_edges(k3d::mesh::bools_t& TaggedEdges, k3d::mesh::selection_t& EdgeSelections, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::indices_t& AdjacentEdges, const k3d::uint_t Edge)
{
	if(TaggedEdges[Edge])
		return;

	TaggedEdges[Edge] = true;
	EdgeSelections[Edge] = 1.0;
	select_connected_edges(TaggedEdges, EdgeSelections, ClockwiseEdges, AdjacentEdges, AdjacentEdges[Edge]);
	select_connected_edges(TaggedEdges, EdgeSelections, ClockwiseEdges, AdjacentEdges, ClockwiseEdges[Edge]);
}

void select_connected_points(k3d::mesh::bools_t& TaggedEdges, k3d::mesh::selection_t& PointSelections, k3d::mesh::indices_t& EdgePoints, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::indices_t& AdjacentEdges, const k3d::uint_t Edge)
{
	if(TaggedEdges[Edge])
		return;

	TaggedEdges[Edge] = true;
	PointSelections[EdgePoints[Edge]] = 1.0;
	select_connected_faces(TaggedEdges, PointSelections, EdgePoints, ClockwiseEdges, AdjacentEdges, AdjacentEdges[Edge]);
	select_connected_faces(TaggedEdges, PointSelections, EdgePoints, ClockwiseEdges, AdjacentEdges, ClockwiseEdges[Edge]);
}

void select_connected_components(k3d::polyhedron::primitive& Polyhedron, k3d::mesh::selection_t& PointSelection)
{
	k3d::mesh::bools_t boundary_edges;
	k3d::mesh::indices_t adjacent_edges;
	k3d::polyhedron::create_edge_adjacency_lookup(Polyhedron.vertex_points, Polyhedron.clockwise_edges, boundary_edges, adjacent_edges);

	const k3d::uint_t face_count = Polyhedron.face_first_loops.size();
	const k3d::uint_t edge_count = Polyhedron.clockwise_edges.size();

	// Find out what edges are directly connected to a selected component
	k3d::mesh::indices_t face_selected_edges;
	k3d::mesh::indices_t edge_selected_edges;
	k3d::mesh::indices_t point_selected_edges;
	k3d::mesh::indices_t edge_faces(edge_count);
	for(k3d::uint_t face = 0; face != face_count; ++face)
	{
		const k3d::uint_t loop_begin = Polyhedron.face_first_loops[face];
		const k3d::uint_t loop_end = loop_begin + Polyhedron.face_loop_counts[face];
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const k3d::uint_t first_edge = Polyhedron.loop_first_edges[loop];
			for(k3d::uint_t edge = first_edge; ;)
			{
				if(Polyhedron.face_selections[face])
					face_selected_edges.push_back(edge);
				if(PointSelection[Polyhedron.vertex_points[edge]])
					point_selected_edges.push_back(edge);
				if(Polyhedron.edge_selections[edge])
					edge_selected_edges.push_back(edge);

				edge_faces[edge] = face; // There is an SDK function for this, but we're here now, so...

				edge = Polyhedron.clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}

	//  For all of these edges, select all of the components of the same type that are connected
	k3d::mesh::bools_t tagged_edges(edge_count, false);
	for(k3d::uint_t i = 0; i != face_selected_edges.size(); ++i)
		select_connected_faces(tagged_edges, Polyhedron.face_selections, edge_faces, Polyhedron.clockwise_edges, adjacent_edges, face_selected_edges[i]);
	tagged_edges.assign(edge_count, false);
	for(k3d::uint_t i = 0; i != edge_selected_edges.size(); ++i)
		select_connected_edges(tagged_edges, Polyhedron.edge_selections, Polyhedron.clockwise_edges, adjacent_edges, edge_selected_edges[i]);
	tagged_edges.assign(edge_count, false);
	for(k3d::uint_t i = 0; i != point_selected_edges.size(); ++i)
		select_connected_points(tagged_edges, PointSelection, Polyhedron.vertex_points, Polyhedron.clockwise_edges, adjacent_edges, point_selected_edges[i]);
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// select_connected_components

class select_connected_components :
	public k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > >
{
	typedef k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > > base;

public:
	select_connected_components(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// Shallow copy of the input (no data is copied, only shared pointers are)
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh

		for(k3d::uint_t i = 0; i != Input.primitives.size(); ++i)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::validate(Output, Output.primitives[i]));
			if(!output_polyhedron)
				continue;

			detail::select_connected_components(*output_polyhedron, Output.point_selection.writable());
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_connected_components,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xe54a08e3, 0x014445d3, 0x5395c8a6, 0xb3bfaae3),
				"SelectConnectedComponents",
				_("Select all components that are directly or indirectly connected to the selected component"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// select_connected_components_factory

k3d::iplugin_factory& select_connected_components_factory()
{
	return select_connected_components::get_factory();
}

} // namespace selection

} // namespace module

