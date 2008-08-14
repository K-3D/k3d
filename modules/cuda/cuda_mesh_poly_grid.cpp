// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Evan Lezar (evanlezar@gmail.com)
*/

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>

#include "cuda_device_mesh.h"
#include "cuda_entry_points.h"

#include <iterator>

namespace module
{

namespace cuda
{

/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_poly_grid

class cuda_mesh_poly_grid :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	cuda_mesh_poly_grid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Grid width")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Grid height")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values())),
		m_p_output_device_mesh()
	{
		m_columns.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_rows.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));

		m_width.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_height.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_orientation.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
	}

	~cuda_mesh_poly_grid ()
	{
		m_p_output_device_mesh.reset();
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		document().pipeline_profiler().start_execution(*this, "Update Mesh Topology");
		k3d::uint32_t rows = m_rows.pipeline_value();
		k3d::uint32_t columns = m_columns.pipeline_value();

		m_p_output_device_mesh.reset( new cuda_device_mesh );

		// Setup some of the data structure on the host
		boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra(new k3d::mesh::polyhedra_t());
		boost::shared_ptr<k3d::mesh::indices_t> first_faces(new k3d::mesh::indices_t(1, 0));
		boost::shared_ptr<k3d::mesh::counts_t> face_counts(new k3d::mesh::counts_t(1, rows * columns));
		boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types(new k3d::mesh::polyhedra_t::types_t(1, k3d::mesh::polyhedra_t::POLYGONS));
		boost::shared_ptr<k3d::mesh::materials_t> face_materials(new k3d::mesh::materials_t(rows * columns, m_material.pipeline_value()));

		polyhedra->first_faces = first_faces;
		polyhedra->face_counts = face_counts;
		polyhedra->types = types;
		polyhedra->face_materials = face_materials;
		Output.polyhedra = polyhedra;

		m_p_output_device_mesh->resize_points_and_selection((rows+1)*(columns+1));
		m_p_output_device_mesh->get_device_polyhedra().resize_faces(rows*columns, true);
		m_p_output_device_mesh->get_device_polyhedra().resize_loops(rows*columns, true);
		m_p_output_device_mesh->get_device_polyhedra().resize_edges(4*rows*columns, true);

		synchronize_threads();

		create_grid_structure_kernel_entry (
											(unsigned int*)m_p_output_device_mesh->get_device_polyhedra().get_per_face_first_loops_pointer(),
											(unsigned int*)m_p_output_device_mesh->get_device_polyhedra().get_per_face_loop_counts_pointer(),
											(unsigned int*)m_p_output_device_mesh->get_device_polyhedra().get_per_loop_first_edges_pointer(),
											(unsigned int*)m_p_output_device_mesh->get_device_polyhedra().get_per_edge_points_pointer(),
											(unsigned int*)m_p_output_device_mesh->get_device_polyhedra().get_per_edge_clockwise_edges_pointer(),
											rows,
											columns);


		m_p_output_device_mesh->copy_from_device ( Output, POLYHEDRA_ALL_FACES + POLYHEDRA_ALL_LOOPS + POLYHEDRA_ALL_EDGES );
		document().pipeline_profiler().finish_execution(*this, "Update Mesh Topology");
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		document().pipeline_profiler().start_execution(*this, "Update Mesh Geometry");
		const float width = (float)m_width.pipeline_value();
		const float height = (float)m_height.pipeline_value();
		const k3d::signed_axis orientation = m_orientation.pipeline_value();

		k3d::uint32_t rows = m_rows.pipeline_value();
		k3d::uint32_t columns = m_columns.pipeline_value();

		float xf[3] = {0, 0, 0};
		float yf[3] = {0, 0, 0};

		switch(orientation)
		{
			case k3d::PX:
				xf[1] = -width;
				yf[2] = height;
				break;
			case k3d::NX:
				xf[1] = width;
				yf[2] = height;
				break;
			case k3d::NY:
				xf[0] = -width;
				yf[2] = height;
				break;
			case k3d::PY:
				xf[0] = width;
				yf[2] = height;
				break;
			case k3d::PZ:
				xf[0] = -width;
				yf[1] = height;
				break;
			case k3d::NZ:
				xf[0] = width;
				yf[1] = height;
				break;
		}

		calculate_grid_points_kernel_entry (m_p_output_device_mesh->get_points_and_selection_pointer(),
														xf,
														yf,
														rows,
														columns
														);
		m_p_output_device_mesh->copy_from_device ( Output, MESH_POINTS + MESH_SELECTION );

		document().pipeline_profiler().finish_execution(*this, "Update Mesh Geometry");
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_mesh_poly_grid, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xe55a0246, 0xc24a39d2, 0xe9f6b1a5, 0xe859fd69),
			"CUDAPolyGrid",
			_("Generates a polygonal grid using the CUDA API"),
			"CUDAPolygon",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_columns;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_rows;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;

	boost::shared_ptr<cuda_device_mesh> m_p_output_device_mesh;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_poly_grid_factory

k3d::iplugin_factory& cuda_mesh_poly_grid_factory()
{
	return cuda_mesh_poly_grid::get_factory();
}

} // namespace cuda

} // namespace module

