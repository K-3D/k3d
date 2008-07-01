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


#include "cuda_mesh.h"

/// Constructor
cuda_polyhedra::cuda_polyhedra ( const k3d::mesh::polyhedra_t& input_polyhedra, const k3d::mesh::polyhedra_t& output_polyhedra ):
    m_p_input_polyhedra(&input_polyhedra),
    m_p_output_polyhedra(&output_polyhedra)
{
    pdev_per_polygon_first_face = 0;
    pdev_per_polygon_face_count = 0;
    pdev_per_polygon_types = 0;
    m_number_of_polygons = 0;
        
    pdev_per_face_first_loops = 0;
    pdev_per_face_loop_count = 0;
    pdev_per_face_selection = 0;
    m_number_of_faces = 0;

    pdev_per_loop_first_edge = 0;
    m_number_of_loops = 0;
        
    pdev_per_edge_point = 0;
    pdev_per_edge_clockwise_edge = 0;       
    pdev_per_edge_selection = 0;
    m_number_of_edges = 0;
    
}

/// Destructor - free the allocated device pointers
cuda_polyhedra::~cuda_polyhedra ()
{
    free_device_memory((void*)pdev_per_polygon_first_face);
    free_device_memory((void*)pdev_per_polygon_face_count);
    free_device_memory((void*)pdev_per_polygon_types);
    
    free_device_memory((void*)pdev_per_face_first_loops);
    free_device_memory((void*)pdev_per_face_loop_count);
    free_device_memory((void*)pdev_per_face_selection);
    
    free_device_memory((void*)pdev_per_loop_first_edge);
    
    free_device_memory((void*)pdev_per_edge_point);
    free_device_memory((void*)pdev_per_edge_clockwise_edge);
    free_device_memory((void*)pdev_per_edge_selection);
}   

/**
 * copy the specified polyhedra to the device
 */
void cuda_polyhedra::copy_to_device( )
{
    // set the counters
    m_number_of_polygons = m_p_input_polyhedra->first_faces->size();
    m_number_of_faces = m_p_input_polyhedra->face_first_loops->size();
    m_number_of_loops = m_p_input_polyhedra->loop_first_edges->size();
    m_number_of_edges = m_p_input_polyhedra->edge_points->size();
    
    // allocate the device memory for the polygons, faces, loops and edges
    allocate_device_memory((void**)&pdev_per_polygon_first_face, m_number_of_polygons*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_polygon_face_count, m_number_of_polygons*sizeof(k3d::uint_t));
    
    allocate_device_memory((void**)&pdev_per_polygon_types, m_number_of_polygons*sizeof(int32_t));
    
    allocate_device_memory((void**)&pdev_per_face_first_loops, m_number_of_faces*sizeof(k3d::uint_t));        
    allocate_device_memory((void**)&pdev_per_face_loop_count, m_number_of_faces*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_face_selection, m_number_of_faces*sizeof(float));
    
    allocate_device_memory((void**)&pdev_per_loop_first_edge, m_number_of_loops*sizeof(k3d::uint_t));
    
    allocate_device_memory((void**)&pdev_per_edge_point, m_number_of_edges*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_edge_clockwise_edge, m_number_of_edges*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_edge_selection, m_number_of_edges*sizeof(float));
    
    // copy the data to the device (pre-convert if needed)
    copy_from_host_to_device(pdev_per_polygon_first_face, &(m_p_input_polyhedra->first_faces->front()), m_number_of_polygons*sizeof(k3d::uint_t));
    copy_from_host_to_device(pdev_per_polygon_face_count, &(m_p_input_polyhedra->face_counts->front()), m_number_of_polygons*sizeof(k3d::uint_t));
        
    copy_from_host_to_device(pdev_per_polygon_types, &(m_p_input_polyhedra->types->front()), m_number_of_polygons*sizeof(int32_t));
    
    copy_from_host_to_device(pdev_per_face_first_loops, &(m_p_input_polyhedra->face_first_loops->front()), m_number_of_faces*sizeof(k3d::uint_t));
    copy_from_host_to_device(pdev_per_face_loop_count, &(m_p_input_polyhedra->face_loop_counts->front()), m_number_of_faces*sizeof(k3d::uint_t));
    
    float* face_selection_temp = (float*) malloc ( m_number_of_faces*sizeof(float) );
    for ( k3d::uint_t face = 0 ; face < m_number_of_faces ; ++face )
    {
        face_selection_temp[face] = (float)((*m_p_input_polyhedra->face_selection)[face]);   
    }    
    copy_from_host_to_device(pdev_per_face_selection, face_selection_temp, m_number_of_faces*sizeof(float));
    
    float* edge_selection_temp = (float*) malloc ( m_number_of_edges*sizeof(float) );
    for ( k3d::uint_t edge = 0 ; edge < m_number_of_edges ; ++edge )
    {
        edge_selection_temp[edge] = (float)((*m_p_input_polyhedra->edge_selection)[edge]);   
    }
    copy_from_host_to_device(pdev_per_edge_selection, edge_selection_temp, m_number_of_edges*sizeof(float));
    
    copy_from_host_to_device(pdev_per_loop_first_edge, &(m_p_input_polyhedra->loop_first_edges->front()), m_number_of_loops*sizeof(k3d::uint_t));
    
    copy_from_host_to_device(pdev_per_edge_point, &(m_p_input_polyhedra->edge_points->front()), m_number_of_edges*sizeof(k3d::uint_t));
    copy_from_host_to_device(pdev_per_edge_clockwise_edge, &(m_p_input_polyhedra->clockwise_edges->front()), m_number_of_edges*sizeof(k3d::uint_t));
    
    synchronize_threads();
    free ( face_selection_temp );
    free ( edge_selection_temp );      
}

