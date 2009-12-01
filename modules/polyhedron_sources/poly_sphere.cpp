// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

namespace module
{

namespace polyhedron
{

namespace sources
{

/////////////////////////////////////////////////////////////////////////////
// poly_sphere_implementation

class poly_sphere_implementation :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	poly_sphere_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("U segments")) + init_description(_("Columns")) + init_value(32) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("V segments")) + init_description(_("Rows")) + init_value(16) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Sphere radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_power(init_owner(*this) + init_name("u_power") + init_label(_("U power")) + init_description(_("Radial Power")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_power(init_owner(*this) + init_name("v_power") + init_label(_("V power")) + init_description(_("Length Power")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Primitive")) + init_description(_("Primitive type (sphere, quad only sphere or sphereized cylinder")) + init_value(SPHERE) + init_enumeration(type_values()))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_u_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_v_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_u_power.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_v_power.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_type.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		const k3d::int32_t u_segments = m_u_segments.pipeline_value();
		const k3d::int32_t v_segments = m_v_segments.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		k3d::mesh::points_t& points = Output.points.create();
		k3d::mesh::selection_t& point_selection = Output.point_selection.create();

		switch(m_type.pipeline_value())
		{
			case SPHERE:
			{
				boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));
				polyhedron->shell_first_faces.push_back(0);
				polyhedron->shell_face_counts.push_back(0);
				polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);

				// Cap the top of the cylinder with a triangle fan ...
				points.push_back(k3d::point3());
				point_selection.push_back(0);

				for(k3d::int32_t u = 0; u != u_segments; ++u)
				{
					k3d::polyhedron::add_triangle(
						Output,
						*polyhedron,
						((u + 1) % u_segments) + 1,
						((u + 0) % u_segments) + 1,
						0,
						material
						);
				}

				// Create a cylinder ...
				k3d::polyhedron::add_cylinder(Output, *polyhedron, v_segments - 2, u_segments, material);

				// Cap the bottom of the cylinder with a triangle fan ...
				points.push_back(k3d::point3());
				point_selection.push_back(0);

				for(k3d::int32_t u = 0; u != u_segments; ++u)
				{
					k3d::polyhedron::add_triangle(
						Output,
						*polyhedron,
						(((v_segments - 2) * u_segments) + (u + 0) % u_segments) + 1,
						(((v_segments - 2) * u_segments) + (u + 1) % u_segments) + 1,
						points.size() - 1,
						material
						);
				}

				break;
			}
			case QUAD_ONLY_SPHERE:
			{
				boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));
				polyhedron->shell_first_faces.push_back(0);
				polyhedron->shell_face_counts.push_back(0);
				polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);

				// Cap the top of the cylinder with quads / a triangle fan ...
				points.push_back(k3d::point3());
				point_selection.push_back(0);

				if(u_segments & 1)
				{
					for(k3d::int32_t u = 0; u != u_segments; ++u)
					{
						k3d::polyhedron::add_triangle(
							Output,
							*polyhedron,
							((u + 1) % u_segments) + 1,
							((u + 0) % u_segments) + 1,
							0,
							material
							);
					}
				}
				else
				{
					for(k3d::int32_t u = 0; u != u_segments; u += 2)
					{
						k3d::polyhedron::add_quadrilateral(
							Output,
							*polyhedron,
							((u + 2) % u_segments) + 1,
							((u + 1) % u_segments) + 1,
							((u + 0) % u_segments) + 1,
							0,
							material
							);
					}
				}

				// Create a cylinder ...
				k3d::polyhedron::add_cylinder(Output, *polyhedron, v_segments - 2, u_segments, material);

				// Cap the bottom of the cylinder with quads / a triangle fan ...
				points.push_back(k3d::point3());
				point_selection.push_back(0);

				if(u_segments & 1)
				{
					for(k3d::int32_t u = 0; u != u_segments; ++u)
					{
						k3d::polyhedron::add_triangle(
							Output,
							*polyhedron,
							(((v_segments - 2) * u_segments) + (u + 0) % u_segments) + 1,
							(((v_segments - 2) * u_segments) + (u + 1) % u_segments) + 1,
							points.size() - 1,
							material
							);
					}
				}
				else
				{
					for(k3d::int32_t u = 0; u != u_segments; u += 2)
					{
						k3d::polyhedron::add_quadrilateral(
							Output,
							*polyhedron,
							(((v_segments - 2) * u_segments) + (u + 0) % u_segments) + 1,
							(((v_segments - 2) * u_segments) + (u + 1) % u_segments) + 1,
							(((v_segments - 2) * u_segments) + (u + 2) % u_segments) + 1,
							points.size() - 1,
							material
							);
					}
				}

				break;
			}
			case SPHEREIZED_CYLINDER:
			{
				boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));
				polyhedron->shell_first_faces.push_back(0);
				polyhedron->shell_face_counts.push_back(0);
				polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);

				// Create a cylinder ...
				k3d::polyhedron::add_cylinder(Output, *polyhedron, v_segments, u_segments, material);

				// Cap the top of the cylinder with a single polygon ...
				polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
				polyhedron->face_loop_counts.push_back(1);
				polyhedron->face_selections.push_back(0);
				polyhedron->face_materials.push_back(material);
				polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());
				for(k3d::int32_t u = 0; u != u_segments; ++u)
				{
					polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
					polyhedron->edge_selections.push_back(0);

					polyhedron->vertex_points.push_back(u_segments - u - 1);
					polyhedron->vertex_selections.push_back(0);
				}
				polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();

				// Cap the bottom of the cylinder with a single polygon ...
				polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
				polyhedron->face_loop_counts.push_back(1);
				polyhedron->face_selections.push_back(0);
				polyhedron->face_materials.push_back(material);
				polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());
				for(k3d::int32_t u = 0; u != u_segments; ++u)
				{
					polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
					polyhedron->edge_selections.push_back(0);

					polyhedron->vertex_points.push_back((v_segments * u_segments) + u);
					polyhedron->vertex_selections.push_back(0);
				}
				polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();

				// Keep the polyhedron total face counts up to date ...
				polyhedron->shell_face_counts.back() = polyhedron->face_first_loops.size();
				
				break;
			}
		}
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		const k3d::int32_t u_segments = m_u_segments.pipeline_value();
		const k3d::int32_t v_segments = m_v_segments.pipeline_value();
		const k3d::double_t radius = m_radius.pipeline_value();
		const k3d::double_t u_power = m_u_power.pipeline_value();
		const k3d::double_t v_power = m_v_power.pipeline_value();
		const k3d::double_t inv_u_power = u_power ? 1.0 / u_power : 1.0;
		const k3d::double_t inv_v_power = v_power ? 1.0 / v_power : 1.0;

		k3d::mesh::points_t& points = Output.points.writable();

		switch(m_type.pipeline_value())
		{
			case SPHERE:
			case QUAD_ONLY_SPHERE:
			{
				points[0] = k3d::point3(0, 0, radius);

				for(k3d::int32_t v = 1; v < v_segments; ++v)
				{
					const k3d::double_t phi = k3d::pi() * k3d::ratio(v, v_segments);
					const k3d::double_t varying_radius = k3d::mix(radius * 0.001, radius, sin(phi));

					k3d::double_t z = cos(phi);
					z = radius * k3d::sign(z) * std::pow(std::abs(z), inv_v_power);

					for(k3d::int32_t u = 0; u != u_segments; ++u)
					{
						const k3d::double_t theta = k3d::pi_times_2() * k3d::ratio(u, u_segments);

						k3d::double_t x = cos(theta);
						k3d::double_t y = -sin(theta);

						x = varying_radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
						y = varying_radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

						points[((v - 1) * u_segments) + u + 1] = k3d::point3(x, y, z);
					}
				}

				points[points.size() - 1] = k3d::point3(0, 0, -radius);

				break;
			}
			case SPHEREIZED_CYLINDER:
			{
				for(k3d::int32_t v = 0; v <= v_segments; ++v)
				{
					const k3d::double_t phi = k3d::pi() * k3d::ratio(v, v_segments);
					const k3d::double_t varying_radius = k3d::mix(radius * 0.001, radius, sin(phi));

					k3d::double_t z = cos(phi);
					z = radius * k3d::sign(z) * std::pow(std::abs(z), inv_v_power);

					for(k3d::int32_t u = 0; u != u_segments; ++u)
					{
						const k3d::double_t theta = k3d::pi_times_2() * k3d::ratio(u, u_segments);

						k3d::double_t x = cos(theta);
						k3d::double_t y = -sin(theta);

						x = varying_radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
						y = varying_radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

						points[(v * u_segments) + u] = k3d::point3(x, y, z);
					}
				}
				break;
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_sphere_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x919c3786, 0x619e4e84, 0xb4ad868f, 0x1e77e67c),
			"PolySphere",
			_("Generates a polygonal sphere"),
			"Polygon",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_u_power;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_v_power;

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
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Quadrilateral Only Sphere", "quad_only_sphere", "Creates a polygonal sphere with quads whenever possible (doesn't work with odd u_segments values"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Sphereized Cylinder", "sphereized_cylinder", "Creates a sphere topologically equivalent to a capped cylinder (there are u_segments top points and u_segments bottom points each linked by a face)"));
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

} // namespace sources

} // namespace polyhedron

} // namespace module


