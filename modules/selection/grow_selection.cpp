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
		\author Romain Behar (romainbehar@yahoo.com)
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

#include <list>
#include <set>

namespace module
{

namespace selection
{

namespace detail
{

// Selects all edges adjacent to the given point
void select_adjacent_edges(k3d::mesh::selection_t& OutputEdgeSelections, const k3d::mesh::indices_t& PointEdges, const k3d::mesh::indices_t& PointFirstEdges, const k3d::mesh::counts_t& PointEdgeCounts, const k3d::uint_t Point, const k3d::double_t EdgeSelection)
{
	const k3d::uint_t first_idx = PointFirstEdges[Point];
	const k3d::uint_t last_idx = first_idx + PointEdgeCounts[Point];
	for(k3d::uint_t i = first_idx; i != last_idx; ++i)
	{
		const k3d::uint_t point_edge = PointEdges[i];
		OutputEdgeSelections[point_edge] = EdgeSelection;
	}
}

void select_adjacent_points(k3d::mesh::selection_t& PointSelections, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::indices_t& EdgePoints, const k3d::mesh::indices_t& PointEdges, const k3d::mesh::indices_t& PointFirstEdges, const k3d::mesh::counts_t& PointEdgeCounts, const k3d::uint_t Point, const k3d::double_t PointSelection)
{
	const k3d::uint_t first_idx = PointFirstEdges[Point];
	const k3d::uint_t last_idx = first_idx + PointEdgeCounts[Point];
	for(k3d::uint_t i = first_idx; i != last_idx; ++i)
	{
		const k3d::uint_t edge = PointEdges[i];
		PointSelections[EdgePoints[ClockwiseEdges[edge]]] = PointSelection;
		PointSelections[EdgePoints[edge]] = PointSelection;
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// grow_selection

class grow_selection :
	public k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > >
{
	typedef k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > > base;

public:
	grow_selection(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh

		for(k3d::uint_t i = 0; i != Output.primitives.size(); ++i)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::validate(Output, Output.primitives[i]));
			if(!output_polyhedron)
				continue;

			const k3d::uint_t edge_count = output_polyhedron->clockwise_edges.size();

			// Get point-to-edge lookup data for outgoing edges
			k3d::mesh::indices_t point_edges_out;
			k3d::mesh::indices_t point_first_edges_out;
			k3d::mesh::indices_t point_edge_counts_out;
			k3d::polyhedron::create_vertex_edge_lookup(output_polyhedron->vertex_points, point_edges_out, point_first_edges_out, point_edge_counts_out);

			// Get point-to-edge lookup data for incoming edges
			k3d::mesh::indices_t vertex_points_in(edge_count);
			for(k3d::uint_t edge = 0; edge != edge_count; ++edge)
			{
				vertex_points_in[edge] = output_polyhedron->vertex_points[output_polyhedron->clockwise_edges[edge]];
			}
			k3d::mesh::indices_t point_edges_in;
			k3d::mesh::indices_t point_first_edges_in;
			k3d::mesh::indices_t point_edge_counts_in;
			k3d::polyhedron::create_vertex_edge_lookup(vertex_points_in, point_edges_in, point_first_edges_in, point_edge_counts_in);

			// Get face-to-edge data
			k3d::mesh::indices_t edge_faces;
			k3d::polyhedron::create_edge_face_lookup(output_polyhedron->face_first_loops, output_polyhedron->face_loop_counts, output_polyhedron->loop_first_edges, output_polyhedron->clockwise_edges, edge_faces);

			// Get edge companions
			k3d::mesh::indices_t adjacent_edges;
			k3d::mesh::bools_t boundary_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(output_polyhedron->vertex_points, output_polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

			// copies, since we're modifying these in the output. They don't exist in Input since the selection was not merged
			const k3d::mesh::selection_t input_point_selections = *Output.point_selection;
			const k3d::mesh::selection_t input_edge_selections = output_polyhedron->edge_selections;
			const k3d::mesh::selection_t input_face_selections = output_polyhedron->face_selections;

			for(k3d::uint_t edge = 0; edge != edge_count; ++edge)
			{
				// Grow edge selections
				const k3d::double_t edge_selection = input_edge_selections[edge];
				if(edge_selection)
				{
					const k3d::uint_t start_point = output_polyhedron->vertex_points[edge];
					const k3d::uint_t end_point = output_polyhedron->vertex_points[output_polyhedron->clockwise_edges[edge]];
					detail::select_adjacent_edges(output_polyhedron->edge_selections, point_edges_out, point_first_edges_out, point_edge_counts_out, start_point, edge_selection);
					detail::select_adjacent_edges(output_polyhedron->edge_selections, point_edges_out, point_first_edges_out, point_edge_counts_out, end_point, edge_selection);
					detail::select_adjacent_edges(output_polyhedron->edge_selections, point_edges_in, point_first_edges_in, point_edge_counts_in, start_point, edge_selection);
					detail::select_adjacent_edges(output_polyhedron->edge_selections, point_edges_in, point_first_edges_in, point_edge_counts_in, end_point, edge_selection);
				}

				// Grow face selections
				if(input_face_selections[edge_faces[edge]])
				{
					output_polyhedron->face_selections[edge_faces[adjacent_edges[edge]]] = input_face_selections[edge_faces[edge]];
				}
			}

			// Grow point selections
			for(k3d::uint_t point = 0; point != input_point_selections.size(); ++point)
			{
				if(input_point_selections[point])
				{
					detail::select_adjacent_points(Output.point_selection.writable(), output_polyhedron->clockwise_edges, output_polyhedron->vertex_points, point_edges_out, point_first_edges_out, point_edge_counts_out, point, input_point_selections[point]);
					detail::select_adjacent_points(Output.point_selection.writable(), output_polyhedron->clockwise_edges, output_polyhedron->vertex_points, point_edges_in, point_first_edges_in, point_edge_counts_in, point, input_point_selections[point]);
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<grow_selection,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xff5b603d, 0x176a4def, 0xa4f195fd, 0x30a7ec15),
				"GrowSelection",
				_("Grows the selection to include adjacent vertices, edges and faces"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// grow_selection_factory

k3d::iplugin_factory& grow_selection_factory()
{
	return grow_selection::get_factory();
}

} // namespace selection

} // namespace module

