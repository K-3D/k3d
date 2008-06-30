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
cuda_polyhedra::cuda_polyhedra ( const k3d::mesh::polyhedra_t& input_polyhedra ) :
    m_polyhedra_reference(input_polyhedra)
{
    
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

void cuda_polyhedra::init_device_version()
{
    number_of_polygons = m_polyhedra_reference.first_faces->size();
    number_of_faces = m_polyhedra_reference.face_first_loops->size();
    number_of_loops = m_polyhedra_reference.loop_first_edges->size();
    number_of_edges = m_polyhedra_reference.edge_points->size();
    
    // allocate the device memory for the polygons, faces, loops and edges
    allocate_device_memory((void**)&pdev_per_polygon_first_face, number_of_polygons*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_polygon_face_count, number_of_polygons*sizeof(k3d::uint_t));
    
    allocate_device_memory((void**)&pdev_per_polygon_types, number_of_polygons*sizeof(int32_t));
    
    allocate_device_memory((void**)&pdev_per_face_first_loops, number_of_faces*sizeof(k3d::uint_t));        
    allocate_device_memory((void**)&pdev_per_face_loop_count, number_of_faces*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_face_selection, number_of_faces*sizeof(float));
    
    allocate_device_memory((void**)&pdev_per_loop_first_edge, number_of_loops*sizeof(k3d::uint_t));
    
    allocate_device_memory((void**)&pdev_per_edge_point, number_of_edges*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_edge_clockwise_edge, number_of_edges*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_edge_selection, number_of_edges*sizeof(float));
            
    // copy the data to the device (pre-convert if needed)
    copy_from_host_to_device(pdev_per_polygon_first_face, &(m_polyhedra_reference.first_faces->front()), number_of_polygons*sizeof(k3d::uint_t));
    copy_from_host_to_device(pdev_per_polygon_face_count, &(m_polyhedra_reference.face_counts->front()), number_of_polygons*sizeof(k3d::uint_t));
        
    copy_from_host_to_device(pdev_per_polygon_types, &(m_polyhedra_reference.types->front()), number_of_polygons*sizeof(int32_t));
    
    copy_from_host_to_device(pdev_per_face_first_loops, &(m_polyhedra_reference.face_first_loops->front()), number_of_faces*sizeof(k3d::uint_t));
    copy_from_host_to_device(pdev_per_face_loop_count, &(m_polyhedra_reference.face_loop_counts->front()), number_of_faces*sizeof(k3d::uint_t));
    
    float* face_selection_temp = (float*) malloc ( number_of_faces*sizeof(float) );
    for ( k3d::uint_t face = 0 ; face < number_of_faces ; ++face )
    {
        face_selection_temp[face] = (float)((*m_polyhedra_reference.face_selection)[face]);   
    }    
    copy_from_host_to_device(pdev_per_face_selection, face_selection_temp, number_of_faces*sizeof(float));
    
    float* edge_selection_temp = (float*) malloc (  number_of_edges*sizeof(float) );
    for ( k3d::uint_t edge = 0 ; edge < number_of_edges ; ++edge )
    {
        edge_selection_temp[edge] = (float)((*m_polyhedra_reference.edge_selection)[edge]);   
    }
    copy_from_host_to_device(pdev_per_edge_selection, edge_selection_temp, number_of_edges*sizeof(float));
    
    copy_from_host_to_device(pdev_per_loop_first_edge, &(m_polyhedra_reference.loop_first_edges->front()), number_of_loops*sizeof(k3d::uint_t));
    
    copy_from_host_to_device(pdev_per_edge_point, &(m_polyhedra_reference.edge_points->front()), number_of_edges*sizeof(k3d::uint_t));
    copy_from_host_to_device(pdev_per_edge_clockwise_edge, &(m_polyhedra_reference.clockwise_edges->front()), number_of_edges*sizeof(k3d::uint_t));
    
    free ( face_selection_temp );
    free ( edge_selection_temp );      
}

/// Constructor
cuda_mesh::cuda_mesh(const k3d::mesh& input_mesh):
    m_mesh_reference(input_mesh),
    m_cuda_polyhedra(*(input_mesh.polyhedra))
{
    
}

/// Destructor - frees the allocated device pointers
cuda_mesh::~cuda_mesh()
{
    free_device_memory((void*)pdev_points_and_selection);
}
 
/** 
 * Copy the mesh points and selection to the device.  Possibly allocate device memory for additional points
 * \param number_of_additional_points number of additional points for which to allocate space on the device
 */
void cuda_mesh::init_device_version(k3d::uint_t number_of_additional_points)
{
    k3d::uint_t host_number_of_points = m_mesh_reference.points->size(); 
    m_number_of_points = host_number_of_points + number_of_additional_points; 
    
    // allocate the memory for the points and the point selection
    allocate_device_memory((void**)&pdev_points_and_selection, (m_number_of_points)*4*sizeof(float));
    
    // convert the point coordinates and selection to floats and copy to the device
    // first allocate host memory
    float* host_points_and_selection = (float*) malloc (  host_number_of_points*4*sizeof(float) );
    
    for ( int point = 0 ; point < host_number_of_points ; ++point )
    {
        int float_index = point*3;
        host_points_and_selection[float_index] = (float)((*m_mesh_reference.points)[point][0]);
        host_points_and_selection[float_index+1] = (float)((*m_mesh_reference.points)[point][1]);
        host_points_and_selection[float_index+2] = (float)((*m_mesh_reference.points)[point][2]);
        host_points_and_selection[float_index+3] = (float)((*m_mesh_reference.point_selection)[point]);
    }
    
    // copy the coordinates and selection to the device
    copy_from_host_to_device( pdev_points_and_selection, host_points_and_selection, host_number_of_points*4*sizeof(float));
    
    // also init the device version of the polyhedra
    m_cuda_polyhedra.init_device_version();
    
    // free the temporary host data
    free ( host_points_and_selection );   
}

/**
 * Copy the data structure from the device.
 */
void cuda_mesh::copy_from_device( k3d::mesh& output_mesh )
{
    // allocate temporary storage for the floats from the device
    float* host_points_and_selection = (float*) malloc ( m_number_of_points*4*sizeof(float) );
    
    copy_from_device_to_host( (void*) host_points_and_selection, (void*) pdev_points_and_selection, m_number_of_points*4*sizeof(float) );
    // check to see if the number of points has changed
    if ( m_number_of_points != output_mesh.points->size() )
    {
        output_mesh.points.reset( new k3d::mesh::points_t ( m_number_of_points ) );
        output_mesh.point_selection.reset ( new k3d::mesh::selection_t ( m_number_of_points ) );
    }
    
    synchronize_threads();
    
    double* out_points = (double*)&(output_mesh.points->front());
    double* out_selection = (double*)&(output_mesh.point_selection->front());
    
    for ( int point = 0 ; point < m_number_of_points ; ++point )
    {
        int float_index = point*4;
        int double_index = point*3;
        
        out_points[double_index] = host_points_and_selection[float_index];
        out_points[double_index+1] = host_points_and_selection[float_index+1];
        out_points[double_index+2] = host_points_and_selection[float_index+2];
        out_selection[point] = host_points_and_selection[float_index+3];
    }
    
}

