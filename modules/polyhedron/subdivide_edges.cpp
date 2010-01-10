// K-3D
// Copyright (c) 2005-2010 Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// subdivide_edges

class subdivide_edges :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	subdivide_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_vertices(init_owner(*this) + init_name("vertices") + init_label(_("Vertices")) + init_description(_("Number of vertices to insert in each selected edge")) + init_value(1L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1)))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_vertices.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
//		m_edge_list.clear();

		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		const k3d::int32_t vertices = m_vertices.pipeline_value();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			k3d::mesh::points_t& points = Output.points.writable();
			k3d::mesh::selection_t& point_selection = Output.point_selection.writable();

			// Compute adjacent edges ...
			k3d::mesh::bools_t boundary_edges;
			k3d::mesh::indices_t adjacent_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

			// While building new edges, we will map from each edge to any points that are created for it ...
			k3d::mesh::indices_t edge_first_points(polyhedron->clockwise_edges.size());

			// Get ready to copy attributes ...
			k3d::table_copier point_attributes(Output.point_attributes);
			k3d::table_copier edge_attributes(polyhedron->edge_attributes);
			k3d::table_copier vertex_attributes(polyhedron->vertex_attributes);

			// For each edge ...
			const k3d::uint_t begin_edge = 0;
			const k3d::uint_t end_edge = begin_edge + polyhedron->clockwise_edges.size();
			for(k3d::uint_t edge = begin_edge; edge != end_edge; ++edge)
			{
				if(!polyhedron->edge_selections[edge])
					continue;

				// Keep track of the points that will be used, in clockwise order, to create new edges ...
				k3d::mesh::indices_t clockwise_points(vertices);

				// If this edge has a companion, and the companion has already created points, use them ...
				if(!boundary_edges[edge] && polyhedron->edge_selections[adjacent_edges[edge]] && adjacent_edges[edge] < edge)
				{
					edge_first_points[edge] = edge_first_points[adjacent_edges[edge]];

					for(k3d::uint_t vertex = 0; vertex != vertices; ++vertex)
						clockwise_points[vertex] = edge_first_points[edge] + vertex;
					std::reverse(clockwise_points.begin(), clockwise_points.end());
				}
				// Otherwise, create new points for this edge ...
				else
				{
					edge_first_points[edge] = points.size();

					const k3d::point3 start_point = points[polyhedron->vertex_points[edge]];
					const k3d::point3 end_point = points[polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]];

					for(k3d::uint_t vertex = 0; vertex != vertices; ++vertex)
					{
						clockwise_points[vertex] = points.size();

						points.push_back(k3d::mix(start_point, end_point, k3d::ratio(static_cast<k3d::int32_t>(vertex + 1), vertices + 1)));
						point_selection.push_back(1);
						point_attributes.push_back(polyhedron->vertex_points[edge]);
					}
				}

				// Using the new points, create new edges ...
				const k3d::uint_t clockwise_edge = polyhedron->clockwise_edges[edge];
				polyhedron->clockwise_edges[edge] = polyhedron->clockwise_edges.size();
				for(k3d::uint_t vertex = 0; vertex != vertices; ++vertex)
				{
					polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
					polyhedron->edge_selections.push_back(1);
					edge_attributes.push_back(edge);
					polyhedron->vertex_points.push_back(clockwise_points[vertex]);
					polyhedron->vertex_selections.push_back(0);
					vertex_attributes.push_back(edge);
				}
				polyhedron->clockwise_edges.back() = clockwise_edge;
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<subdivide_edges,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x62de51ff, 0xe70f423a, 0x953d899a, 0x1dbf5c38),
				"SubdivideEdges",
				"Subdivides selected edges by adding points.",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_vertices;
};

/////////////////////////////////////////////////////////////////////////////
// subdivide_edges_factory

k3d::iplugin_factory& subdivide_edges_factory()
{
	return subdivide_edges::get_factory();
}

} // namespace polyhedron

} // namespace module

