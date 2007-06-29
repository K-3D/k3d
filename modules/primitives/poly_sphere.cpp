// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dprimitives
{

namespace detail
{

/// Sphere primitive
void sphere(const unsigned long u_segments, const unsigned long v_segments, const double radius, const double inv_u_power, const double inv_v_power, const bool QuadsOnly, k3d::imaterial* material, k3d::legacy::mesh* Mesh, k3d::legacy::polyhedron& polyhedron)
{
	// Special case where there's no sphere body
	if(2 == v_segments)
	{
		// Create the sphere points : top, equator and bottom
		k3d::legacy::point* top_vertex = new k3d::legacy::point(0, 0, radius);
		Mesh->points.push_back(top_vertex);

		for(unsigned long u = 0; u < u_segments; ++u)
		{
			k3d::legacy::point* equator_vertex = new k3d::legacy::point(0, 0, 0);
			Mesh->points.push_back(equator_vertex);
		}

		k3d::legacy::point* bottom_vertex = new k3d::legacy::point(0, 0, -radius);
		Mesh->points.push_back(bottom_vertex);

		// Top and bottom triangle fans
		typedef std::vector<k3d::legacy::split_edge*> edges_t;
		k3d::legacy::split_edge* top_first_edge = 0;
		k3d::legacy::split_edge* top_previous_edge = 0;
		k3d::legacy::split_edge* bottom_first_edge = 0;
		k3d::legacy::split_edge* bottom_previous_edge = 0;
		for(unsigned long u = 0; u != u_segments; ++u)
		{
			k3d::legacy::point* point1 = Mesh->points[1 + u];
			k3d::legacy::point* point2 = Mesh->points[1 + (u + 1) % u_segments];

			k3d::legacy::split_edge* common_edge_top = 0;
			k3d::legacy::split_edge* common_edge_bottom = 0;

			// Top
			{
				//      * e1
				//
				// e3 *   * e2
				k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(top_vertex);
				k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(point1);
				k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(point2);
				edge1->face_clockwise = edge2;
				edge2->face_clockwise = edge3;
				edge3->face_clockwise = edge1;
				polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

				common_edge_top = edge2;

				if(!top_previous_edge)
					top_first_edge = edge1;
				else
					k3d::legacy::join_edges(*edge1, *top_previous_edge);

				top_previous_edge = edge3;
			}

			// Bottom
			{
				// e1 *   * e2
				//
				//      * e3
				k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(point2);
				k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(point1);
				k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(bottom_vertex);
				edge1->face_clockwise = edge2;
				edge2->face_clockwise = edge3;
				edge3->face_clockwise = edge1;
				polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

				common_edge_bottom = edge1;

				if(!bottom_previous_edge)
					bottom_first_edge = edge3;
				else
					k3d::legacy::join_edges(*edge3, *bottom_previous_edge);

				bottom_previous_edge = edge2;
			}

			// Join top and bottom
			k3d::legacy::join_edges(*common_edge_top, *common_edge_bottom);
		}
		k3d::legacy::join_edges(*top_previous_edge, *top_first_edge);
		k3d::legacy::join_edges(*bottom_previous_edge, *bottom_first_edge);

		// Set point positions
		for(unsigned long u = 0; u != u_segments; ++u)
		{
			const double theta = k3d::pi_times_2() * static_cast<double>(u) / static_cast<double>(u_segments);

			double x = cos(theta);
			double y = -sin(theta);

			x = radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
			y = radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

			Mesh->points[u_segments - u]->position = k3d::point3(x, y, 0);
		}

		return;
	}

	// Create the sphere body which is a grid
	k3d::legacy::point* top_vertex = new k3d::legacy::point(0, 0, radius);
	Mesh->points.push_back(top_vertex);
	const k3d::legacy::grid_results_t grid = k3d::legacy::add_grid(*Mesh, polyhedron, v_segments - 2, u_segments, false, true, material);
	k3d::legacy::point* bottom_vertex = new k3d::legacy::point(0, 0, -radius);
	Mesh->points.push_back(bottom_vertex);

	const boost::multi_array<k3d::legacy::point*, 2>& points = boost::get<0>(grid);
	const boost::multi_array<k3d::legacy::split_edge*, 3>& edges = boost::get<1>(grid);
	const unsigned long point_v_segments = points.shape()[0];
	const unsigned long point_u_segments = points.shape()[1];

	// Cap the top of the sphere with a triangle fan
	{
		k3d::legacy::split_edge* first_edge = 0;
		k3d::legacy::split_edge* previous_edge = 0;
		for(unsigned long u = 0; u != point_u_segments; ++u)
		{
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
			{
				// Create a triangle joining edge and top sphere point
				k3d::legacy::split_edge* common_edge = edges[0][(point_u_segments - u) % point_u_segments][0];
				k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(top_vertex);
				k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(common_edge->face_clockwise->vertex);
				k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(common_edge->vertex);
				edge1->face_clockwise = edge2;
				edge2->face_clockwise = edge3;
				edge3->face_clockwise = edge1;
				polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

				k3d::legacy::join_edges(*common_edge, *edge2);

				if(!previous_edge)
					first_edge = edge1;
				else
					k3d::legacy::join_edges(*edge1, *previous_edge);

				previous_edge = edge3;
			}
		}

		k3d::legacy::join_edges(*previous_edge, *first_edge);
	}

	// Cap the bottom of the sphere with a triangle fan
	{
		k3d::legacy::split_edge* first_edge = 0;
		k3d::legacy::split_edge* previous_edge = 0;
		for(unsigned long u = 0; u != point_u_segments; ++u)
		{
			// Quads work with even u_segments only
			if(QuadsOnly && !(point_u_segments & 1))
			{
				// Create a quad joining edge and bottom sphere point
				k3d::legacy::split_edge* common_edge = k3d::legacy::face_anticlockwise(edges[point_v_segments-2][u][3]);
				++u;
				k3d::legacy::split_edge* next_common_edge = k3d::legacy::face_anticlockwise(edges[point_v_segments-2][u][3]);

				k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(next_common_edge->vertex);
				k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(bottom_vertex);
				k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(common_edge->face_clockwise->vertex);
				k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(common_edge->vertex);
				edge1->face_clockwise = edge2;
				edge2->face_clockwise = edge3;
				edge3->face_clockwise = edge4;
				edge4->face_clockwise = edge1;
				polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

				k3d::legacy::join_edges(*common_edge, *edge3);
				k3d::legacy::join_edges(*next_common_edge, *edge4);

				if(!previous_edge)
					first_edge = edge2;
				else
					k3d::legacy::join_edges(*edge2, *previous_edge);

				previous_edge = edge1;
			}
			else
			{
				// Create a triangle joining edge and bottom sphere point
				k3d::legacy::split_edge* common_edge = k3d::legacy::face_anticlockwise(edges[point_v_segments-2][u][3]);
				k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(common_edge->vertex);
				k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(bottom_vertex);
				k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(common_edge->face_clockwise->vertex);
				edge1->face_clockwise = edge2;
				edge2->face_clockwise = edge3;
				edge3->face_clockwise = edge1;
				polyhedron.faces.push_back(new k3d::legacy::face(edge1, material));

				k3d::legacy::join_edges(*common_edge, *edge3);

				if(!previous_edge)
					first_edge = edge2;
				else
					k3d::legacy::join_edges(*edge2, *previous_edge);

				previous_edge = edge1;
			}
		}

		k3d::legacy::join_edges(*previous_edge, *first_edge);
	}

	// Set body point positions
	for(unsigned long v = 0; v != point_v_segments; ++v)
	{
		const double phi = k3d::pi() * static_cast<double>(1 + v) / static_cast<double>(point_v_segments + 1);
		const double varying_radius = k3d::mix(radius * 0.001, radius, sin(phi));

		double z = cos(phi);
		z = radius * k3d::sign(z) * std::pow(std::abs(z), inv_v_power);

		for(unsigned long u = 0; u != point_u_segments; ++u)
		{
			const double theta = k3d::pi_times_2() * static_cast<double>(u) / static_cast<double>(point_u_segments);

			double x = cos(theta);
			double y = -sin(theta);

			x = varying_radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
			y = varying_radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

			points[v][u]->position = k3d::point3(x, y, z);
		}
	}
}

/// This primitive is basically a sphereized PolyCylinder: the end caps still exist but are scaled down to a single point
void sphereized_cylinder(const unsigned long u_segments, const unsigned long v_segments, const double radius, const double inv_u_power, const double inv_v_power, k3d::imaterial* material, k3d::legacy::mesh* Mesh, k3d::legacy::polyhedron& polyhedron)
{
	// Create the sphere body which is a grid
	const k3d::legacy::grid_results_t grid = k3d::legacy::add_grid(*Mesh, polyhedron, v_segments, u_segments, false, true, material);
	const boost::multi_array<k3d::legacy::point*, 2>& points = boost::get<0>(grid);
	const boost::multi_array<k3d::legacy::split_edge*, 3>& edges = boost::get<1>(grid);
	const unsigned long point_v_segments = points.shape()[0];
	const unsigned long point_u_segments = points.shape()[1];

	// Cap the top of the sphere so it is topologically solid
	{
		std::vector<k3d::legacy::split_edge*> new_edges;
		for(unsigned long u = 0; u != point_u_segments; ++u)
			new_edges.push_back(new k3d::legacy::split_edge(edges[0][(point_u_segments - u) % point_u_segments][0]->vertex));

		for(unsigned long u = 0; u != point_u_segments; ++u)
		{
			new_edges[u]->face_clockwise = new_edges[(u + 1) % point_u_segments];
			k3d::legacy::join_edges(*new_edges[u], *edges[0][(point_u_segments-1)-u][0]);
		}

		polyhedron.faces.push_back(new k3d::legacy::face(new_edges[0], material));
	}

	// Cap the bottom of the sphere so it is topologically solid
	{
		std::vector<k3d::legacy::split_edge*> new_edges;
		for(unsigned long u = 0; u != point_u_segments; ++u)
			new_edges.push_back(new k3d::legacy::split_edge(edges[point_v_segments-2][u][3]->vertex));

		for(unsigned long u = 0; u != point_u_segments; ++u)
		{
			new_edges[u]->face_clockwise = new_edges[(u + 1) % point_u_segments];
			k3d::legacy::join_edges(*new_edges[u], *edges[point_v_segments-2][u][2]);
		}

		polyhedron.faces.push_back(new k3d::legacy::face(new_edges[0], material));
	}

	for(unsigned long v = 0; v != point_v_segments; ++v)
	{
		const double phi = k3d::pi() * static_cast<double>(v) / static_cast<double>(point_v_segments - 1);
		const double varying_radius = k3d::mix(radius * 0.001, radius, sin(phi));

		double z = cos(phi);
		z = radius * k3d::sign(z) * std::pow(std::abs(z), inv_v_power);

		for(unsigned long u = 0; u != point_u_segments; ++u)
		{
			const double theta = k3d::pi_times_2() * static_cast<double>(u) / static_cast<double>(point_u_segments);

			double x = cos(theta);
			double y = -sin(theta);

			x = varying_radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
			y = varying_radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

			points[v][u]->position = k3d::point3(x, y, z);
		}
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// poly_sphere_implementation

class poly_sphere_implementation :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	poly_sphere_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("U segments")) + init_description(_("Columns")) + init_value(32) + init_constraint(constraint::minimum(3L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("V segments")) + init_description(_("Rows")) + init_value(16) + init_constraint(constraint::minimum(2L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Sphere radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_power(init_owner(*this) + init_name("u_power") + init_label(_("U power")) + init_description(_("Radial Power")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_power(init_owner(*this) + init_name("v_power") + init_label(_("V power")) + init_description(_("Length Power")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Primitive")) + init_description(_("Primitive type (sphere, quad only sphere or sphereized cylinder")) + init_value(SPHERE) + init_enumeration(type_values()))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_u_power.changed_signal().connect(make_reset_mesh_slot());
		m_v_power.changed_signal().connect(make_reset_mesh_slot());
		m_type.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const unsigned long u_segments = m_u_segments.value();
		const unsigned long v_segments = m_v_segments.value();
		const double radius = m_radius.value();
		const double u_power = m_u_power.value();
		const double v_power = m_v_power.value();
		k3d::imaterial* const material = m_material.value();

		const double inv_u_power = u_power ? 1.0 / u_power : 1.0;
		const double inv_v_power = v_power ? 1.0 / v_power : 1.0;

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		switch(m_type.value())
		{
			case SPHERE:
				detail::sphere(u_segments, v_segments, radius, inv_u_power, inv_v_power, false, material, &Mesh, polyhedron);
			break;
			case QUAD_ONLY_SPHERE:
				detail::sphere(u_segments, v_segments, radius, inv_u_power, inv_v_power, true, material, &Mesh, polyhedron);
			break;
			case SPHEREIZED_CYLINDER:
				detail::sphereized_cylinder(u_segments, v_segments, radius, inv_u_power, inv_v_power, material, &Mesh, polyhedron);
			break;
			default:
				assert_not_reached();
		}

		assert_warning(is_valid(polyhedron));
		assert_warning(is_solid(polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_sphere_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x919c3786, 0x619e4e84, 0xb4ad868f, 0x1e77e67c),
			"PolySphere",
			_("Generates a polygonal sphere"),
			"Polygons",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_u_power;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_v_power;

	/// Enumerates supported primitive types
	typedef enum
	{
		SPHERE,
		QUAD_ONLY_SPHERE,
		SPHEREIZED_CYLINDER
	} primitive_t;

	friend std::ostream& operator << (std::ostream& Stream, const primitive_t& Value)
	{
		switch(Value)
			{
				case SPHERE:
					Stream << "sphere";
					break;
				case QUAD_ONLY_SPHERE:
					Stream << "quad_only_sphere";
					break;
				case SPHEREIZED_CYLINDER:
					Stream << "sphereized_cylinder";
					break;
			}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, primitive_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "sphere")
			Value = SPHERE;
		else if(text == "quad_only_sphere")
			Value = QUAD_ONLY_SPHERE;
		else if(text == "sphereized_cylinder")
			Value = SPHEREIZED_CYLINDER;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& type_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Sphere", "sphere", "Creates a polygonal sphere"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Quad_only_sphere", "quad_only_sphere", "Creates a polygonal sphere with quads whenever possible (doesn't work with odd u_segments values"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Sphereized_cylinder", "sphereized_cylinder", "Creates a sphere topologically equivalent to a capped cylinder (there are u_segments top points and u_segments bottom points each linked by a face)"));
		}

		return values;
	}

	/// Controls the type of primitive to be created
	k3d_data(primitive_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
};

/////////////////////////////////////////////////////////////////////////////
// poly_sphere_factory

k3d::iplugin_factory& poly_sphere_factory()
{
	return poly_sphere_implementation::get_factory();
}

} // namespace libk3dprimitives


