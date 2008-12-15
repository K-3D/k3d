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
// cuda_mesh_poly_grid_and_transform

class cuda_mesh_poly_grid_and_transform :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	cuda_mesh_poly_grid_and_transform(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Grid width")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Grid height")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values())),
		m_input_matrix(init_owner(*this) + init_name("input_matrix") + init_label(_("Input matrix")) + init_description(_("Input matrix")) + init_value(k3d::identity3())),
		m_p_output_device_mesh(init_owner(*this) + init_name("ouput_device_mesh") + init_label(_("Output Device Mesh")) + init_description(_("Pointer to device mesh")) + init_value<cuda_device_mesh*>(0))
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

		m_input_matrix.changed_signal().connect(make_update_mesh_slot());
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		document().pipeline_profiler().start_execution(*this, "Update Topology:init");
		k3d::uint32_t rows = m_rows.pipeline_value();
		k3d::uint32_t columns = m_columns.pipeline_value();

		// Setup some of the data structure on the host
		k3d::mesh::polyhedra_t& polyhedra = Output.polyhedra.create(new k3d::mesh::polyhedra_t());
		k3d::mesh::indices_t& first_faces = polyhedra.first_faces.create(new k3d::mesh::indices_t(1, 0));
		k3d::mesh::counts_t& face_counts = polyhedra.face_counts.create(new k3d::mesh::counts_t(1, rows * columns));
		k3d::mesh::polyhedra_t::types_t& types = polyhedra.types.create(new k3d::mesh::polyhedra_t::types_t(1, k3d::mesh::polyhedra_t::POLYGONS));
		k3d::mesh::materials_t& face_materials = polyhedra.face_materials.create(new k3d::mesh::materials_t(rows * columns, m_material.pipeline_value()));

		document().pipeline_profiler().finish_execution(*this, "Update Topology:init");

		document().pipeline_profiler().start_execution(*this, "Update Topology:DeviceAllocate");

		m_p_output_device_mesh.pipeline_value()->resize_points_and_selection((rows+1)*(columns+1));
		m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().resize_faces(rows*columns, true);
		m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().resize_loops(rows*columns, true);
		m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().resize_edges(4*rows*columns, true);

		synchronize_threads();
		document().pipeline_profiler().finish_execution(*this, "Update Topology:DeviceAllocate");

		document().pipeline_profiler().start_execution(*this, "Update Topology:Kernel");
		create_grid_structure_kernel_entry (
											(unsigned int*)m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().get_per_face_first_loops_pointer(),
											(unsigned int*)m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().get_per_face_loop_counts_pointer(),
											(unsigned int*)m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().get_per_loop_first_edges_pointer(),
											(unsigned int*)m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().get_per_edge_points_pointer(),
											(unsigned int*)m_p_output_device_mesh.pipeline_value()->get_device_polyhedra().get_per_edge_clockwise_edges_pointer(),
											rows,
											columns);
		document().pipeline_profiler().finish_execution(*this, "Update Topology:Kernel");

		document().pipeline_profiler().start_execution(*this, "Update Topology:Copy");
		m_p_output_device_mesh.pipeline_value()->copy_from_device ( Output, POLYHEDRA_ALL_FACES + POLYHEDRA_ALL_LOOPS + POLYHEDRA_ALL_EDGES );
		document().pipeline_profiler().finish_execution(*this, "Update Topology:Copy");
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		document().pipeline_profiler().start_execution(*this, "Update Geometry:init");
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
		document().pipeline_profiler().finish_execution(*this, "Update Geometry:init");

		document().pipeline_profiler().start_execution(*this, "Update Geometry:Kernel");
		calculate_grid_points_kernel_entry (m_p_output_device_mesh.pipeline_value()->get_points_and_selection_pointer(),
														xf,
														yf,
														rows,
														columns
														);
		document().pipeline_profiler().finish_execution(*this, "Update Geometry:Kernel");

		document().pipeline_profiler().start_execution(*this, "Update Geometry:Transform Points");

		m_p_output_device_mesh.pipeline_value()->select_all_points();
		const k3d::matrix4 Transformation = m_input_matrix.pipeline_value();

		int num_points = m_p_output_device_mesh.pipeline_value()->get_number_of_points();
		void *cuda_array = NULL;

		// first convert the double precision mesh points to single precision for the GPU
		// use 3 floats for the points, and a 4th for the selection weight
		// a 4 x 4 matrix of floats
		float *float_transformation = (float*) malloc ( 64 );

		float_transformation[0] = Transformation[0][0];
		float_transformation[1] = Transformation[0][1];
		float_transformation[2] = Transformation[0][2];
		float_transformation[3] = Transformation[0][3];
		float_transformation[4] = Transformation[1][0];
		float_transformation[5] = Transformation[1][1];
		float_transformation[6] = Transformation[1][2];
		float_transformation[7] = Transformation[1][3];
		float_transformation[8] = Transformation[2][0];
		float_transformation[9] = Transformation[2][1];
		float_transformation[10] = Transformation[2][2];
		float_transformation[11] = Transformation[2][3];
		float_transformation[12] = Transformation[3][0];
		float_transformation[13] = Transformation[3][1];
		float_transformation[14] = Transformation[3][2];
		float_transformation[15] = Transformation[3][3];
		copy_and_bind_texture_to_array( &cuda_array, float_transformation, 4, 4 );

		// use the implementation that uses the device mesh
		transform_points_device_mesh (m_p_output_device_mesh.pipeline_value()->get_points_and_selection_pointer() , num_points);


		free_CUDA_array ( cuda_array );
		free ( float_transformation );
		document().pipeline_profiler().finish_execution(*this, "Update Geometry:Transform Points");


		document().pipeline_profiler().start_execution(*this, "Update Geometry:Copy");
		m_p_output_device_mesh.pipeline_value()->copy_from_device ( Output, MESH_POINTS + MESH_SELECTION );
		document().pipeline_profiler().finish_execution(*this, "Update Geometry:Copy");
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_mesh_poly_grid_and_transform, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x37a67c9d, 0x504dae59, 0xa4967c95, 0xdb261ca6),
			"CUDAPolyGridAndTransformPoints",
			_("Generates a polygonal grid and applies a transform points filter using the CUDA API"),
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
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input_matrix;

	k3d_data(cuda_device_mesh*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_p_output_device_mesh;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_poly_grid_and_transform_factory

k3d::iplugin_factory& cuda_mesh_poly_grid_and_transform_factory()
{
	return cuda_mesh_poly_grid_and_transform::get_factory();
}

} // namespace cuda

} // namespace module

