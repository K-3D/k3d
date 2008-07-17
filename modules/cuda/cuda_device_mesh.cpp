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
    \author Evan Lezar (evanlezar@gmail.com)
    \date 27 July 2008
*/


#include "cuda_device_mesh.h"

/// Constructor
cuda_device_mesh::cuda_device_mesh ( const k3d::mesh& mesh ):
    m_p_host_mesh(&mesh),
    m_cuda_device_polyhedra(*mesh.polyhedra)
{
    pdev_points_and_selection = 0;
    m_number_of_points = 0;

    pdev_additional_points = 0;
    m_number_of_additional_points = 0;
}

/// Destructor - frees the allocated device pointers
cuda_device_mesh::~cuda_device_mesh()
{
    //k3d::log() << debug << "~cuda_device_mesh" << std::endl;
    free_device_memory((void*)pdev_points_and_selection);
    free_device_memory((void*)pdev_additional_points);
}

/**
 * Set the mesh
 */
void cuda_device_mesh::set_mesh ( const k3d::mesh& input_mesh )
{
	m_p_host_mesh = &input_mesh;
	// TODO:  Set the points and polyhedra associated with the mesh
    set_points_and_selections( *input_mesh.points, *input_mesh.point_selection);
}

/**
 * Set the point and selection pointers
 */
void cuda_device_mesh::set_points_and_selections ( k3d::mesh::points_t input_points, k3d::mesh::selection_t input_selections )
{
	m_p_host_points = &input_points;
    m_number_of_points = input_points.size();
	m_p_host_selections = &input_selections;
}

/**
 *  Allocate space on the device for additional points
 */
void cuda_device_mesh::allocate_additional_points ( k3d::uint32_t number_of_new_points )
{
    if ( pdev_additional_points )
    {
        free_device_memory(pdev_additional_points);
    }

    allocate_device_memory((void**)&pdev_additional_points, number_of_new_points*4*sizeof(float));
    m_number_of_additional_points = number_of_new_points;
}

/**
 * copy the specified mesh to the device
 */
void cuda_device_mesh::copy_to_device( k3d::uint32_t what_to_copy )
{
	float* host_points_and_selection = 0;
    if ( what_to_copy & (MESH_POINTS + MESH_SELECTION) )
    {
    	bool copy_points = what_to_copy & MESH_POINTS;
        bool copy_selection = what_to_copy & MESH_SELECTION;
        m_number_of_points = m_p_host_mesh->points->size();
    	// allocate the memory for the points and the point selection
    	allocate_device_memory((void**)&pdev_points_and_selection, (m_number_of_points)*4*sizeof(float));

    	// convert the point coordinates and selection to floats and copy to the device
    	// first allocate host memory
		host_points_and_selection = (float*) malloc (  m_number_of_points*4*sizeof(float) );

		for ( int point = 0 ; point < m_number_of_points ; ++point )
		{
            int float_index = point*4;
            if ( copy_points )
            {
                host_points_and_selection[float_index] = (float)((*m_p_host_mesh->points)[point][0]);
                host_points_and_selection[float_index+1] = (float)((*m_p_host_mesh->points)[point][1]);
                host_points_and_selection[float_index+2] = (float)((*m_p_host_mesh->points)[point][2]);
            }
            if ( copy_selection )
            {
                host_points_and_selection[float_index+3] = (float)((*m_p_host_mesh->point_selection)[point]);
            }
		}

		// copy the coordinates and selection to the device
		copy_from_host_to_device((void*)pdev_points_and_selection, (const void*)host_points_and_selection, m_number_of_points*4*sizeof(float));
    }

    // remember to also copy the device versions of polyhedra etc
    if ( what_to_copy & ALL_POLYHEDRA_INFO )
    {
    	m_cuda_device_polyhedra.copy_to_device( what_to_copy );
    }

    // free the temporary host data
	synchronize_threads();
	free ( host_points_and_selection );
}

void cuda_device_mesh::copy_points_and_selections_to_device ( )
{
    float* host_points_and_selection = 0;
    
    m_number_of_points = m_p_host_mesh->points->size();
    // allocate the memory for the points and the point selection
    allocate_device_memory((void**)&pdev_points_and_selection, (m_number_of_points)*4*sizeof(float));

    // convert the point coordinates and selection to floats and copy to the device
    // first allocate host memory
    host_points_and_selection = (float*) malloc (  m_number_of_points*4*sizeof(float) );
    
    double* in_points = (double*)m_p_host_points;
    double* in_selection = (double*)m_p_host_selections;
    for ( int point = 0 ; point < m_number_of_points ; ++point )
    {
        int float_index = point*4;
        int double_index = point*3;
        host_points_and_selection[float_index] = (float)(in_points[double_index]);
        host_points_and_selection[float_index+1] = (float)(in_points[double_index+1]);
        host_points_and_selection[float_index+2] = (float)(in_points[double_index+2]);
        host_points_and_selection[float_index+3] = (float)(in_selection[point]);
    }

    // copy the coordinates and selection to the device
    copy_from_host_to_device((void*)pdev_points_and_selection, (const void*)host_points_and_selection, m_number_of_points*4*sizeof(float));
    
    synchronize_threads();
    free ( host_points_and_selection );
}

/**
 * Copy the specified mesh from the device
 */
