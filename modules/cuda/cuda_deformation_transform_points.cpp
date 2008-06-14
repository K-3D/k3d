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

		CUDA_initialize_device();		
		// first convert the double precision mesh points to single precision for the GPU
		// use 3 floats for the points, and a 4th for the selection weight
		// TODO:  Use CUDA to allocate host memory for assynchronous transfer 
		
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
    	
    	// use non-streamed version
    	//transform_points_synchronous ( (double *)&(InputPoints[0]), (double *)&(PointSelection[0]), (double *)&(OutputPoints[0]), num_points );
    	// use streams
		transform_points_asynchronous ( (double *)&(InputPoints[0]), (double *)&(PointSelection[0]), (double *)&(OutputPoints[0]), num_points );
		
		free_CUDA_array ( cuda_array );
		free ( float_transformation );
		document().pipeline_profiler().finish_execution(*this, "Deform Mesh");
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

} // namespace cuda

} // namespace module

