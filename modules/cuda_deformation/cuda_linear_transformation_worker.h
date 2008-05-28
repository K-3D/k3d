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
	\author Evan Lezar (evanlezar@gmail.com
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/parallel/blocked_range.h>
#include <k3dsdk/parallel/parallel_for.h>
#include <k3dsdk/parallel/threads.h>
#include <k3dsdk/idocument.h>

#include <k3dsdk/log.h>

// include the entry points as external definitions
#include "../cuda_common/cuda_entry_points.h"

namespace module
{

namespace cuda_deformation
{

/// Helper class that can apply a linear transformation to a collection of points.
/// Designed for compatibility with k3d::parallel::parallel_for().
class cuda_linear_transformation_worker
{
public:
	cuda_linear_transformation_worker(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints, const k3d::matrix4& Transformation, const k3d::idocument& Document) :
		input_points(InputPoints),
		point_selection(PointSelection),
		output_points(OutputPoints),
		transformation(Transformation),
		doc(Document)
	{
	}
		
	void operator()(const k3d::uint_t point_begin, const k3d::uint_t point_end) const
	{
		
		int num_points = point_end - point_begin;
		
		
		doc.pipeline_profiler().start_execution(*this, "");
		
		// first convert the double precision mesh points to single precision for the GPU
		// use 3 floats for the points, and a 4th for the selection weight
		// TODO:  Use CUDA to allocate host memory for assynchronous transfer 
		float *host_points_single_p = (float*) malloc ( num_points*sizeof(float)*4);
		// a 4 x 4 matrix of floats
		float *float_transformation = (float*) malloc ( 64 );
		
		float_transformation[0] = transformation[0][0];
		float_transformation[1] = transformation[0][1];
		float_transformation[2] = transformation[0][2];
		float_transformation[3] = transformation[0][3];
		float_transformation[4] = transformation[1][0];
		float_transformation[5] = transformation[1][1];
		float_transformation[6] = transformation[1][2];
		float_transformation[7] = transformation[1][3];
		float_transformation[8] = transformation[2][0];
		float_transformation[9] = transformation[2][1];
		float_transformation[10] = transformation[2][2];
		float_transformation[11] = transformation[2][3];
		float_transformation[12] = transformation[3][0];
		float_transformation[13] = transformation[3][1];
		float_transformation[14] = transformation[3][2];
		float_transformation[15] = transformation[3][3];
		
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			k3d::uint_t float_index = (point - point_begin)*4;
			host_points_single_p[float_index] = (float)input_points[point][0];
			host_points_single_p[float_index+1] = (float)input_points[point][1];
			host_points_single_p[float_index+2] = (float)input_points[point][2];
			host_points_single_p[float_index+3] = (float)point_selection[point];
			
			/*
			k3d::log() << info << input_points[point] << std::endl;
			k3d::log() << info << host_points_single_p[float_index] 
			            << ":" <<  host_points_single_p[float_index+1] 
			            << ":" <<  host_points_single_p[float_index+2]
			            << ":" <<  host_points_single_p[float_index+3]
			     		<< std::endl;
			*/
		}
		
		CUDA_initialize_device();
      	float *device_points;
		float *device_matrix;
		
    	// allocate the memory on the device - 16 bytes per point
    	allocate_device_memory((void**)&device_points, num_points*sizeof(float)*4);
    	
    	allocate_device_memory((void**)&device_matrix, 64);
		
    	// copy the data to the device
    	copy_from_host_to_device(device_points, host_points_single_p, num_points*16);
    	copy_from_host_to_device(device_matrix, float_transformation, 64);
    	
		// call the kernel to execute
		apply_linear_transform_to_point_data ( device_points, device_matrix, num_points );
		
		// copy the data from the device
    	copy_from_device_to_host(host_points_single_p, device_points, num_points*16);
		
		free_cuda_pointer(device_points);
		
		//k3d::log() << info << "OUTPUT" << std::endl;
		
		// Convert the resulting points to double precision 
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			k3d::uint_t float_index = (point - point_begin)*4;
			output_points[point][0] = host_points_single_p[float_index];
			output_points[point][1] = host_points_single_p[float_index + 1];
			output_points[point][2] = host_points_single_p[float_index + 2];
			
			/*
			k3d::log() << info << input_points[point] << std::endl;
			k3d::log() << info << host_points_single_p[float_index] 
			            << ":" <<  host_points_single_p[float_index+1] 
				        << ":" <<  host_points_single_p[float_index+2]
				        << ":" <<  host_points_single_p[float_index+3]
				     	<< std::endl;
			*/     	
				
		}
		
		free ( host_points_single_p );
	}

private:
	const k3d::mesh::points_t& input_points;
	const k3d::mesh::selection_t& point_selection;
	k3d::mesh::points_t& output_points;
	const k3d::matrix4& transformation;
	const k3d::idocument& doc;
};

} // namespace cuda_deformation

} // namespace module

