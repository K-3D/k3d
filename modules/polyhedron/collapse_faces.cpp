// K-3D
// Copyright (c) 2005-2006, Romain Behar
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
	\author Romain Behar (romainbehar@yahoo.com)
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
// collapse_faces

class collapse_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	collapse_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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

		// Create a mapping from old points to new points ...
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + points.size();
		k3d::mesh::indices_t point_map(points.size());
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
			point_map[point] = point;

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Keep track of faces to be deleted ...
			k3d::mesh::bools_t remove_faces(polyhedron->face_shells.size(), false);
			
			// Compute a point-face adjacency list ...
			std::vector<k3d::mesh::indices_t> adjacency_list;
			k3d::polyhedron::create_point_face_lookup(Output, *polyhedron, adjacency_list);

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
				// Prepare to create a new point, based on an average of all the points in the group ...
				const k3d::uint_t new_point = points.size();

				k3d::mesh::indices_t point_indices;
				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					if(face_groups[face] != face_group)
						continue;

					const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
					for(k3d::uint_t edge = first_edge; ; )
					{
						point_indices.push_back(polyhedron->vertex_points[edge]);
						point_map[polyhedron->vertex_points[edge]] = new_point;

						edge = polyhedron->clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}

					remove_faces[face] = true;
				}
				k3d::mesh::weights_t point_weights(point_indices.size(), 1.0 / point_indices.size());

				// Compute the new point position ...
				k3d::point3 average(0, 0, 0);
				for(k3d::uint_t i = 0; i != point_indices.size(); ++i)
					average += k3d::to_vector(point_weights[i] * points[point_indices[i]]);

				// Create the new point ...
				points.push_back(average);
				point_selection.push_back(1);
				remove_points.push_back(false);
				point_attributes.push_back(point_indices.size(), &point_indices[0], &point_weights[0]);
			}

			// Implicitly delete edges that have been collapsed onto a single point ...
			k3d::mesh::bools_t remove_edges(polyhedron->clockwise_edges.size(), false);
			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + polyhedron->clockwise_edges.size();
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(point_map[polyhedron->vertex_points[edge]] != point_map[polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]])
					continue;
				remove_edges[edge] = true;
			}

			// Don't explicitly delete any loops ...
			k3d::mesh::bools_t remove_loops(polyhedron->loop_first_edges.size(), false);

			// Make it happen ...
			k3d::polyhedron::delete_components(Output, *polyhedron, remove_points, remove_edges, remove_loops, remove_faces);
		}

		// Map old points to new points ...
		k3d::mesh::remap_points(Output, point_map);

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
		static k3d::document_plugin_factory<collapse_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x13f8a224, 0x5e284ac4, 0xb5b58ebd, 0x10f65bb6),
				"CollapseFaces",
				"For each selected face, collapses its vertices to the centroid",
				"Polyhedron",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// factories

k3d::iplugin_factory& collapse_faces_factory()
{
	return collapse_faces::get_factory();
}

} // namespace polyhedron

} // namespace module

