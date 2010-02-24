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
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/table_copier.h>

#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// bevel_points

class bevel_points :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::material_sink<k3d::node > > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::material_sink<k3d::node > > > base;

public:
	bevel_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("Distance")) + init_description(_("Offset distance along edges.")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_fraction(init_owner(*this) + init_name("fraction") + init_label(_("Fraction")) + init_description(_("Offset position as a fraction of edge length.")) + init_value(0.1) + init_step_increment(0.05) + init_units(typeid(k3d::measurement::scalar))),
		m_cap(init_owner(*this) + init_name("cap") + init_label(_("Cap")) + init_description(_("Cap each bevelled point with a new face.")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_distance.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_fraction.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_cap.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// Create output geometry
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points)
			return;
		if(!Output.point_selection)
			return;

		// Clear previously cached data
		bevel_vertices.clear();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::bool_t cap = m_cap.pipeline_value();

		k3d::mesh::points_t& points = Output.points.writable();
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Identify face "corners" to be bevelled ...
			bevel_corners corners;

			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + polyhedron->clockwise_edges.size();
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(!point_selection[polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]])
					continue;

				corners.push_back(bevel_corner(edge, polyhedron->clockwise_edges[edge]));
			}

			// Create a mapping from edges to adjacent edges ...
			k3d::mesh::bools_t boundary_edges;
			k3d::mesh::indices_t adjacent_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

			// Create a mapping from edges to faces ...
			k3d::mesh::indices_t edge_faces;
			k3d::polyhedron::create_edge_face_lookup(*polyhedron, edge_faces);

			// Create mappings from edges to corners ...
			k3d::mesh::indices_t edge1_corners(polyhedron->clockwise_edges.size(), corners.size());
			k3d::mesh::indices_t edge2_corners(polyhedron->clockwise_edges.size(), corners.size());
			for(k3d::uint_t corner = 0; corner != corners.size(); ++corner)
			{
				edge1_corners[corners[corner].edge1] = corner;
				edge2_corners[corners[corner].edge2] = corner;
			}

			// Get ready to copy attributes ...
			k3d::table_copier point_attributes(Output.point_attributes);
			k3d::table_copier edge_attributes(polyhedron->edge_attributes);
			k3d::table_copier vertex_attributes(polyhedron->vertex_attributes);
			k3d::table_copier face_attributes(polyhedron->face_attributes);

			// Create new bevel points, being careful to avoid duplicating adjacent points ...
			for(bevel_corners::iterator corner = corners.begin(); corner != corners.end(); ++corner)
			{
				if(!corner->point1)
				{
					const k3d::uint_t new_point = points.size();

					corner->point1 = new_point;

					if(!boundary_edges[corner->edge1])
						corners[edge2_corners[adjacent_edges[corner->edge1]]].point2 = new_point;

					bevel_vertices.push_back(
						bevel_vertex(
							new_point,
							points[polyhedron->vertex_points[corner->edge2]],
							points[polyhedron->vertex_points[corner->edge1]]));
					points.push_back(k3d::point3());
					point_selection.push_back(1);

					point_attributes.push_back(polyhedron->vertex_points[corner->edge2]);
				}

				if(!corner->point2)
				{
					const k3d::uint_t new_point = points.size();

					corner->point2 = new_point;

					if(!boundary_edges[corner->edge2])
						corners[edge1_corners[adjacent_edges[corner->edge2]]].point1 = new_point;

					bevel_vertices.push_back(
						bevel_vertex(
							new_point,
							points[polyhedron->vertex_points[corner->edge2]],
							points[polyhedron->vertex_points[polyhedron->clockwise_edges[corner->edge2]]]));
					points.push_back(k3d::point3());
					point_selection.push_back(1);

					point_attributes.push_back(polyhedron->vertex_points[corner->edge2]);
				}
			}

			// Create a bevel edge for each face corner ...
			for(bevel_corners::iterator corner = corners.begin(); corner != corners.end(); ++corner)
			{
				corner->new_edge = polyhedron->clockwise_edges.size();

				polyhedron->clockwise_edges[corner->edge1] = *corner->new_edge;

				polyhedron->clockwise_edges.push_back(corner->edge2);
				polyhedron->edge_selections.push_back(1);
				polyhedron->vertex_points.push_back(*corner->point1);
				polyhedron->vertex_selections.push_back(0);

				k3d::uint_t edge_indices[2] = { corner->edge1, corner->edge2 };
				k3d::double_t edge_weights[2] = { 0.5, 0.5 };
				edge_attributes.push_back(2, edge_indices, edge_weights);

				vertex_attributes.push_back(corner->edge2);

				polyhedron->vertex_points[corner->edge2] = *corner->point2;
			}

			// Optionally create bevel faces ...
			if(cap)
			{
				// Create a face for each group of adjacent corners ...
				k3d::mesh::bools_t used_corners(corners.size(), false);
				for(k3d::uint_t start_corner = 0; start_corner != corners.size(); ++start_corner)
				{
					if(used_corners[start_corner])
						continue;

					k3d::bool_t loop = false;
					k3d::mesh::indices_t clockwise_corners;

					// Follow adjacent corners in the clockwise direction until we either hit
					// the polyhedron boundary or create a loop ...
					for(k3d::uint_t adjacent_corner = start_corner; ;)
					{
						clockwise_corners.push_back(adjacent_corner);
						used_corners[adjacent_corner] = true;

						if(boundary_edges[corners[adjacent_corner].edge1])
							break;

						adjacent_corner = edge2_corners[adjacent_edges[corners[adjacent_corner].edge1]];
						if(adjacent_corner == start_corner)
						{
							loop = true;
							break;
						}
					}

					// If we didn't create a loop, follow adjacent corners in the counter-clockwise
					// direction until we hit the polyhedron boundary ...
					if(!loop)
					{
						for(k3d::uint_t adjacent_corner = start_corner; ;)
						{
							if(boundary_edges[corners[adjacent_corner].edge2])
								break;

							adjacent_corner = edge1_corners[adjacent_edges[corners[adjacent_corner].edge2]];

							clockwise_corners.insert(clockwise_corners.begin(), adjacent_corner);
							used_corners[adjacent_corner] = true;
						}
					}

					// Create a new face ...
					polyhedron->face_shells.push_back(polyhedron->face_shells[edge_faces[corners[clockwise_corners[0]].edge1]]);
					polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
					polyhedron->face_loop_counts.push_back(1);
					polyhedron->face_selections.push_back(1);
					polyhedron->face_materials.push_back(material);

					// Create new face attributes ...
					k3d::mesh::indices_t face_indices(clockwise_corners.size());
					for(k3d::uint_t corner = 0; corner != clockwise_corners.size(); ++corner)
						face_indices[corner] = edge_faces[corners[clockwise_corners[corner]].edge1];
					k3d::mesh::weights_t face_weights(clockwise_corners.size(), 1.0 / clockwise_corners.size());
					face_attributes.push_back(clockwise_corners.size(), &face_indices[0], &face_weights[0]);

					polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

					const k3d::uint_t first_edge = polyhedron->clockwise_edges.size();
					for(k3d::uint_t corner = 0; corner != clockwise_corners.size(); ++corner)
					{
						polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
						polyhedron->edge_selections.push_back(1);
						polyhedron->vertex_points.push_back(*corners[clockwise_corners[corner]].point2);
						polyhedron->vertex_selections.push_back(0);

						k3d::uint_t edge_indices[2] = { corners[clockwise_corners[corner]].edge1, corners[clockwise_corners[corner]].edge2 };
						k3d::double_t edge_weights[2] = { 0.5, 0.5 };
						edge_attributes.push_back(2, edge_indices, edge_weights);

						vertex_attributes.push_back(corners[clockwise_corners[corner]].edge2);
					}
					if(loop)
					{
						polyhedron->clockwise_edges.back() = first_edge;
					}
					else
					{
						polyhedron->clockwise_edges.push_back(first_edge);
						polyhedron->edge_selections.push_back(1);
						polyhedron->vertex_points.push_back(*corners[clockwise_corners.back()].point1);
						polyhedron->vertex_selections.push_back(0);

						k3d::uint_t edge_indices[2] = { corners[clockwise_corners.front()].edge2, corners[clockwise_corners.back()].edge1 };
						k3d::double_t edge_weights[2] = { 0.5, 0.5 };
						edge_attributes.push_back(2, edge_indices, edge_weights);

						vertex_attributes.push_back(corners[clockwise_corners.back()].edge2);
					}
				}
			}
		}

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points, point_map);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		const k3d::double_t distance = m_distance.pipeline_value();
		const k3d::double_t fraction = m_fraction.pipeline_value();

		k3d::mesh::points_t& points = Output.points.writable();

		const k3d::uint_t vertex_begin = 0;
		const k3d::uint_t vertex_end = vertex_begin + bevel_vertices.size();
		for(k3d::uint_t vertex_index = vertex_begin; vertex_index != vertex_end; ++vertex_index)
		{
			const bevel_vertex& vertex = bevel_vertices[vertex_index];

			points[point_map[vertex.point]] =
				k3d::mix(vertex.start, vertex.end, fraction)
				+ (k3d::normalize(vertex.end - vertex.start) * distance);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bevel_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x5a578576, 0xc207453f, 0xa7a2b9ae, 0xfb1ad739),
				"BevelPoints",
				"Bevels a surface at each selected point",
				"Polyhedron",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	/// Caches polyhedron "corners" that are to be bevelled
	struct bevel_corner
	{
		bevel_corner(const k3d::uint_t Edge1, const k3d::uint_t Edge2) :
			edge1(Edge1),
			edge2(Edge2)
		{
		}

		k3d::uint_t edge1;
		boost::optional<k3d::uint_t> point1;
		boost::optional<k3d::uint_t> new_edge;
		boost::optional<k3d::uint_t> point2;
		k3d::uint_t edge2;
	};

	typedef std::vector<bevel_corner> bevel_corners;

	/// Caches new points for better interactive performance
	struct bevel_vertex
	{
		bevel_vertex(const k3d::uint_t Point, const k3d::point3& Start, const k3d::point3& End) :
			point(Point),
			start(Start),
			end(End)
		{
		}

		k3d::uint_t point;
		k3d::point3 start;
		k3d::point3 end;
	};

	/// Caches new points for better interactive performance
	std::vector<bevel_vertex> bevel_vertices;
	/// Caches a mapping from the original input point indices to the new output point indices (because we delete existing points)
	k3d::mesh::indices_t point_map;

	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_distance;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_fraction;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap;
};

/////////////////////////////////////////////////////////////////////////////
// bevel_points_factory

k3d::iplugin_factory& bevel_points_factory()
{
	return bevel_points::get_factory();
}

} // namespace polyhedron

} // namespace module


