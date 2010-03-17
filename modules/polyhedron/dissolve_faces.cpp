// K-3D
// Copyright (c) 2005-2010, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// dissolve_faces

class dissolve_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	dissolve_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	static void expand_face_group(
		const k3d::polyhedron::primitive& Polyhedron,
		const std::vector<k3d::mesh::indices_t>& AdjacencyList,
		const k3d::uint_t Face,
		const k3d::uint_t FaceGroup,
		std::vector<boost::optional<k3d::uint_t> >& FaceGroups
		)
	{
		const k3d::uint_t first_edge = Polyhedron.loop_first_edges[Polyhedron.face_first_loops[Face]];
		for(k3d::uint_t edge = first_edge; ; )
		{
			const k3d::mesh::indices_t& neighbors = AdjacencyList[Polyhedron.vertex_points[edge]];
			for(k3d::uint_t i = 0; i != neighbors.size(); ++i)
			{
				const k3d::uint_t neighbor = neighbors[i];
				if(!FaceGroups[neighbor] && Polyhedron.face_selections[neighbor])
				{
					FaceGroups[neighbor] = FaceGroup;
					expand_face_group(Polyhedron, AdjacencyList, neighbor, FaceGroup, FaceGroups);
				}
			}

			edge = Polyhedron.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
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

	typedef std::vector<adjacency_edge> adjacency_edges;

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points)
			return;
		if(!Output.point_selection)
			return;

		k3d::mesh::points_t& points = Output.points.writable();
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();
		k3d::table_copier point_attributes(Output.point_attributes);

		// Don't explicitly remove any points ...
		k3d::mesh::bools_t remove_points(points.size(), false);

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Keep track of faces to be deleted ...
			k3d::mesh::bools_t remove_faces(polyhedron->face_shells.size(), false);

			// Don't explicitly delete any edges ...
			k3d::mesh::bools_t remove_edges(polyhedron->clockwise_edges.size(), false);

			// Don't explicitly delete any loops ...
			k3d::mesh::bools_t remove_loops(polyhedron->loop_first_edges.size(), false);
			
			// Get ready to copy attributes ...
			k3d::table_copier face_attributes(polyhedron->face_attributes);
			k3d::table_copier edge_attributes(polyhedron->edge_attributes);
			k3d::table_copier vertex_attributes(polyhedron->vertex_attributes);

			// Compute a point-face adjacency list ...
			std::vector<k3d::mesh::indices_t> adjacency_list;
			k3d::polyhedron::create_point_face_lookup(Output, *polyhedron, adjacency_list);

			// Compute edge-neighbor lookups ...
			k3d::mesh::bools_t boundary_edges;
			k3d::mesh::indices_t adjacent_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

			// Compute edge-face lookups ...
			k3d::mesh::indices_t edge_faces;
			k3d::polyhedron::create_edge_face_lookup(*polyhedron, edge_faces);

			// Label groups of selected, adjacent faces ...
			std::vector<boost::optional<k3d::uint_t> > face_groups(polyhedron->face_shells.size());
			k3d::uint_t face_group_count = 0;
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(face_groups[face])
					continue;

				if(polyhedron->face_selections[face])
				{
					face_groups[face] = ++face_group_count;
					expand_face_group(*polyhedron, adjacency_list, face, face_group_count, face_groups);
				}
				else
				{
					face_groups[face] = 0;
				}
			}

			// For each face group ...
			for(k3d::uint_t face_group = 1; face_group <= face_group_count; ++face_group)
			{
				// Keep track of faces in the group ...
				k3d::mesh::indices_t face_indices;

				// Keep track of edges to be retained ...
				adjacency_edges unsorted_edges;

				// For each face in the group ...
				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					if(face_groups[face] != face_group)
						continue;

					face_indices.push_back(face);

					// The original face will be removed ...
					remove_faces[face] = true;

					// Identify edges (if any) that are on the border of the group ...
					const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
					for(k3d::uint_t edge = first_edge; ; )
					{
						if(boundary_edges[edge] || face_groups[edge_faces[adjacent_edges[edge]]] != face_group)
						{
							unsorted_edges.push_back(
								adjacency_edge(
									polyhedron->vertex_points[edge],
									edge,
									polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]
									));
						}

						edge = polyhedron->clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
				}

				// Sort the edges into order so they form a continuous loop around the combined faces ...
				adjacency_edges sorted_edges;
				sorted_edges.push_back(unsorted_edges.back());
				unsorted_edges.pop_back();

				while(unsorted_edges.size())
				{
					k3d::uint_t matched = 0;
					for(adjacency_edges::iterator edge = unsorted_edges.begin(); edge != unsorted_edges.end(); ++edge)
					{
						if(edge->end_point == sorted_edges.front().start_point)
						{
							sorted_edges.insert(sorted_edges.begin(), *edge);
							unsorted_edges.erase(edge);
							++matched;
							break;
						}
						else if(sorted_edges.back().end_point == edge->start_point)
						{
							sorted_edges.insert(sorted_edges.end(), *edge);
							unsorted_edges.erase(edge);
							++matched;
							break;
						}
					}
					if(!matched)
						break;
				}

				// Should never happen ...
				if(face_indices.empty())
					continue;

				// Create a new face ...
				polyhedron->face_shells.push_back(polyhedron->face_shells[face_indices.front()]);
				polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
				polyhedron->face_loop_counts.push_back(1);
				polyhedron->face_selections.push_back(1);
				polyhedron->face_materials.push_back(polyhedron->face_materials[face_indices.front()]);

				k3d::mesh::weights_t face_weights(face_indices.size(), 1.0 / face_indices.size());
				face_attributes.push_back(face_indices.size(), &face_indices[0], &face_weights[0]);
				remove_faces.push_back(false);

				polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());
				remove_loops.push_back(false);

				for(k3d::uint_t i = 0; i != sorted_edges.size(); ++i)
				{
					polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
					polyhedron->edge_selections.push_back(0);
					polyhedron->vertex_points.push_back(sorted_edges[i].start_point);
					polyhedron->vertex_selections.push_back(0);

					edge_attributes.push_back(sorted_edges[i].edge);
					vertex_attributes.push_back(sorted_edges[i].edge);
					remove_edges.push_back(false);
				}
				polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();
			}

			// Make it happen ...
			k3d::polyhedron::delete_components(Output, *polyhedron, remove_points, remove_edges, remove_loops, remove_faces);
		}

		// Remove unused points ...
		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<dissolve_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x964fdfc2, 0x464d30e6, 0x4585c999, 0x4fea50cf),
				"DissolveFaces",
				"Merges adjacent faces into a single large face.",
				"Polyhedron",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// factories

k3d::iplugin_factory& dissolve_faces_factory()
{
	return dissolve_faces::get_factory();
}

} // namespace polyhedron

} // namespace module

