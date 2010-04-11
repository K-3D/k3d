// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
	\author Bart Janssens <bart.janssens@lid.kviv.be>
	\author Timothy M. Shead <tshead@k-3d.com>
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/table_copier.h>
#include <k3dsdk/utility.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// collapse_points

class collapse_points :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	collapse_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
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

		k3d::mesh::points_t& points = Output.points.writable();
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();

		// Mark points explicitly chosen by the user ...
		k3d::mesh::bools_t remove_points(Output.point_selection->begin(), Output.point_selection->end());
		const k3d::uint_t selected_point_count = std::count(remove_points.begin(), remove_points.end(), true);
		if(selected_point_count < 2)
			return;

		// Compute the average position of the selected points ...
		k3d::point3 average(0, 0, 0);
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + points.size();
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			if(!remove_points[point])
				continue;
			average += k3d::to_vector(points[point]);
		}
		average /= selected_point_count;

		// Create a new point based on the average position ...
		const k3d::uint_t new_point = points.size();
		points.push_back(average);
		point_selection.push_back(1);
		remove_points.push_back(false);

		k3d::mesh::indices_t point_indices;
		k3d::mesh::create_index_list(remove_points, point_indices);
		const k3d::mesh::weights_t point_weights(selected_point_count, 1.0 / selected_point_count);
		k3d::table_copier point_copier(Output.point_attributes);
		point_copier.push_back(selected_point_count, &point_indices[0], &point_weights[0]);

		// Map old point references to the new point ...
		k3d::mesh::indices_t point_map(points.size());
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
			point_map[point] = remove_points[point] ? new_point : point;
		point_map[new_point] = new_point;
		k3d::mesh::remap_points(Output, point_map);

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Delete any edges that have been "collapsed" onto the new point ...
			k3d::mesh::bools_t remove_edges(polyhedron->clockwise_edges.size(), false);
			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + polyhedron->clockwise_edges.size();
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(polyhedron->vertex_points[edge] != new_point)
					continue;
				if(polyhedron->vertex_points[polyhedron->clockwise_edges[edge]] != new_point)
					continue;
				remove_edges[edge] = true;
			}
	
			// Don't explicitly delete any loops ...
			k3d::mesh::bools_t remove_loops(polyhedron->loop_first_edges.size(), false);

			// Don't explicitly delete any faces ...
			k3d::mesh::bools_t remove_faces(polyhedron->face_shells.size(), false);
			
			// Make it happen ...
			k3d::polyhedron::delete_components(Output, *polyhedron, remove_points, remove_edges, remove_loops, remove_faces);
		}

		// Mark points to be implicitly removed because they're no-longer used ...
		k3d::mesh::bools_t unused_point;
		k3d::mesh::lookup_unused_points(Output, unused_point);
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			if(!unused_point[point])
				continue;

			remove_points[point] = true;
		}	

		// Delete points ...
		k3d::mesh::delete_points(Output, remove_points);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<collapse_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x915ba4d4, 0xd4154a12, 0x938bec97, 0x60f819f3),
				"CollapsePoints",
				"Merge selected points into one point, positioned at the average position of the selected points.",
				"Polyhedron",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// collapse_points_factory

k3d::iplugin_factory& collapse_points_factory()
{
	return collapse_points::get_factory();
}

} // namespace polyhedron

} // namespace module