void cuda_device_mesh::copy_from_device( k3d::mesh& destination_mesh, k3d::uint32_t what_to_copy )
{
	k3d::mesh* p_output_mesh = &destination_mesh;
	float* host_points_and_selection = 0;
	if ( what_to_copy & (MESH_POINTS + MESH_SELECTION) )
    {
    	bool copy_points = what_to_copy & MESH_POINTS;
        bool copy_selection = what_to_copy & MESH_SELECTION;

        k3d::uint32_t total_number_of_points = m_number_of_points + m_number_of_additional_points;
    	// allocate temporary storage for the floats from the device
    	host_points_and_selection = (float*) malloc ( total_number_of_points*4*sizeof(float) );

    	copy_from_device_to_host( (void*) host_points_and_selection, (void*) pdev_points_and_selection, m_number_of_points*4*sizeof(float) );
    	copy_from_device_to_host( (void*) (host_points_and_selection + m_number_of_points*4), (void*) pdev_additional_points, m_number_of_additional_points*4*sizeof(float) );
        
        double* out_points;
        double* out_selection;
        
    	if ( copy_points )
        {
            // check to see if mesh's points exist
            if ( !(p_output_mesh->points) )
            {
                boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t(total_number_of_points));
                p_output_mesh->points = points;
            }
            // check to see if the number of points has changed
            else if ( total_number_of_points  != p_output_mesh->points->size() )
            {
                p_output_mesh->points.reset( new k3d::mesh::points_t ( total_number_of_points ) );
                p_output_mesh->point_selection.reset ( new k3d::mesh::selection_t ( total_number_of_points ) );
            }
            out_points = (double*)&(p_output_mesh->points->front());
        }
        
        if ( copy_selection )
        {
    		if ( !(p_output_mesh->point_selection) )
    		{
    			boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t(total_number_of_points));
    			p_output_mesh->point_selection = point_selection;
    		}
            else if ( total_number_of_points  != p_output_mesh->point_selection->size() )
            {
                p_output_mesh->point_selection.reset ( new k3d::mesh::selection_t ( total_number_of_points ) );
            }
            out_selection = (double*)&(p_output_mesh->point_selection->front());
        }
        
		synchronize_threads();
		for ( int point = 0 ; point < total_number_of_points ; ++point )
		{
            int float_index = point*4;
			int double_index = point*3;
            if ( copy_points )
            {
                out_points[double_index] = (double)(host_points_and_selection[float_index]);
                out_points[double_index+1] = (double)(host_points_and_selection[float_index+1]);
                out_points[double_index+2] = (double)(host_points_and_selection[float_index+2]);
            }
            if ( copy_selection )
            {
                out_selection[point] = (double)(host_points_and_selection[float_index+3]);
            }
		}
    }
    // copy the polyhedra data from the device

	if ( what_to_copy & ALL_POLYHEDRA_INFO )
	{
        if ( !p_output_mesh->polyhedra )
        {
            boost::shared_ptr<const k3d::mesh::polyhedra_t> polyhedra (new k3d::mesh::polyhedra_t());
            p_output_mesh->polyhedra = polyhedra;
        }    
                
        m_cuda_device_polyhedra.copy_from_device(*(p_output_mesh->polyhedra), what_to_copy);
	}

	// TODO:  copy vertex data
    p_output_mesh->vertex_data = m_p_host_mesh->vertex_data;
    
    synchronize_threads();
    free ( host_points_and_selection );
}

/**
 * Ouput debug information for the data structure
 */
void cuda_device_mesh::output_debug_info()
{
    k3d::log() << debug << "=====================" << std::endl;
    k3d::log() << debug << "CUDA Device mesh info" << std::endl;
    k3d::log() << debug << "=====================" << std::endl;
    k3d::log() << debug << "pdev_points_and_selection: " << pdev_points_and_selection << std::endl;
    k3d::log() << debug << "m_number_of_points: " << m_number_of_points << std::endl;
    k3d::log() << debug << "pdev_additional_points: " << pdev_additional_points << std::endl;
    k3d::log() << debug << "m_number_of_additional_points: " << m_number_of_additional_points << std::endl;
    k3d::log() << debug << "m_p_host_mesh: " << m_p_host_mesh << std::endl;
    m_cuda_device_polyhedra.output_debug_info();
    k3d::log() << debug << "=====================" << std::endl;
}


cuda_device_polyhedra& cuda_device_mesh::get_device_polyhedra ()
{
    return m_cuda_device_polyhedra;
}

float* cuda_device_mesh::get_points_and_selection_pointer()
{
    return pdev_points_and_selection;
}

float* cuda_device_mesh::get_additional_points_and_selection_pointer()
{
    return pdev_additional_points;
}

k3d::uint32_t* cuda_device_mesh::get_polyhedra_edge_point_indices_pointer()
{
    return m_cuda_device_polyhedra.get_per_edge_points_pointer();
}

k3d::uint32_t* cuda_device_mesh::get_polyhedra_clockwise_edge_point_indices_pointer()
{
    return m_cuda_device_polyhedra.get_per_edge_clockwise_edges_pointer();
}

k3d::uint32_t* cuda_device_mesh::get_polyhedra_loop_first_edges_pointer()
{
    return m_cuda_device_polyhedra.get_per_loop_first_edges_pointer();
}

void cuda_device_mesh::set_device_polyhedra( const k3d::mesh::polyhedra_t& host_polyhedra )
{
    
}
