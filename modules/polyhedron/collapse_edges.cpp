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
// collapse_edges

class collapse_edges :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	collapse_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_position(init_owner(*this) + init_name("position") + init_label(_("Position")) + init_description(_("Location of the collapsed point")) + init_value(0.5) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_position.changed_signal().connect(k3d::hint::converter<
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

		k3d::mesh::points_t& points = Output.points.writable();
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();
		k3d::table_copier point_attributes(Output.point_attributes);

		const k3d::double_t position = m_position.pipeline_value();

		// Don't explicitly remove any points ...
		k3d::mesh::bools_t remove_points(points.size(), false);

		// Keep track of a mapping from old points to new points ...
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

			// Compute adjacent edges ...
			k3d::mesh::bools_t boundary_edges;
			k3d::mesh::indices_t adjacent_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

			// Map from selected edges to newly-created points ...
			k3d::mesh::indices_t new_edge_points(polyhedron->clockwise_edges.size());

			// Keep track of edges to be deleted ...
			k3d::mesh::bools_t remove_edges(polyhedron->clockwise_edges.size(), false);

			// For each selected edge ...
			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + polyhedron->clockwise_edges.size();
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(!polyhedron->edge_selections[edge])
					continue;

				const k3d::uint_t start_point_index = polyhedron->vertex_points[edge];
				const k3d::uint_t end_point_index = polyhedron->vertex_points[polyhedron->clockwise_edges[edge]];

				// If this edge has a companion, and the companion has already created its point, use it ...
				if(!boundary_edges[edge] && polyhedron->edge_selections[adjacent_edges[edge]] && adjacent_edges[edge] < edge)
				{
					new_edge_points[edge] = new_edge_points[adjacent_edges[edge]];
				}
				// Otherwise, create a new point for this edge ...
				{
					new_edge_points[edge] = points.size();

					points.push_back(k3d::mix(points[start_point_index], points[end_point_index], position));
					point_selection.push_back(1);
					remove_points.push_back(false);
					point_map.push_back(new_edge_points[edge]);

					k3d::uint_t point_indices[2] = { start_point_index, end_point_index };
					k3d::double_t point_weights[2] = { position, 1.0 - position };

					point_attributes.push_back(2, point_indices, point_weights);
				}

				point_map[start_point_index] = new_edge_points[edge];
				point_map[end_point_index] = new_edge_points[edge];

				remove_edges[edge] = true;
			}

			// Don't explicitly delete any loops ...
			k3d::mesh::bools_t remove_loops(polyhedron->loop_first_edges.size(), false);

			// Don't explicitly delete any faces ...
			k3d::mesh::bools_t remove_faces(polyhedron->face_shells.size(), false);
			
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

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<collapse_edges,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
					k3d::uuid(0xa5252071, 0xa3484315, 0x9c9daf1c, 0x786042a3),
					"CollapseEdges",
					"For each selected edge, collapses its two vertices into one",
					"Mesh",
					k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_position;
};

/////////////////////////////////////////////////////////////////////////////
// factories

k3d::iplugin_factory& collapse_edges_factory()
{
	return collapse_edges::get_factory();
}

} // namespace polyhedron

} // namespace module

