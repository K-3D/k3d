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
#include <k3dsdk/material_sink.h>
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
// bridge_faces

class bridge_faces :
	public k3d::mesh_selection_sink<k3d::material_sink<k3d::mesh_modifier<k3d::node > > >
{
	typedef k3d::mesh_selection_sink<k3d::material_sink<k3d::mesh_modifier<k3d::node > > > base;

public:
	bridge_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points)
			return;
		if(!Output.point_selection)
			return;

		const k3d::mesh::points_t& points = *Output.points;

		k3d::imaterial* const material = m_material.pipeline_value();

		// Don't explicitly remove any points ...
		k3d::mesh::bools_t remove_points(points.size(), false);

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Get the set of selected faces ...
			k3d::mesh::indices_t selected_faces;
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(polyhedron->face_selections[face])
					selected_faces.push_back(face);
			}

			// We need exactly two faces ...
			if(selected_faces.size() != 2)
			{
				k3d::log() << warning << "To bridge, exactly two faces must be selected." << std::endl;
				continue;
			}
			
			// Both faces must be members of the same shell ...
			if(polyhedron->face_shells[selected_faces[0]] != polyhedron->face_shells[selected_faces[1]])
			{
				k3d::log() << warning << "Cannot bridge across polyhedron shells." << std::endl;
				continue;
			}

			// Lookup loop lengths ...
			k3d::mesh::counts_t loop_edge_counts;
			k3d::polyhedron::create_loop_edge_count_lookup(*polyhedron, loop_edge_counts);

			// Both faces must have the same number of edges ...
			if(loop_edge_counts[polyhedron->face_first_loops[selected_faces[0]]] != loop_edge_counts[polyhedron->face_first_loops[selected_faces[1]]])
			{
				k3d::log() << warning << "Cannot bridge faces with different edge counts." << std::endl;
				continue;
			}

			// We will explicitly delete the originally selected faces ...
			k3d::mesh::bools_t remove_faces(polyhedron->face_selections.begin(), polyhedron->face_selections.end());

			// We will need to traverse edges in counterclockwise order for the second face ...
			k3d::mesh::indices_t counterclockwise_edges;
			k3d::polyhedron::create_counterclockwise_edge_lookup(*polyhedron, counterclockwise_edges);

			// Find the best starting-vertex for the second face ...
			const k3d::uint_t first_edge_1 = polyhedron->loop_first_edges[polyhedron->face_first_loops[selected_faces[0]]];

			k3d::uint_t first_edge_2 = polyhedron->loop_first_edges[polyhedron->face_first_loops[selected_faces[1]]];
			k3d::double_t distance = std::numeric_limits<double>::max();
			for(k3d::uint_t edge = first_edge_2; ;)
			{
				const k3d::double_t d = k3d::length(points[polyhedron->vertex_points[first_edge_1]] - points[polyhedron->vertex_points[edge]]);
				if(d < distance)
				{
					distance = d;
					first_edge_2 = edge;
				}

				edge = polyhedron->clockwise_edges[edge];
				if(edge == first_edge_2)
					break;
			}

			// Get ready to copy attributes ...
			k3d::table_copier face_attributes(polyhedron->face_attributes);
			k3d::table_copier edge_attributes(polyhedron->edge_attributes);
			k3d::table_copier vertex_attributes(polyhedron->vertex_attributes);

			// For each edge in the first face ...
			for(k3d::uint_t edge1 = first_edge_1, edge2 = first_edge_2; ;)
			{
				// Create a new bridge face ...
				polyhedron->face_shells.push_back(polyhedron->face_shells[selected_faces[0]]);
				polyhedron->face_first_loops.push_back(polyhedron->face_loop_counts.size());
				polyhedron->face_loop_counts.push_back(1);
				polyhedron->face_selections.push_back(1);
				polyhedron->face_materials.push_back(material);
				polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

				k3d::double_t face_weights[2] = { 0.5, 0.5 };
				face_attributes.push_back(2, &selected_faces[0], face_weights);

				remove_faces.push_back(false);

				polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
				polyhedron->edge_selections.push_back(0);
				polyhedron->vertex_points.push_back(polyhedron->vertex_points[edge1]);
				polyhedron->vertex_selections.push_back(0);

				edge_attributes.push_back(edge1);
				vertex_attributes.push_back(edge1);

				polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
				polyhedron->edge_selections.push_back(0);
				polyhedron->vertex_points.push_back(polyhedron->vertex_points[polyhedron->clockwise_edges[edge1]]);
				polyhedron->vertex_selections.push_back(0);

				edge_attributes.push_back(polyhedron->clockwise_edges[edge1]);
				vertex_attributes.push_back(polyhedron->clockwise_edges[edge1]);

				polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
				polyhedron->edge_selections.push_back(0);
				polyhedron->vertex_points.push_back(polyhedron->vertex_points[counterclockwise_edges[edge2]]);
				polyhedron->vertex_selections.push_back(0);

				edge_attributes.push_back(counterclockwise_edges[edge2]);
				vertex_attributes.push_back(counterclockwise_edges[edge2]);

				polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
				polyhedron->edge_selections.push_back(0);
				polyhedron->vertex_points.push_back(polyhedron->vertex_points[edge2]);
				polyhedron->vertex_selections.push_back(0);

				edge_attributes.push_back(edge2);
				vertex_attributes.push_back(edge2);

				// Close the edge loop ...
				polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();

				edge1 = polyhedron->clockwise_edges[edge1];
				if(edge1 == first_edge_1)
					break;

				edge2 = counterclockwise_edges[edge2];
			}

			// Don't explicitly delete any edges ...
			k3d::mesh::bools_t remove_edges(polyhedron->clockwise_edges.size(), false);

			// Don't explicitly delete any loops ...
			k3d::mesh::bools_t remove_loops(polyhedron->loop_first_edges.size(), false);
			
			// Make it happen ...
			k3d::polyhedron::delete_components(Output, *polyhedron, remove_points, remove_edges, remove_loops, remove_faces);
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bridge_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x8e744389, 0x5d284d38, 0x95bb89cc, 0x1b9e528e),
				"BridgeFaces",
				"Creates new polygons forming a bridge between two selected polygons",
				"Polyhedron",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bridge_faces_factory

k3d::iplugin_factory& bridge_faces_factory()
{
	return bridge_faces::get_factory();
}

} // namespace polyhedron

} // namespace module

