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
*/

#include "helpers.h"

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>

namespace libk3dpolyhedra
{

/////////////////////////////////////////////////////////////////////////////
// bevel_points_implementation

class bevel_points_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::material_sink<k3d::node > > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::material_sink<k3d::node > > > base;

public:
	bevel_points_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_offset(init_owner(*this) + init_name("offset") + init_label(_("Offset")) + init_description(_("Offset along external edges")) + init_value(0.3) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("Distance")) + init_description(_("Use distance instead of edge offset")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());

		m_material.changed_signal().connect(make_update_mesh_slot());
		m_offset.changed_signal().connect(make_update_mesh_slot());
		m_distance.changed_signal().connect(make_update_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& Input, k3d::legacy::mesh& Output)
	{
		// Clear previously cached data
		m_bevel_points.clear();
		m_new_faces.clear();

		// Create output geometry
		k3d::legacy::deep_copy(Input, Output);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Output);

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron_i = Output.polyhedra.begin(); polyhedron_i != Output.polyhedra.end(); ++polyhedron_i)
		{
			k3d::legacy::polyhedron& polyhedron = **polyhedron_i;

			// Get selected points and edges ending at them
			points_t points;
			point_edge_map_t point_edge_map;
			for(k3d::legacy::polyhedron::faces_t::iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					// For every edge ending with a selected vertex ...
					k3d::legacy::point* endpoint = edge->face_clockwise->vertex;
					if(endpoint->selection_weight)
					{
						// Add edge to the point-edge map ...
						if(point_edge_map.count(endpoint))
						{
							point_edge_map[endpoint].push_back(edge);
						}
						else
						{
							points.push_back(endpoint);
							point_edge_map.insert(std::make_pair(endpoint, edges_t(1, edge)));
						}
					}

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}

			// Bevel points
			faces_t new_faces;
			bevel_points(points, point_edge_map, Output, m_bevel_points, new_faces);

			// Save new faces
			polyhedron.faces.insert(polyhedron.faces.end(), new_faces.begin(), new_faces.end());
			m_new_faces.insert(m_new_faces.end(), new_faces.begin(), new_faces.end());

			assert_warning(k3d::legacy::is_valid(polyhedron));
		}

		// Update output selection
		for(faces_t::iterator face = m_new_faces.begin(); face != m_new_faces.end(); ++face)
			(*face)->selection_weight = 1.0;
	}

	void on_update_mesh(const k3d::legacy::mesh& Input, k3d::legacy::mesh& Output)
	{
		const double offset = m_offset.pipeline_value();
		const bool distance = m_distance.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		for(bevel_points_t::iterator bevel_point = m_bevel_points.begin(); bevel_point != m_bevel_points.end(); ++bevel_point)
			bevel_point->update(offset, distance);

		for(faces_t::iterator face = m_new_faces.begin(); face != m_new_faces.end(); ++face)
			(*face)->material = material;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bevel_points_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x5a578576, 0xc207453f, 0xa7a2b9ae, 0xfb1ad739),
				"BevelPoints",
				"Bevels a surface at each selected point",
				"Polygons",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	/// Caches new geometry for better interactive performance
	class bevel_point
	{
	public:
		bevel_point(const k3d::point3& StartPosition, const k3d::point3& EndPosition, k3d::legacy::point* Point) :
			start_position(StartPosition),
			end_position(EndPosition),
			point(Point)
		{
		}

		void update(const double Offset, const bool Distance)
		{
			double edge_offset = Offset;
			if(!Distance)
			{
				// Offset gives position on the edge
				if(Offset > 1)
					edge_offset = 1;
			}
			else
			{
				// Offset is a length from start_position
				const double length = k3d::distance(end_position, start_position);
				if(length)
					edge_offset = Offset / length;
			}

			point->position = start_position + (end_position - start_position) * edge_offset;
		}

	private:
		k3d::point3 start_position;
		k3d::point3 end_position;
		k3d::legacy::point* point;
	};

	typedef std::vector<k3d::legacy::face*> faces_t;
	typedef std::vector<k3d::legacy::split_edge*> edges_t;
	typedef std::vector<k3d::legacy::point*> points_t;
	typedef std::map<k3d::legacy::point*, edges_t> point_edge_map_t;
	typedef std::vector<bevel_point> bevel_points_t;

	/// Caches edge start and end for easy update
	k3d::legacy::point* save_edge(k3d::legacy::split_edge* Edge, bevel_points_t& BevelPoints, const bool Invert = false)
	{
		const k3d::point3& start_position = Edge->vertex->position;
		const k3d::point3& end_position = Edge->face_clockwise->vertex->position;

		k3d::legacy::point* new_point = new k3d::legacy::point(start_position);
		assert_warning(new_point);

		if(!Invert)
			BevelPoints.push_back(bevel_point(end_position, start_position, new_point));
		else
			BevelPoints.push_back(bevel_point(start_position, end_position, new_point));

		return new_point;
	}

	void bevel_points(const points_t& Points, const point_edge_map_t& PointEdgeMap, k3d::legacy::mesh& Mesh, bevel_points_t& BevelPoints, faces_t& NewFaces)
	{
		// Create new points and save original edge positions before bevelling
		typedef std::vector<k3d::legacy::point*> points_t;
		points_t new_points;
		points_t border_new_points;

		points_t interior_points;
		edges_t interior_edges;
		points_t border_points;
		typedef std::vector<edges_t> edge_lists_t;
		edge_lists_t border_edges;
		for(points_t::const_iterator point = Points.begin(); point != Points.end(); ++point)
		{
			const edges_t& edges = PointEdgeMap.find(*point)->second;

			// Don't bevel single points
			if(edges.size() == 1)
				continue;

			// Separate interior points from border points
			k3d::legacy::split_edge* start_edge = edges.front();
			const bool interior = (0 != helpers::vertex_valency(start_edge->face_clockwise));
			edges_t point_edges;
			if(interior)
			{
				interior_points.push_back(*point);
				interior_edges.push_back(start_edge);

				k3d::legacy::split_edge* edge = start_edge;
				do
				{
					new_points.push_back(save_edge(edge, BevelPoints));

					edge = edge->face_clockwise->companion;
				}
				while(edge != start_edge);
			}
			else
			{
				// Save border edges only, each one will give a triangle fan
				edges_t fan_starts;
				for(edges_t::const_iterator edge = edges.begin(); edge != edges.end(); ++edge)
				{
					if(!(*edge)->companion)
					{
						fan_starts.push_back(*edge);

						k3d::legacy::split_edge* fan_edge = *edge;
						do
						{
							border_new_points.push_back(save_edge(fan_edge, BevelPoints));

							fan_edge = fan_edge->face_clockwise->companion;
							return_if_fail(fan_edge);
						}
						while(fan_edge->face_clockwise->companion);

						// Save two last edges, i
						border_new_points.push_back(save_edge(fan_edge, BevelPoints));
						border_new_points.push_back(save_edge(fan_edge->face_clockwise, BevelPoints, true));
					}
				}

				assert_warning(fan_starts.size());
				if(fan_starts.size())
				{
					border_points.push_back(*point);
					border_edges.push_back(fan_starts);
				}
			}
		}

		// Actually bevel points
		points_t::iterator new_point = new_points.begin();
		points_t::iterator bevel_point = interior_points.begin();
		edges_t::iterator start_edge = interior_edges.begin();
		for(; bevel_point != interior_points.end(); ++bevel_point, ++start_edge)
		{
			k3d::legacy::point* vertex = *bevel_point;

			// Save edges with selected vertex as endpoint
			edges_t old_edges;

			k3d::legacy::split_edge* edge = *start_edge;
			do
			{
				old_edges.push_back(edge);
				edge = edge->face_clockwise->companion;
			}
			while(edge != *start_edge);

			// For each edge, append a new edge
			edges_t bevel_edges;
			points_t current_new_points;

			for(edges_t::iterator edge_i = old_edges.begin(); edge_i != old_edges.end(); ++edge_i)
			{
				edge = *edge_i;

				// Add new split_edge
				k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(*new_point);
				new_edge->face_clockwise = edge->face_clockwise;
				edge->face_clockwise = new_edge;

				current_new_points.push_back(*new_point);
				++new_point;

				// Save edges to update them afterwards
				bevel_edges.push_back(new_edge);
			}

			// Assign new points
			const unsigned long n = bevel_edges.size();
			for(unsigned long i = 0; i < n; ++i)
			{
				bevel_edges[i]->face_clockwise->vertex = current_new_points[(i + 1)%n];
			}

			// Delete bevelled point
			Mesh.points.erase(std::remove(Mesh.points.begin(), Mesh.points.end(), vertex), Mesh.points.end());
			delete vertex;

			// Create new face
			k3d::legacy::face* new_face = helpers::fill_hole(bevel_edges.rbegin(), bevel_edges.rend(), 0);
			NewFaces.push_back(new_face);
		}

		// Bevel border points
		points_t::iterator border_new_point = border_new_points.begin();
		bevel_point = border_points.begin();
		edge_lists_t::iterator start_edges = border_edges.begin();
		for(; bevel_point != border_points.end(); ++bevel_point, ++start_edges)
		{
			k3d::legacy::point* vertex = *bevel_point;
			for(edges_t::iterator start_edge = start_edges->begin(); start_edge != start_edges->end(); ++start_edge)
			{
				// Save edge fan
				edges_t old_edges;

				k3d::legacy::split_edge* edge = *start_edge;
				do
				{
					old_edges.push_back(edge);

					edge = edge->face_clockwise->companion;
				}
				while(edge->face_clockwise->companion);

				old_edges.push_back(edge);

				// For each edge, append a new edge
				edges_t bevel_edges;
				points_t current_new_points;

				for(edges_t::iterator edge_i = old_edges.begin(); edge_i != old_edges.end(); ++edge_i)
				{
					edge = *edge_i;

					// Add new split_edge
					k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(*border_new_point);
					new_edge->face_clockwise = edge->face_clockwise;
					edge->face_clockwise = new_edge;

					current_new_points.push_back(*border_new_point);
					++border_new_point;

					// Save edges to update them afterwards
					bevel_edges.push_back(new_edge);
				}

				current_new_points.push_back(*border_new_point++);

				// Assign new points
				const unsigned long n = bevel_edges.size();
				for(unsigned long i = 0; i < n; ++i)
				{
					bevel_edges[i]->face_clockwise->vertex = current_new_points[i + 1];
				}

				// Create new face
				edges_t new_edges;

				edges_t::reverse_iterator r_edge = bevel_edges.rbegin();
				do
				{
					k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge((*r_edge)->face_clockwise->vertex);

					// Set companions
					k3d::legacy::join_edges(**r_edge, *new_edge);

					new_edges.push_back(new_edge);
				}
				while(++r_edge != bevel_edges.rend());

				k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge((*bevel_edges.begin())->vertex);
				new_edges.push_back(new_edge);

				// Create new face
				k3d::legacy::loop_edges(new_edges.begin(), new_edges.end());
				k3d::legacy::face* new_face = new k3d::legacy::face(new_edges.front(), 0);

				NewFaces.push_back(new_face);
			}

			// Delete bevelled point
			Mesh.points.erase(std::remove(Mesh.points.begin(), Mesh.points.end(), vertex), Mesh.points.end());
			delete vertex;
		}

		// Add new points
		Mesh.points.insert(Mesh.points.end(), new_points.begin(), new_points.end());
		Mesh.points.insert(Mesh.points.end(), border_new_points.begin(), border_new_points.end());
	}

	bevel_points_t m_bevel_points;
	faces_t m_new_faces;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_offset;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_distance;
};

/////////////////////////////////////////////////////////////////////////////
// bevel_points_factory

k3d::iplugin_factory& bevel_points_factory()
{
	return bevel_points_implementation::get_factory();
}

} // namespace libk3dpolyhedra