/**
 * Copy the specified polyhedra from the device
 */
void cuda_polyhedra::copy_from_device( )
{
    
}

/// Constructor
cuda_device_mesh::cuda_device_mesh ( const k3d::mesh& mesh ):
    m_p_host_mesh(&mesh)
{
    pdev_points_and_selection = 0;
    m_number_of_points = 0;
}

/// Destructor - frees the allocated device pointers
cuda_device_mesh::~cuda_device_mesh()
{
    free_device_memory((void*)pdev_points_and_selection);
}

/**
 * copy the specified mesh to the device
 */
void cuda_device_mesh::copy_to_device()
{
    m_number_of_points = m_p_host_mesh->points->size(); 
    
    // allocate the memory for the points and the point selection
    allocate_device_memory((void**)&pdev_points_and_selection, (m_number_of_points)*4*sizeof(float));
    
    // convert the point coordinates and selection to floats and copy to the device
    // first allocate host memory
    float* host_points_and_selection = (float*) malloc (  m_number_of_points*4*sizeof(float) );
    
    for ( int point = 0 ; point < m_number_of_points ; ++point )
    {
        int float_index = point*3;
        host_points_and_selection[float_index] = (float)((*m_p_host_mesh->points)[point][0]);
        host_points_and_selection[float_index+1] = (float)((*m_p_host_mesh->points)[point][1]);
        host_points_and_selection[float_index+2] = (float)((*m_p_host_mesh->points)[point][2]);
        host_points_and_selection[float_index+3] = (float)((*m_p_host_mesh->point_selection)[point]);
    }
    
    // copy the coordinates and selection to the device
    copy_from_host_to_device( pdev_points_and_selection, host_points_and_selection, m_number_of_points*4*sizeof(float));
    
    // remember to also copy the device versions of polyhedra etc
    
    // make sure the copy is complete before freeing memory
    synchronize_threads();
    
    // free the temporary host data
    free ( host_points_and_selection );
}

/**
 * Copy the specified mesh from the device
 */
void cuda_device_mesh::copy_from_device( k3d::mesh& destination_mesh )
{
    k3d::mesh* p_output_mesh = &destination_mesh;
    // allocate temporary storage for the floats from the device
    float* host_points_and_selection = (float*) malloc ( m_number_of_points*4*sizeof(float) );
    
    copy_from_device_to_host( (void*) host_points_and_selection, (void*) pdev_points_and_selection, m_number_of_points*4*sizeof(float) );
    
    // check to see if the number of points has changed
    if ( m_number_of_points != p_output_mesh->points->size() )
    {
        p_output_mesh->points.reset( new k3d::mesh::points_t ( m_number_of_points ) );
        p_output_mesh->point_selection.reset ( new k3d::mesh::selection_t ( m_number_of_points ) );
    }
    
    double* out_points = (double*)&(p_output_mesh->points->front());
    double* out_selection = (double*)&(p_output_mesh->point_selection->front());
    
    synchronize_threads();
    for ( int point = 0 ; point < m_number_of_points ; ++point )
    {
        int float_index = point*4;
        int double_index = point*3;
        
        out_points[double_index] = host_points_and_selection[float_index];
        out_points[double_index+1] = host_points_and_selection[float_index+1];
        out_points[double_index+2] = host_points_and_selection[float_index+2];
        out_selection[point] = host_points_and_selection[float_index+3];
    }
    
    // remember to also copy polyhedra from the device
    synchronize_threads();    
}

