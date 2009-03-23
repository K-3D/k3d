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
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/node.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>

#include <iterator>

namespace module
{

namespace polyhedron
{

namespace sources
{

/////////////////////////////////////////////////////////////////////////////
// poly_cone_implementation

class poly_cone_implementation :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::node > > base;

public:
	poly_cone_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("U segments")) + init_description(_("Columns")) + init_value(32) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("V segments")) + init_description(_("Rows")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_bottom(init_owner(*this) + init_name("bottom") + init_label(_("Bottom")) + init_description(_("Cap cone bottom")) + init_value(true)),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Bottom radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Z Max (RenderMan convention)")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_power(init_owner(*this) + init_name("u_power") + init_label(_("U power")) + init_description(_("Radial power")) + init_value(1.0) + init_step_increment(0.1)),
		m_v_power(init_owner(*this) + init_name("v_power") + init_label(_("V power")) + init_description(_("Length power")) + init_value(1.0) + init_step_increment(0.1))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
		m_bottom.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_height.changed_signal().connect(make_reset_mesh_slot());
		m_u_power.changed_signal().connect(make_reset_mesh_slot());
		m_v_power.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double radius = m_radius.pipeline_value();
		const double height = m_height.pipeline_value();
		const double u_power = m_u_power.pipeline_value();
		const double v_power = m_v_power.pipeline_value();
		const double inv_u_power = u_power ? 1.0 / u_power : 1.0;
		const bool bottom = m_bottom.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		// Create the cone body ...
		const k3d::legacy::grid_results_t grid = k3d::legacy::add_grid(Mesh, polyhedron, m_v_segments.pipeline_value(), m_u_segments.pipeline_value(), false, true, material);
		const boost::multi_array<k3d::legacy::point*, 2>& points = boost::get<0>(grid);
		const boost::multi_array<k3d::legacy::split_edge*, 3>& edges = boost::get<1>(grid);

		const unsigned long point_v_segments = points.shape()[0];
		const unsigned long point_u_segments = points.shape()[1];

		// Ensure that the top of the cone is "closed" topologically ...
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

		// Optionally cap the bottom of the cone ...
		if(bottom)
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

		// Shape the cone points
		for(unsigned long v = 0; v != point_v_segments; ++v)
		{
			const double percent = static_cast<double>(v) / static_cast<double>(point_v_segments - 1);
			const double varying_radius = k3d::mix(0.001 * radius, radius, percent);

			for(unsigned long u = 0; u != point_u_segments; ++u)
			{
				const double theta = k3d::pi_times_2() * static_cast<double>(u) / static_cast<double>(point_u_segments);

				double x = cos(theta);
				double y = -sin(theta);
				double z = k3d::mix(height, 0.0, std::pow(percent, v_power));

				x = varying_radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
				y = varying_radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

				points[v][u]->position = k3d::point3(x, y, z);
			}
		}

		assert_warning(is_valid(polyhedron));
		if(bottom)
			assert_warning(is_solid(polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_cone_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x7c41b43f, 0x385143ab, 0x86969795, 0x368d6199),
			"PolyCone",
			_("Generates a polygonal cone with optional endcap"),
			"Polygon",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bottom;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_u_power;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v_power;
};

/////////////////////////////////////////////////////////////////////////////
// poly_cone_factory

k3d::iplugin_factory& poly_cone_factory()
{
	return poly_cone_implementation::get_factory();
}

} // namespace sources

} // namespace polyhedron

} // namespace module

