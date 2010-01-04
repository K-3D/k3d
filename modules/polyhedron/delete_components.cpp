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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/table.h>
#include <k3dsdk/table_copier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/utility.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// delete_components

class delete_components :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	delete_components(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points)
			return;
		if(!Output.point_selection)
			return;

		// Mark points to be explicitly removed by the user ...
		k3d::mesh::bools_t remove_point(Output.point_selection->begin(), Output.point_selection->end());
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + Output.point_selection->size();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Cache some useful stuff up-front ...
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();

			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + polyhedron->clockwise_edges.size();

			const k3d::uint_t loop_begin = 0;
			const k3d::uint_t loop_end = loop_begin + polyhedron->loop_first_edges.size();

			// Mark edges to be explicitly removed by the user ...
			k3d::mesh::bools_t remove_edge(polyhedron->edge_selections.begin(), polyhedron->edge_selections.end());

			// Mark edges to be implicitly removed because their points are going away ...
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(!remove_point[polyhedron->vertex_points[edge]])
					continue;

				remove_edge[edge] = true;
			}

			// Mark loops to be implicitly removed because they have fewer than three edges remaining ...
			k3d::mesh::bools_t remove_loop(polyhedron->loop_first_edges.size(), false);
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];

				k3d::uint_t remaining_edge_count = 0;
				for(k3d::uint_t edge = first_edge; ;)
				{
					if(!remove_edge[edge])
						++remaining_edge_count;

					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}

				if(remaining_edge_count < 3)
					remove_loop[loop] = true;
			}

			// Mark faces to be explicitly removed by the user ...
			k3d::mesh::bools_t remove_face(polyhedron->face_selections.begin(), polyhedron->face_selections.end());

			// Mark faces to be implicitly removed because their first loop is going away ...
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(remove_face[face])
					continue;

				if(remove_loop[polyhedron->face_first_loops[face]])
					remove_face[face] = true;
			}

			// Mark loops and edges to be implicitly removed because their face is going away ...
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(!remove_face[face])
					continue;

				const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
				const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
				for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
				{
					remove_loop[loop] = true;

					const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
					for(k3d::uint_t edge = first_edge; ;)
					{
						remove_edge[edge] = true;

						edge = polyhedron->clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
				}
			}

			// Compute new first edges for loops ...
			k3d::mesh::indices_t new_first_edge(polyhedron->loop_first_edges.size());
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				if(remove_loop[loop])
					continue;

				const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ;)
				{
					if(!remove_edge[edge])
					{
						new_first_edge[loop] = edge;
						break;
					}

					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}

			// Compute new clockwise_edges ...
			k3d::mesh::indices_t new_clockwise_edge(polyhedron->clockwise_edges.size());
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				if(remove_loop[loop])
					continue;

				const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];

				k3d::mesh::indices_t remaining_edges;
				for(k3d::uint_t edge = first_edge; ;)
				{
					if(!remove_edge[edge])
						remaining_edges.push_back(edge);

					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}

				for(k3d::uint_t e = 0; e != remaining_edges.size(); ++e)
				{
					new_clockwise_edge[remaining_edges[e]] = remaining_edges[(e+1)%remaining_edges.size()];
				}
			}

			// Compute maps and remaining component counts ...
			k3d::mesh::indices_t face_map;
			k3d::mesh::create_index_removal_map(remove_face, face_map);
			const k3d::uint_t remaining_faces = std::count(remove_face.begin(), remove_face.end(), false);

			k3d::mesh::indices_t loop_map;
			k3d::mesh::create_index_removal_map(remove_loop, loop_map);
			const k3d::uint_t remaining_loops = std::count(remove_loop.begin(), remove_loop.end(), false);

			k3d::mesh::indices_t edge_map;
			k3d::mesh::create_index_removal_map(remove_edge, edge_map);
			const k3d::uint_t remaining_edges = std::count(remove_edge.begin(), remove_edge.end(), false);

			// Delete faces, updating loop indices as we go ...
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(remove_face[face])
					continue;

				polyhedron->face_shells[face_map[face]] = polyhedron->face_shells[face];
				polyhedron->face_first_loops[face_map[face]] = loop_map[polyhedron->face_first_loops[face]];
				polyhedron->face_loop_counts[face_map[face]] = polyhedron->face_loop_counts[face];
				polyhedron->face_selections[face_map[face]] = polyhedron->face_selections[face];
				polyhedron->face_materials[face_map[face]] = polyhedron->face_materials[face];
			}
			polyhedron->face_shells.resize(remaining_faces);
			polyhedron->face_first_loops.resize(remaining_faces);
			polyhedron->face_loop_counts.resize(remaining_faces);
			polyhedron->face_selections.resize(remaining_faces);
			polyhedron->face_materials.resize(remaining_faces);

			// Delete face attributes ...
			k3d::table_copier face_attribute_copier(polyhedron->face_attributes);
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(remove_face[face])
					continue;

				face_attribute_copier.copy(face, face_map[face]);
			}
			polyhedron->face_attributes.set_row_count(remaining_faces);

			// Delete loops, updating edge indices as we go ...
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				if(remove_loop[loop])
					continue;

				polyhedron->loop_first_edges[loop_map[loop]] = edge_map[new_first_edge[loop]];
			}
			polyhedron->loop_first_edges.resize(remaining_loops);

			// Delete edges, updating edge indices as we go ...
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(remove_edge[edge])
					continue;

				polyhedron->clockwise_edges[edge_map[edge]] = edge_map[new_clockwise_edge[edge]];
				polyhedron->edge_selections[edge_map[edge]] = polyhedron->edge_selections[edge];
				polyhedron->vertex_points[edge_map[edge]] = polyhedron->vertex_points[edge];
				polyhedron->vertex_selections[edge_map[edge]] = polyhedron->vertex_selections[edge];
			}
			polyhedron->clockwise_edges.resize(remaining_edges);
			polyhedron->edge_selections.resize(remaining_edges);
			polyhedron->vertex_points.resize(remaining_edges);
			polyhedron->vertex_selections.resize(remaining_edges);

			// Delete edge attributes ...
			k3d::table_copier edge_attribute_copier(polyhedron->edge_attributes);
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(remove_edge[edge])
					continue;

				edge_attribute_copier.copy(edge, edge_map[edge]);
			}
			polyhedron->edge_attributes.set_row_count(remaining_edges);

			// Delete vertex attributes ...
			k3d::table_copier vertex_attribute_copier(polyhedron->vertex_attributes);
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(remove_edge[edge])
					continue;

				vertex_attribute_copier.copy(edge, edge_map[edge]);
			}
			polyhedron->vertex_attributes.set_row_count(remaining_edges);
		}

		// Mark points to be implicitly removed because they're no-longer used ...
		k3d::mesh::bools_t unused_point;
		k3d::mesh::lookup_unused_points(Output, unused_point);
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			if(!unused_point[point])
				continue;

			remove_point[point] = true;
		}	

		// Delete points ...
		k3d::mesh::indices_t point_map;
		k3d::mesh::delete_points(Output, remove_point, point_map);
		const k3d::uint_t remaining_points = std::count(remove_point.begin(), remove_point.end(), false);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<delete_components,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x2d738b7a, 0x6f473349, 0x53a066ad, 0xa857f734),
				"Delete",
				"Deletes selected faces, edges and vertices",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// delete_components_factory

k3d::iplugin_factory& delete_components_factory()
{
	return delete_components::get_factory();
}

} // namespace polyhedron

} // namespace module

