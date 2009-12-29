// K-3D
// Copyright (c) 2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
	\author Romain Behar <romainbehar@yahoo.com>
	\author Timothy M. Shead <tshead@k-3d.com>
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

#include <vector>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// bridge_edges

class bridge_edges :
	public k3d::mesh_selection_sink<k3d::material_sink<k3d::mesh_modifier<k3d::node > > >
{
	typedef k3d::mesh_selection_sink<k3d::material_sink<k3d::mesh_modifier<k3d::node > > > base;

public:
	bridge_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	struct adjacency_edge
	{
		adjacency_edge(const k3d::uint_t StartPoint, const k3d::uint_t Edge, const k3d::uint_t EndPoint) :
			start_point(StartPoint),
			edge(Edge),
			end_point(EndPoint)
		{
		}

		k3d::uint_t start_point;
		k3d::uint_t edge;
		k3d::uint_t end_point;
	};

	typedef std::vector<adjacency_edge> adjacency_list;

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		k3d::imaterial* const material = m_material.pipeline_value();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Lookup edges on the boundary of the polyhedron ...
			k3d::mesh::bools_t boundary_edges;
			k3d::mesh::indices_t adjacent_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

			// Create a mapping from edges to their adjacent faces ...
			k3d::mesh::indices_t edge_faces;
			k3d::polyhedron::create_edge_face_lookup(polyhedron->face_first_loops, polyhedron->face_loop_counts, polyhedron->loop_first_edges, polyhedron->clockwise_edges, edge_faces);

			// Get the set of selected boundary edges (excluding polygon holes) ...
			k3d::mesh::indices_t edges;

			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
				for(k3d::uint_t edge = first_edge; ; )
				{
					if(boundary_edges[edge] && polyhedron->edge_selections[edge])
						edges.push_back(edge);

					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}

			// Locate the start and end vertices for each edge ...
			adjacency_list ungrouped_edges;
			for(k3d::uint_t i = 0; i != edges.size(); ++i)
			{
				ungrouped_edges.push_back(
					adjacency_edge(
						polyhedron->vertex_points[edges[i]],
						edges[i],
						polyhedron->vertex_points[polyhedron->clockwise_edges[edges[i]]]
						));
			}

			// Partition selected boundary edges into groups of contiguous edges ...
			std::vector<adjacency_list> grouped_edges;

			while(ungrouped_edges.size())
			{
				adjacency_list current;
				current.push_back(ungrouped_edges.back());
				ungrouped_edges.pop_back();

				while(ungrouped_edges.size())
				{
					k3d::uint_t matched = 0;
					for(adjacency_list::iterator edge = ungrouped_edges.begin(); edge != ungrouped_edges.end(); ++edge)
					{
						if(edge->end_point == current.front().start_point)
						{
							current.insert(current.begin(), *edge);
							ungrouped_edges.erase(edge);
							++matched;
							break;
						}
						else if(current.back().end_point == edge->start_point)
						{
							current.insert(current.end(), *edge);
							ungrouped_edges.erase(edge);
							++matched;
							break;
						}
					}

					if(!matched)
						break;
				}

				grouped_edges.push_back(current);
			}

			// We need exactly two sets of edges ...
			if(grouped_edges.size() != 2)
			{
				k3d::log() << warning << "To bridge, two sets of contiguously connected edges must be selected." << std::endl;
				continue;
			}

			// Both groups of edges must be members of the same shell ...
			if(polyhedron->face_shells[edge_faces[grouped_edges.front().front().edge]] != polyhedron->face_shells[edge_faces[grouped_edges.back().front().edge]])
			{
				k3d::log() << warning << "Cannot bridge across polyhedron shells." << std::endl;
				continue;
			}

			// Create a new polygon bridge ...
			polyhedron->face_shells.push_back(polyhedron->face_shells[edge_faces[grouped_edges.front().front().edge]]);
			polyhedron->face_first_loops.push_back(polyhedron->face_loop_counts.size());
			polyhedron->face_loop_counts.push_back(1);
			polyhedron->face_selections.push_back(1);
			polyhedron->face_materials.push_back(material);
			polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

			// Create the first cross edge ...
			polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
			polyhedron->edge_selections.push_back(0);
			polyhedron->vertex_points.push_back(grouped_edges.front().front().start_point);
			polyhedron->vertex_selections.push_back(0);

			// Create the first set of parallel edges ...
			for(adjacency_list::reverse_iterator edge = grouped_edges.back().rbegin(); edge != grouped_edges.back().rend(); ++edge)
			{
				polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
				polyhedron->edge_selections.push_back(0);
				polyhedron->vertex_points.push_back(edge->end_point);
				polyhedron->vertex_selections.push_back(0);
			}

			// Create the second cross edge ...
			polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
			polyhedron->edge_selections.push_back(0);
			polyhedron->vertex_points.push_back(grouped_edges.back().front().start_point);
			polyhedron->vertex_selections.push_back(0);

			// Create the second set of parallel edges ...
			for(adjacency_list::reverse_iterator edge = grouped_edges.front().rbegin(); edge != grouped_edges.front().rend(); ++edge)
			{
				polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
				polyhedron->edge_selections.push_back(0);
				polyhedron->vertex_points.push_back(edge->end_point);
				polyhedron->vertex_selections.push_back(0);
			}

			// Close the edge loop ...
			polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bridge_edges,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xa360bb82, 0x223a46df, 0x91e4b594, 0xf9b6b7a3),
				"BridgeEdges",
				"Creates new polygons bridging two sets of connected border edges",
				"Polygon",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bridge_edges_factory

k3d::iplugin_factory& bridge_edges_factory()
{
	return bridge_edges::get_factory();
}

} // namespace polyhedron

} // namespace module

