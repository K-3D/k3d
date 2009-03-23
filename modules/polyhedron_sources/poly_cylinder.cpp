// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>

#include <iterator>

namespace module
{

namespace polyhedron
{

namespace sources
{

/////////////////////////////////////////////////////////////////////////////
// poly_cylinder_implementation

class poly_cylinder_implementation :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::node > > base;

public:
	poly_cylinder_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("U segments")) + init_description(_("Columns")) + init_value(32) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("V segments")) + init_description(_("Rows")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_top(init_owner(*this) + init_name("top") + init_label(_("Top")) + init_description(_("Cap cylinder top")) + init_value(true)),
		m_bottom(init_owner(*this) + init_name("bottom") + init_label(_("Bottom")) + init_description(_("Cap cylinder bottom")) + init_value(true)),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Cylinder radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmax(init_owner(*this) + init_name("zmax") + init_label(_("Z max")) + init_description(_("Z max (RenderMan convention)")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmin(init_owner(*this) + init_name("zmin") + init_label(_("Z min")) + init_description(_("Z min (RenderMan convention)")) + init_value(-5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_power(init_owner(*this) + init_name("u_power") + init_label(_("U power")) + init_description(_("Radial power")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_top_segments(init_owner(*this) + init_name("top_segments") + init_label(_("Top segments")) + init_description(_("0 : single face, >= 1 : segments")) + init_value(0) + init_constraint(constraint::minimum<k3d::int32_t>(0)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_bottom_segments(init_owner(*this) + init_name("bottom_segments") + init_label(_("Bottom segments")) + init_description(_("0 : single face, >= 1 : segments")) + init_value(0) + init_constraint(constraint::minimum<k3d::int32_t>(0)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
		m_top.changed_signal().connect(make_reset_mesh_slot());
		m_bottom.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_zmax.changed_signal().connect(make_reset_mesh_slot());
		m_zmin.changed_signal().connect(make_reset_mesh_slot());
		m_u_power.changed_signal().connect(make_reset_mesh_slot());
		m_top_segments.changed_signal().connect(make_reset_mesh_slot());
		m_bottom_segments.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const unsigned long u_segments = m_u_segments.pipeline_value();
		const unsigned long v_segments = m_v_segments.pipeline_value();
		const double radius = m_radius.pipeline_value();
		const double zmax = m_zmax.pipeline_value();
		const double zmin = m_zmin.pipeline_value();
		const double u_power = m_u_power.pipeline_value();
		const double inv_u_power = u_power ? 1.0 / u_power : 1.0;
		const bool top = m_top.pipeline_value();
		const bool bottom = m_bottom.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		// Create the cylinder body
		const k3d::legacy::grid_results_t grid = k3d::legacy::add_grid(Mesh, polyhedron, v_segments, u_segments, false, true, material);
		const boost::multi_array<k3d::legacy::point*, 2>& points = boost::get<0>(grid);
		const boost::multi_array<k3d::legacy::split_edge*, 3>& edges = boost::get<1>(grid);

		// Shape the cylinder points
		for(unsigned long v = 0; v <= v_segments; ++v)
		{
			const double height = static_cast<double>(v) / static_cast<double>(v_segments);

			for(unsigned long u = 0; u != u_segments; ++u)
			{
				const double theta = k3d::pi_times_2() * static_cast<double>(u) / static_cast<double>(u_segments);

				double x = cos(theta);
				double y = -sin(theta);
				double z = k3d::mix(zmax, zmin, height);

				x = radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
				y = radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

				points[v][u]->position = k3d::point3(x, y, z);
			}
		}

		// Optionally cap the top of the cylinder ...
		if(top)
		{
			const unsigned long top_segments = m_top_segments.pipeline_value();

			if(!top_segments)
			{
				std::vector<k3d::legacy::split_edge*> new_edges;
				for(unsigned long u = 0; u != u_segments; ++u)
					new_edges.push_back(new k3d::legacy::split_edge(edges[0][(u_segments - u) % u_segments][0]->vertex));

				for(unsigned long u = 0; u != u_segments; ++u)
				{
					new_edges[u]->face_clockwise = new_edges[(u + 1) % u_segments];
					k3d::legacy::join_edges(*new_edges[u], *edges[0][(u_segments-1)-u][0]);
				}

				polyhedron.faces.push_back(new k3d::legacy::face(new_edges[0], material));
			}
			else
			{
				k3d::point3 middle(0, 0, zmax);
				k3d::legacy::point* middle_point = new k3d::legacy::point(middle);
				Mesh.points.push_back(middle_point);

				// Init top edge list
				std::vector<k3d::legacy::split_edge*> top_edges;
				top_edges.reserve(u_segments);
				for(unsigned long u = 0; u < u_segments; ++u)
				{
					k3d::legacy::split_edge* top_edge = edges[0][(u_segments-1) - u][0];
					top_edges.push_back(top_edge);
				}

				// Create segment quads
				for(unsigned long n = top_segments; n > 1; --n)
				{
					std::vector<k3d::legacy::split_edge*> new_top_edges;
					new_top_edges.reserve(u_segments);
					std::vector<k3d::legacy::point*> new_points;
					new_points.reserve(u_segments);

					// Create bottom points
					const double factor = static_cast<double>(n - 1) / static_cast<double>(top_segments);
					for(unsigned long u = 0; u < u_segments; ++u)
					{
						k3d::legacy::split_edge* border_edge = edges[0][(u_segments-1) - u][0];
						new_points.push_back(new k3d::legacy::point(middle + factor * (border_edge->vertex->position - middle)));
					}

					// Add quads that link top edges and bottom points
					k3d::legacy::split_edge* first_edge = 0;
					k3d::legacy::split_edge* previous_edge = 0;
					for(unsigned long u = 0; u != u_segments; ++u)
					{
						k3d::legacy::split_edge* top_edge = top_edges[u];

						// Create a quad
						k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(top_edge->face_clockwise->vertex);
						k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(top_edge->vertex);
						k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(new_points[u]);
						k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(new_points[u ? u - 1 : (u_segments - 1)]);
						k3d::legacy::join_edges(*top_edge, *edge1);

						if(!previous_edge)
							first_edge = edge4;
						else
							k3d::legacy::join_edges(*edge4, *previous_edge);

						previous_edge = edge2;

						edge1->face_clockwise = edge2;
						edge2->face_clockwise = edge3;
						edge3->face_clockwise = edge4;
						edge4->face_clockwise = edge1;
						polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

						new_top_edges.push_back(edge3);
					}

					k3d::legacy::join_edges(*previous_edge, *first_edge);

					Mesh.points.insert(Mesh.points.end(), new_points.begin(), new_points.end());

					std::swap(top_edges, new_top_edges);
				}

				// Create middle triangle fan
				k3d::legacy::split_edge* first_edge = 0;
				k3d::legacy::split_edge* previous_edge = 0;
				for(unsigned long u = 0; u != u_segments; ++u)
				{
/*
					// Quads work with even u_segments only
					if(QuadsOnly && !(point_u_segments & 1))
					{
						// Create a quad joining edge and top sphere point
						k3d::legacy::split_edge* common_edge = edges[0][(point_u_segments - u) % point_u_segments][0];
						++u;
						k3d::legacy::split_edge* next_common_edge = edges[0][(point_u_segments - u) % point_u_segments][0];

						k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(top_vertex);
						k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(common_edge->face_clockwise->vertex);
						k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(common_edge->vertex);
						k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(next_common_edge->vertex);

						edge1->face_clockwise = edge2;
						edge2->face_clockwise = edge3;
						edge3->face_clockwise = edge4;
						edge4->face_clockwise = edge1;
						polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

						k3d::legacy::join_edges(*common_edge, *edge2);
						k3d::legacy::join_edges(*next_common_edge, *edge3);

						if(!previous_edge)
							first_edge = edge1;
						else
							k3d::legacy::join_edges(*edge1, *previous_edge);

						previous_edge = edge4;
					}
					else
*/
					{
						// Create a triangle joining edge and top sphere point
						k3d::legacy::split_edge* top_edge = top_edges[u];
						k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(top_edge->face_clockwise->vertex);
						k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(top_edge->vertex);
						k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(middle_point);
						edge1->face_clockwise = edge2;
						edge2->face_clockwise = edge3;
						edge3->face_clockwise = edge1;
						polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

						k3d::legacy::join_edges(*top_edge, *edge1);

						if(!previous_edge)
							first_edge = edge3;
						else
							k3d::legacy::join_edges(*edge3, *previous_edge);

						previous_edge = edge2;
					}
				}

				k3d::legacy::join_edges(*previous_edge, *first_edge);
			}
		}

		// Optionally cap the bottom of the cylinder ...
		if(bottom)
		{
			const unsigned long bottom_segments = m_bottom_segments.pipeline_value();

			if(!bottom_segments)
			{
				std::vector<k3d::legacy::split_edge*> new_edges;
				for(unsigned long u = 0; u != u_segments; ++u)
					new_edges.push_back(new k3d::legacy::split_edge(edges[v_segments-1][u][3]->vertex));

				for(unsigned long u = 0; u != u_segments; ++u)
				{
					new_edges[u]->face_clockwise = new_edges[(u + 1) % u_segments];
					k3d::legacy::join_edges(*new_edges[u], *edges[v_segments-1][u][2]);
				}

				polyhedron.faces.push_back(new k3d::legacy::face(new_edges[0], material));
			}
			else
			{
				k3d::point3 middle(0, 0, zmin);
				k3d::legacy::point* middle_point = new k3d::legacy::point(middle);
				Mesh.points.push_back(middle_point);

				// Init top edge list
				std::vector<k3d::legacy::split_edge*> top_edges;
				top_edges.reserve(u_segments);
				for(unsigned long u = 0; u < u_segments; ++u)
				{
					k3d::legacy::split_edge* top_edge = edges[v_segments-1][u][2];
					top_edges.push_back(top_edge);
				}

				// Create segment quads
				for(unsigned long n = bottom_segments; n > 1; --n)
				{
					std::vector<k3d::legacy::split_edge*> new_top_edges;
					new_top_edges.reserve(u_segments);
					std::vector<k3d::legacy::point*> new_points;
					new_points.reserve(u_segments);

					// Create bottom points
					const double factor = static_cast<double>(n - 1) / static_cast<double>(bottom_segments);
					for(unsigned long u = 0; u < u_segments; ++u)
					{
						k3d::legacy::split_edge* border_edge = edges[v_segments-1][u][2];
						new_points.push_back(new k3d::legacy::point(middle + factor * (border_edge->vertex->position - middle)));
					}

					// Add quads that link top edges and bottom points
					k3d::legacy::split_edge* first_edge = 0;
					k3d::legacy::split_edge* previous_edge = 0;
					for(unsigned long u = 0; u != u_segments; ++u)
					{
						k3d::legacy::split_edge* top_edge = top_edges[u];

						// Create a quad
						k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(top_edge->face_clockwise->vertex);
						k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(top_edge->vertex);
						k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(new_points[u]);
						k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(new_points[u ? u - 1 : (u_segments - 1)]);
						k3d::legacy::join_edges(*top_edge, *edge1);

						if(!previous_edge)
							first_edge = edge4;
						else
							k3d::legacy::join_edges(*edge4, *previous_edge);

						previous_edge = edge2;

						edge1->face_clockwise = edge2;
						edge2->face_clockwise = edge3;
						edge3->face_clockwise = edge4;
						edge4->face_clockwise = edge1;
						polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

						new_top_edges.push_back(edge3);
					}

					k3d::legacy::join_edges(*previous_edge, *first_edge);

					Mesh.points.insert(Mesh.points.end(), new_points.begin(), new_points.end());

					std::swap(top_edges, new_top_edges);
				}

				// Create middle's triangle fan
				k3d::legacy::split_edge* first_edge = 0;
				k3d::legacy::split_edge* previous_edge = 0;
				for(unsigned long u = 0; u != u_segments; ++u)
				{
					{
						// Create a triangle joining edge and top sphere point
						k3d::legacy::split_edge* top_edge = top_edges[u];
						k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(top_edge->face_clockwise->vertex);
						k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(top_edge->vertex);
						k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(middle_point);
						edge1->face_clockwise = edge2;
						edge2->face_clockwise = edge3;
						edge3->face_clockwise = edge1;
						polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

						k3d::legacy::join_edges(*top_edge, *edge1);

						if(!previous_edge)
							first_edge = edge3;
						else
							k3d::legacy::join_edges(*edge3, *previous_edge);

						previous_edge = edge2;
					}
				}

				k3d::legacy::join_edges(*previous_edge, *first_edge);
			}
		}

		if(top && bottom)
			assert_warning(k3d::legacy::is_solid(polyhedron));
		else
			assert_warning(k3d::legacy::is_valid(polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_cylinder_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xd8c4d9fd, 0x42334a54, 0xa4b48185, 0xd8506489),
			"PolyCylinder",
			_("Generates a polygonal cylinder with optional endcaps"),
			"Polygon",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_top;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bottom;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmax;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmin;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_u_power;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_top_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_bottom_segments;
};

/////////////////////////////////////////////////////////////////////////////
// poly_cylinder_factory

k3d::iplugin_factory& poly_cylinder_factory()
{
	return poly_cylinder_implementation::get_factory();
}

} // namespace sources

} // namespace polyhedron

} // namespace module

