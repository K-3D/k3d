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
// poly_disk_implementation

class poly_disk_implementation :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::node > > base;

public:
	poly_disk_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("height")) + init_description(_("Height")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Radial Segments")) + init_value(16) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("v_segments")) + init_description(_("Radial Segments")) + init_value(2) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_height.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double radius = m_radius.pipeline_value();
		const double height = m_height.pipeline_value();
		const unsigned long u_segments = m_u_segments.pipeline_value();
		const unsigned long v_segments = m_v_segments.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		// Create the disk
		const k3d::legacy::grid_results_t grid = k3d::legacy::add_grid(Mesh, polyhedron, v_segments, u_segments, false, true, material);
		const boost::multi_array<k3d::legacy::point*, 2>& points = boost::get<0>(grid);
		const boost::multi_array<k3d::legacy::split_edge*, 3>& edges = boost::get<1>(grid);

		// Shape the disk points
		for(unsigned long v = 0; v <= v_segments; ++v)
		{
			const double ring = static_cast<double>(v) / static_cast<double>(v_segments);

			for(unsigned long u = 0; u != u_segments; ++u)
			{
				const double theta = k3d::pi_times_2() * static_cast<double>(u) / static_cast<double>(u_segments);

				double x = cos(theta);
				double y = -sin(theta);
				double z = height;

				x = radius * ring * x;
				y = radius * ring * y;

				points[v][u]->position = k3d::point3(x, y, z);
			}
		}
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_disk_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xbfe3e783, 0x2a7a4f51, 0xb696858a, 0xb41d794f),
			"PolyDisk",
			_("Generates a polygonal disk"),
			"Polygon",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
};

/////////////////////////////////////////////////////////////////////////////
// poly_disk_factory

k3d::iplugin_factory& poly_disk_factory()
{
	return poly_disk_implementation::get_factory();
}

} // namespace sources

} // namespace polyhedron

} // namespace module

