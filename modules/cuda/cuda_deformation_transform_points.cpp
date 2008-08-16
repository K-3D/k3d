// K-3D
// Copyright (c) 2006, Romain Behar
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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>
#include <k3dsdk/transformable.h>

#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/log.h>

// include the entry points as external definitions
#include "cuda_entry_points.h"
#include "cuda_mesh_simple_deformation_modifier.h"

namespace module
{

namespace cuda
{

/////////////////////////////////////////////////////////////////////////////
// cuda_deformation_transform_points

class cuda_deformation_transform_points :
	public k3d::transformable<k3d::mesh_simple_deformation_modifier>
{
	typedef k3d::transformable<k3d::mesh_simple_deformation_modifier> base;

public:
	cuda_deformation_transform_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_input_matrix.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		document().pipeline_profiler().start_execution(*this, "");
		const k3d::matrix4 Transformation = m_input_matrix.pipeline_value();

		int num_points = InputPoints.size();
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

		// struct to store timing info for GPU implementation
    	timingInfo_t timing_info;
    	timing_info.numEntries = 0;
    	timing_info.timings = 0;
    	timing_info.labels = 0;

    	document().pipeline_profiler().finish_execution(*this, "BIND_TEXTURE");

    	// use non-streamed version
    	transform_points_synchronous ( (double *)&(InputPoints[0]), (double *)&(PointSelection[0]), (double *)&(OutputPoints[0]), num_points, &timing_info );

		free_CUDA_array ( cuda_array );
		free ( float_transformation );

		if ( timing_info.timings && timing_info.labels )
		{
			for ( int i = 0; i < timing_info.numEntries; i++ )
			{
				if ( timing_info.labels[i])
				{
					document().pipeline_profiler().add_timing_entry(*this, timing_info.labels[i], timing_info.timings[i]);
					free ( timing_info.labels[i] );
				}
				else
				{
					document().pipeline_profiler().add_timing_entry(*this, "", timing_info.timings[i]);
				}
			}
			free ( timing_info.labels );
			free ( timing_info.timings );
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_deformation_transform_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > > > factory(
				k3d::uuid(0x3bb9fc2b, 0x65483516, 0xd7c69198, 0x30375235),
				"CUDATransformPoints",
				_("Transform mesh points using input matrix"),
				"CUDADeformation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
};

/////////////////////////////////////////////////////////////////////////////
// cuda_deformation_transform_points_factory

k3d::iplugin_factory& cuda_deformation_transform_points_factory()
{
	return cuda_deformation_transform_points::get_factory();
}

/////////////////////////////////////////////////////////////////////////////
// cuda_deformation_transform_points_asynchronous

class cuda_deformation_transform_points_asynchronous :
	public k3d::transformable<k3d::mesh_simple_deformation_modifier>
{
	typedef k3d::transformable<k3d::mesh_simple_deformation_modifier> base;

public:
	cuda_deformation_transform_points_asynchronous(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_input_matrix.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		document().pipeline_profiler().start_execution(*this, "");
		const k3d::matrix4 Transformation = m_input_matrix.pipeline_value();

		int num_points = InputPoints.size();
		void *cuda_array = NULL;

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

		// struct to store timing info for GPU implementation
    	timingInfo_t timing_info;
    	timing_info.numEntries = 0;
    	timing_info.timings = 0;
    	timing_info.labels = 0;

    	document().pipeline_profiler().finish_execution(*this, "BIND_TEXTURE");

		// use streams
		transform_points_asynchronous ( (double *)&(InputPoints[0]), (double *)&(PointSelection[0]), (double *)&(OutputPoints[0]), num_points, &timing_info );

		free_CUDA_array ( cuda_array );
		free ( float_transformation );

		if ( timing_info.timings && timing_info.labels )
		{
			for ( int i = 0; i < timing_info.numEntries; i++ )
			{
				if ( timing_info.labels[i])
				{
					document().pipeline_profiler().add_timing_entry(*this, timing_info.labels[i], timing_info.timings[i]);
					free ( timing_info.labels[i] );
				}
				else
				{
					document().pipeline_profiler().add_timing_entry(*this, "", timing_info.timings[i]);
				}
			}
			free ( timing_info.labels );
			free ( timing_info.timings );
		}


	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_deformation_transform_points_asynchronous,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > > > factory(
				k3d::uuid(0x1cf04dc1, 0x7b443a52, 0x384baca7, 0xc84071a3),
				"CUDATransformPointsAsynchronous",
				_("Transform mesh points using input matrix"),
				"CUDADeformation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
};

/////////////////////////////////////////////////////////////////////////////
// cuda_deformation_transform_points_asynchronous_factory

k3d::iplugin_factory& cuda_deformation_transform_points_asynchronous_factory()
{
	return cuda_deformation_transform_points_asynchronous::get_factory();
}

/////////////////////////////////////////////////////////////////////////////
// cuda_deformation_transform_points_device_mesh

class cuda_deformation_transform_points_device_mesh :
	public k3d::transformable<k3d::cuda_mesh_simple_deformation_modifier>
{
	typedef k3d::transformable<k3d::cuda_mesh_simple_deformation_modifier> base;

public:
	cuda_deformation_transform_points_device_mesh(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_input_matrix.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(k3d::mesh& Output)
	{
		k3d::log() << debug << "on_deform_mesh" << std::endl;
		document().pipeline_profiler().start_execution(*this, "");
		const k3d::matrix4 Transformation = m_input_matrix.pipeline_value();

		int num_points = m_p_input_device_mesh.pipeline_value()->get_number_of_points();
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

    	document().pipeline_profiler().finish_execution(*this, "BIND_TEXTURE");

    	document().pipeline_profiler().start_execution(*this, "Kernel Call");
    	// use the implementation that uses the device mesh
    	transform_points_device_mesh (m_p_input_device_mesh.pipeline_value()->get_points_and_selection_pointer() , num_points);
    	document().pipeline_profiler().finish_execution(*this, "Kernel Call");

    	document().pipeline_profiler().start_execution(*this, "Free Memory");
		free_CUDA_array ( cuda_array );
		free ( float_transformation );
		document().pipeline_profiler().finish_execution(*this, "Free Memory");

		document().pipeline_profiler().start_execution(*this, "Copy From Device");

		Output.points.reset();
		Output.point_selection.reset();
		m_p_input_device_mesh.pipeline_value()->copy_from_device(Output, MESH_POINTS+MESH_SELECTION);

		document().pipeline_profiler().finish_execution(*this, "Copy From Device");

	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_deformation_transform_points_device_mesh,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > > > factory(
				k3d::uuid(0x2ef86b2e, 0x2449d8a8, 0xbac438a5, 0xffeb92c8),
				"CUDATransformPointsDeviceMesh",
				_("Transform mesh points using input matrix using a device mesh"),
				"CUDADeformation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// cuda_deformation_transform_points_device_mesh_factory

k3d::iplugin_factory& cuda_deformation_transform_points_device_mesh_factory()
{
	return cuda_deformation_transform_points_device_mesh::get_factory();
}

} // namespace cuda

} // namespace module

