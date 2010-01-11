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
	\author Romain Behar <romainbehar@yahoo.com>
	\author Timothy M. Shead <tshead@k-3d.com>
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// connect_vertices

class connect_vertices :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	connect_vertices(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.point_selection)
			return;
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Keep track of newly-created geometry ...
			new_edges_t new_edges;
			new_faces_t new_faces;

			// For each shell ...
			const k3d::uint_t shell_begin = 0;
			const k3d::uint_t shell_end = shell_begin + polyhedron->shell_types.size();
			for(k3d::uint_t shell = shell_begin; shell != shell_end; ++shell)
			{
				// For each face ...
				const k3d::uint_t face_begin = 0;
				const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					if(polyhedron->face_shells[face] != shell)
						continue;

					const edge_groups_t edge_groups = get_edge_groups(point_selection, *polyhedron, face);
					if(4 == edge_groups.size() && edge_groups[0].size() == edge_groups[2].size())
					{
						const k3d::uint_t count = edge_groups[0].size();
						for(k3d::uint_t i = 0; i != count; ++i)
						{
							split_face(*polyhedron, shell, face, edge_groups[0][count - i - 1], edge_groups[2][i], new_faces, new_edges);
						}
					}
				}
			}

			// Select new edges ...
			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + new_edges.size();
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
				polyhedron->edge_selections[new_edges[edge]] = 1;
		}

		// Deselect output points ...
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + point_selection.size();
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
			point_selection[point] = 0;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<connect_vertices,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xbbe1c426, 0x07a54105, 0xbbdb5e49, 0xcaf1825e),
				"ConnectVertices",
				"Creates edges between selected vertices",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef k3d::mesh::indices_t new_faces_t;
	typedef k3d::mesh::indices_t new_edges_t;

	typedef k3d::mesh::indices_t edge_group_t;
	typedef std::vector<edge_group_t> edge_groups_t;

	const edge_groups_t get_edge_groups(const k3d::mesh::selection_t& PointSelection, const k3d::polyhedron::primitive& Polyhedron, const k3d::uint_t Face)
	{
		edge_groups_t edge_groups;

		const k3d::uint_t face_first_edge = Polyhedron.loop_first_edges[Polyhedron.face_first_loops[Face]];
		for(k3d::uint_t edge = face_first_edge; ;)
		{
			if(edge_groups.empty())
			{
				edge_groups.push_back(edge_group_t(1, edge));
			}
			else
			{
				const k3d::bool_t group_selected = PointSelection[Polyhedron.vertex_points[edge_groups.back().back()]];
				const k3d::bool_t edge_selected = PointSelection[Polyhedron.vertex_points[edge]];

				if(group_selected != edge_selected)
				{
					edge_groups.push_back(edge_group_t(1, edge));
				}
				else
				{
					edge_groups.back().push_back(edge);
				}
			}

			edge = Polyhedron.clockwise_edges[edge];
			if(edge == face_first_edge)
				break;
		}

		// If the first and last groups have the same selection-state, they should be one big group ...
		if(edge_groups.size() > 1)
		{
			const k3d::bool_t first_selected = PointSelection[Polyhedron.vertex_points[edge_groups.front().front()]];
			const k3d::bool_t last_selected = PointSelection[Polyhedron.vertex_points[edge_groups.back().back()]];
			if(first_selected == last_selected)
			{
				edge_groups.back().insert(edge_groups.back().end(), edge_groups.front().begin(), edge_groups.front().end());
				edge_groups.erase(edge_groups.begin());
			}
		}

		// If there is more tnan one group, see to it that the first group is selected ...
		if(edge_groups.size() > 1)
		{
			const k3d::bool_t first_selected = PointSelection[Polyhedron.vertex_points[edge_groups.front().front()]];
			if(!first_selected)
			{
				edge_groups.insert(edge_groups.end(), edge_groups.front());
				edge_groups.erase(edge_groups.begin());
			}
		}

		return edge_groups;
	}

	/// Connects two vertices belonging to the same face
	void split_face(k3d::polyhedron::primitive& Polyhedron, const k3d::uint_t Shell, const k3d::uint_t Face, const k3d::uint_t Edge1, const k3d::uint_t Edge2, new_faces_t& NewFaces, new_edges_t& NewEdges)
	{
		return_if_fail(k3d::polyhedron::same_loop(Polyhedron.clockwise_edges, Edge1, Edge2));
		const k3d::uint_t ccw_edge1 = k3d::polyhedron::counterclockwise_edge(Polyhedron.clockwise_edges, Edge1);
		const k3d::uint_t ccw_edge2 = k3d::polyhedron::counterclockwise_edge(Polyhedron.clockwise_edges, Edge2);

		const k3d::uint_t new_edge1 = Polyhedron.clockwise_edges.size();
		Polyhedron.clockwise_edges.push_back(Edge2);
		Polyhedron.edge_selections.push_back(0);
		Polyhedron.vertex_points.push_back(Polyhedron.vertex_points[Edge1]);
		Polyhedron.vertex_selections.push_back(0);

		const k3d::uint_t new_edge2 = Polyhedron.clockwise_edges.size();
		Polyhedron.clockwise_edges.push_back(Edge1);
		Polyhedron.edge_selections.push_back(0);
		Polyhedron.vertex_points.push_back(Polyhedron.vertex_points[Edge2]);
		Polyhedron.vertex_selections.push_back(0);

		Polyhedron.clockwise_edges[ccw_edge1] = new_edge1;
		Polyhedron.clockwise_edges[ccw_edge2] = new_edge2;

		const k3d::uint_t new_face = Polyhedron.face_first_loops.size();

		Polyhedron.face_shells.push_back(Shell);
		Polyhedron.face_first_loops.push_back(Polyhedron.loop_first_edges.size());
		Polyhedron.face_loop_counts.push_back(1);
		Polyhedron.face_selections.push_back(0);
		Polyhedron.face_materials.push_back(Polyhedron.face_materials[Face]);
		Polyhedron.loop_first_edges.push_back(Edge1);
		if(Polyhedron.loop_first_edges[Polyhedron.face_first_loops[Face]] == Edge1)
			Polyhedron.loop_first_edges[Polyhedron.face_first_loops[Face]] = new_edge1;

		NewFaces.push_back(new_face);
		NewEdges.push_back(new_edge1);
		NewEdges.push_back(new_edge2);
	}
};

/////////////////////////////////////////////////////////////////////////////
// connect_vertices_factory

k3d::iplugin_factory& connect_vertices_factory()
{
	return connect_vertices::get_factory();
}

} // namespace polyhedron

} // namespace module

