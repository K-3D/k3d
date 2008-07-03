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
cuda_device_polyhedra::cuda_device_polyhedra ( const k3d::mesh::polyhedra_t& host_polyhedra ):
    m_p_input_polyhedra(&host_polyhedra)
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
    
    // initialize additional storage pointers
    pdev_additional_per_polygon_first_face = 0;
    pdev_additional_per_polygon_face_count = 0;
    pdev_additional_per_polygon_types = 0;
    m_number_of_additional_polyhedra = 0;
    
    pdev_additional_per_face_first_loops = 0;
    pdev_additional_per_face_loop_count = 0;
    pdev_additional_per_face_selection = 0;
    m_number_of_additional_faces = 0;
    
    pdev_additional_per_loop_first_edge = 0;
    m_number_of_additional_loops = 0;
    
    pdev_additional_per_edge_point = 0;
    pdev_additional_per_edge_clockwise_edge = 0;       
    pdev_additional_per_edge_selection = 0;
    m_number_of_additional_edges = 0;
    
}

/// Destructor - free the allocated device pointers
cuda_device_polyhedra::~cuda_device_polyhedra ()
{
    k3d::log() << debug << "~cuda_device_polyhedra" << std::endl;
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
    
    // free the memory allocated for additional structures
    free_device_memory(pdev_additional_per_polygon_face_count);    
    free_device_memory(pdev_additional_per_polygon_first_face);
    free_device_memory(pdev_additional_per_polygon_types);
    
    free_device_memory ( pdev_additional_per_face_first_loops );   
    free_device_memory(pdev_additional_per_face_loop_count);   
    free_device_memory(pdev_additional_per_face_selection);
    
    free_device_memory(pdev_additional_per_loop_first_edge);
    
    free_device_memory(pdev_additional_per_edge_point);   
    free_device_memory(pdev_additional_per_edge_clockwise_edge);    
    free_device_memory(pdev_additional_per_edge_selection);    
}   

/**
 * Allocate device memory for additional polyhedra components
 */ 
void cuda_device_polyhedra::allocate_additional_polyhedra ( k3d::uint_t number_of_additional_polyhedra )
{
    if ( pdev_additional_per_polygon_face_count )
    {
        free_device_memory(pdev_additional_per_polygon_face_count);    
    }
    if ( pdev_additional_per_polygon_first_face )
    {
        free_device_memory(pdev_additional_per_polygon_first_face);
    }
    if ( pdev_additional_per_polygon_types )
    {
        free_device_memory(pdev_additional_per_polygon_types);
    }
    
    allocate_device_memory((void**)&pdev_additional_per_polygon_face_count, number_of_additional_polyhedra*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_additional_per_polygon_first_face, number_of_additional_polyhedra*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_additional_per_polygon_types, number_of_additional_polyhedra*sizeof(k3d::int32_t));
    
    m_number_of_additional_polyhedra = number_of_additional_polyhedra;        
}

void cuda_device_polyhedra::allocate_additional_faces ( k3d::uint_t number_of_additional_faces )
{
    if ( pdev_additional_per_face_first_loops )
    {
        free_device_memory ( pdev_additional_per_face_first_loops );   
    }
    if ( pdev_additional_per_face_loop_count )
    {
        free_device_memory(pdev_additional_per_face_loop_count);   
    }
    if ( pdev_additional_per_face_selection )
    {
        free_device_memory(pdev_additional_per_face_selection);
    }
    
    allocate_device_memory((void**)&pdev_additional_per_face_first_loops, number_of_additional_faces*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_additional_per_face_loop_count, number_of_additional_faces*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_additional_per_face_selection, number_of_additional_faces*sizeof(float));
    m_number_of_additional_faces = number_of_additional_faces;
    
}
void cuda_device_polyhedra::allocate_additional_loops ( k3d::uint_t number_of_additional_loops )
{
    if ( pdev_additional_per_loop_first_edge )
    {
        free_device_memory(pdev_additional_per_loop_first_edge);
    }
    
    allocate_device_memory((void**)&pdev_additional_per_loop_first_edge, number_of_additional_loops*sizeof(k3d::uint_t));
    m_number_of_additional_loops = number_of_additional_loops;   
}
void cuda_device_polyhedra::allocate_additional_edges ( k3d::uint_t number_of_additional_edges )
{
    if ( pdev_additional_per_edge_point )
    {
        free_device_memory(pdev_additional_per_edge_point);   
    }
    if ( pdev_additional_per_edge_clockwise_edge )
    {
        free_device_memory(pdev_additional_per_edge_clockwise_edge);    
    }       
    if ( pdev_additional_per_edge_selection )
    {
        free_device_memory(pdev_additional_per_edge_selection);    
    }
    
    allocate_device_memory((void**)&pdev_additional_per_edge_point, number_of_additional_edges*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_additional_per_edge_clockwise_edge, number_of_additional_edges*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_additional_per_edge_selection, number_of_additional_edges*sizeof(float));
    m_number_of_additional_edges = number_of_additional_edges;
    
}

/**
 * copy the specified polyhedra to the device
 */
void cuda_device_polyhedra::copy_to_device( )
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
 * Copy the polyhedra from the device
 */
void cuda_device_polyhedra::copy_from_device( k3d::mesh::polyhedra_t& destination_polyhedra )
{
    k3d::log() << debug << "cuda_device_polyhedra::copy_from_device" << std::endl;
    
    k3d::mesh::polyhedra_t* p_output_polyhedra = &destination_polyhedra;

    // allocate memory for the float to double conversion
    float* face_selection_temp = (float*) malloc ( m_number_of_faces*sizeof(float) );
    copy_from_device_to_host(face_selection_temp, pdev_per_face_selection, m_number_of_faces*sizeof(float));
    float* edge_selection_temp = (float*) malloc ( m_number_of_edges*sizeof(float) );
    copy_from_device_to_host(edge_selection_temp, pdev_per_edge_selection, m_number_of_edges*sizeof(float));
    
    boost::shared_ptr<k3d::mesh::indices_t> first_faces ( new k3d::mesh::indices_t ( m_number_of_polygons ) );
    boost::shared_ptr<k3d::mesh::counts_t> face_counts ( new k3d::mesh::counts_t ( m_number_of_polygons ) );
    boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types ( new k3d::mesh::polyhedra_t::types_t ( m_number_of_polygons ) );
    boost::shared_ptr<k3d::mesh::indices_t> face_first_loops ( new k3d::mesh::indices_t ( m_number_of_faces ) );
    boost::shared_ptr<k3d::mesh::counts_t> face_loop_counts ( new k3d::mesh::counts_t ( m_number_of_faces ) );
    boost::shared_ptr<k3d::mesh::selection_t> face_selection ( new k3d::mesh::selection_t ( m_number_of_faces ) );
    boost::shared_ptr<k3d::mesh::indices_t> loop_first_edges ( new k3d::mesh::indices_t ( m_number_of_loops ) );
    boost::shared_ptr<k3d::mesh::indices_t> edge_points ( new k3d::mesh::indices_t ( m_number_of_edges ) );
    boost::shared_ptr<k3d::mesh::indices_t> clockwise_edges ( new k3d::mesh::indices_t ( m_number_of_edges ) );
    boost::shared_ptr<k3d::mesh::selection_t> edge_selection ( new k3d::mesh::selection_t ( m_number_of_edges ) );
    
    copy_from_device_to_host(&(first_faces->front()), pdev_per_polygon_first_face, m_number_of_polygons*sizeof(k3d::uint_t));
    copy_from_device_to_host(&(face_counts->front()), pdev_per_polygon_face_count, m_number_of_polygons*sizeof(k3d::uint_t));
    
    copy_from_device_to_host(&(types->front()), pdev_per_polygon_types, m_number_of_polygons*sizeof(int32_t));
    
    double* out_selection = (double*)&(face_selection->front());
    for ( k3d::uint_t face = 0 ; face < m_number_of_faces ; ++face )
    {
        (out_selection)[face] = (double)face_selection_temp[face];   
    }
    
    copy_from_device_to_host(&(face_first_loops->front()), pdev_per_face_first_loops, m_number_of_faces*sizeof(k3d::uint_t));
    copy_from_device_to_host(&(face_loop_counts->front()), pdev_per_face_loop_count, m_number_of_faces*sizeof(k3d::uint_t));
    
    copy_from_device_to_host(&(loop_first_edges->front()), pdev_per_loop_first_edge, m_number_of_loops*sizeof(k3d::uint_t));
    
    out_selection = (double*)&(edge_selection->front());
    for ( k3d::uint_t edge = 0 ; edge < m_number_of_edges ; ++edge )
    {
        (out_selection)[edge] = (double) edge_selection_temp[edge];   
    }
    
    copy_from_device_to_host(&(edge_points->front()), pdev_per_edge_point, m_number_of_edges*sizeof(k3d::uint_t));
    copy_from_device_to_host(&(clockwise_edges->front()), pdev_per_edge_clockwise_edge, m_number_of_edges*sizeof(k3d::uint_t));
    
    p_output_polyhedra->first_faces = first_faces;
    p_output_polyhedra->face_counts = face_counts;   
    p_output_polyhedra->types = types;       
    p_output_polyhedra->face_first_loops = face_first_loops;    
    p_output_polyhedra->face_loop_counts = face_loop_counts;   
    p_output_polyhedra->face_selection = face_selection;   
    p_output_polyhedra->loop_first_edges = loop_first_edges;           
    p_output_polyhedra->edge_points = edge_points;           
    p_output_polyhedra->clockwise_edges = clockwise_edges;           
    p_output_polyhedra->edge_selection = edge_selection;   
    
    // TODO:  constant_data, face_materlials, face_varying_data
    p_output_polyhedra->face_materials = m_p_input_polyhedra->face_materials;
    p_output_polyhedra->constant_data = m_p_input_polyhedra->constant_data;
    p_output_polyhedra->uniform_data = m_p_input_polyhedra->uniform_data;    
    p_output_polyhedra->face_varying_data = m_p_input_polyhedra->face_varying_data;
    
    synchronize_threads();
    free ( face_selection_temp );
    free ( edge_selection_temp );
    k3d::log() << debug << "cuda_device_polyhedra::copy_from_device::end" << std::endl;
}

/**
 * Ouput debug information for the data structure
 */
void cuda_device_polyhedra::output_debug_info()
{
    k3d::log() << debug << "=====================" << std::endl;
    k3d::log() << debug << "CUDA Device polyhedra info" << std::endl;
    k3d::log() << debug << "=====================" << std::endl;
    k3d::log() << debug << "pdev_per_polygon_first_face: " << pdev_per_polygon_first_face << std::endl;
    k3d::log() << debug << "pdev_per_polygon_face_count: " << pdev_per_polygon_face_count << std::endl;
    k3d::log() << debug << "pdev_per_polygon_types: " << pdev_per_polygon_types << std::endl;
    k3d::log() << debug << "m_number_of_polygons: " << m_number_of_polygons << std::endl;
               
    k3d::log() << debug << "pdev_per_face_first_loops: " << pdev_per_face_first_loops << std::endl;
    k3d::log() << debug << "pdev_per_face_loop_count: " << pdev_per_face_loop_count << std::endl;
    k3d::log() << debug << "pdev_per_face_selection: " << pdev_per_face_selection << std::endl;
    k3d::log() << debug << "m_number_of_faces: " << m_number_of_faces << std::endl;             

    k3d::log() << debug << "pdev_per_loop_first_edge: " << pdev_per_loop_first_edge << std::endl;
    k3d::log() << debug << "m_number_of_loops: " << m_number_of_loops << std::endl;
    
    k3d::log() << debug << "pdev_per_edge_point: " << pdev_per_face_first_loops << std::endl;
    k3d::log() << debug << "pdev_per_edge_clockwise_edge: " << pdev_per_face_loop_count << std::endl;
    k3d::log() << debug << "pdev_per_edge_selection: " << pdev_per_face_selection << std::endl;
    k3d::log() << debug << "m_number_of_edges: " << m_number_of_faces << std::endl;        
    
    k3d::log() << debug << "m_p_input_polyhedra: " << m_p_input_polyhedra << std::endl;        
    k3d::log() << debug << "=====================" << std::endl;
}

/// Constructor
cuda_device_mesh::cuda_device_mesh ( const k3d::mesh& mesh ):
    m_p_host_mesh(&mesh),
    m_cuda_device_polyhedra(*mesh.polyhedra)
{
    pdev_points_and_selection = 0;
    m_number_of_points = 0;
}

/// Destructor - frees the allocated device pointers
cuda_device_mesh::~cuda_device_mesh()
{
    k3d::log() << debug << "~cuda_device_mesh" << std::endl;
    free_device_memory((void*)pdev_points_and_selection);
    free_device_memory((void*)pdev_additional_points);
}

/**
 *  Allocate space on the device for additional points
 */
void cuda_device_mesh::allocate_additional_points ( k3d::uint_t number_of_new_points )
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
void cuda_device_mesh::copy_to_device()
{
    k3d::log() << debug << "cuda_device_mesh::copy_to_device" << std::endl;
    m_number_of_points = m_p_host_mesh->points->size(); 
    
    // allocate the memory for the points and the point selection
    allocate_device_memory((void**)&pdev_points_and_selection, (m_number_of_points)*4*sizeof(float));
    
    // convert the point coordinates and selection to floats and copy to the device
    // first allocate host memory
    float* host_points_and_selection = (float*) malloc (  m_number_of_points*4*sizeof(float) );
    
    for ( int point = 0 ; point < m_number_of_points ; ++point )
    {
        int float_index = point*4;
        host_points_and_selection[float_index] = (float)((*m_p_host_mesh->points)[point][0]);
        host_points_and_selection[float_index+1] = (float)((*m_p_host_mesh->points)[point][1]);
        host_points_and_selection[float_index+2] = (float)((*m_p_host_mesh->points)[point][2]);
        host_points_and_selection[float_index+3] = (float)((*m_p_host_mesh->point_selection)[point]);
    }
    
    // copy the coordinates and selection to the device
    copy_from_host_to_device( pdev_points_and_selection, host_points_and_selection, m_number_of_points*4*sizeof(float));
    
    // remember to also copy the device versions of polyhedra etc
    m_cuda_device_polyhedra.copy_to_device();
    
    // make sure the copy is complete before freeing memory
    synchronize_threads();
    
    // free the temporary host data
    free ( host_points_and_selection );
    k3d::log() << debug << "cuda_device_mesh::copy_to_device::end" << std::endl;
}

/**
 * Copy the specified mesh from the device
 */
void cuda_device_mesh::copy_from_device( k3d::mesh& destination_mesh )
{
    k3d::log() << debug << "cuda_device_mesh::copy_from_device" << std::endl;

    k3d::mesh* p_output_mesh = &destination_mesh;
    
    // allocate temporary storage for the floats from the device
    float* host_points_and_selection = (float*) malloc ( m_number_of_points*4*sizeof(float) );
    
    copy_from_device_to_host( (void*) host_points_and_selection, (void*) pdev_points_and_selection, m_number_of_points*4*sizeof(float) );
        
    // check to see if mesh's points and point selection exists
    if ( !(p_output_mesh->points) )
    {
        boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t( m_number_of_points ));
        boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t(m_number_of_points));
        p_output_mesh->points = points;
        p_output_mesh->point_selection = point_selection;               
    }
    // check to see if the number of points has changed
    else if ( m_number_of_points != p_output_mesh->points->size() )
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
        
        out_points[double_index] = (double)(host_points_and_selection[float_index]);
        out_points[double_index+1] = (double)(host_points_and_selection[float_index+1]);
        out_points[double_index+2] = (double)(host_points_and_selection[float_index+2]);
        out_selection[point] = (double)(host_points_and_selection[float_index+3]);
    }
    
    // copy the polyhedra data from the device
    boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra (new k3d::mesh::polyhedra_t() );
    m_cuda_device_polyhedra.copy_from_device(*(polyhedra));
    
    p_output_mesh->polyhedra = polyhedra;
    // TODO:  copy vertex data
    p_output_mesh->vertex_data = m_p_host_mesh->vertex_data;
    
    synchronize_threads();    
    k3d::log() << debug << "cuda_device_mesh::copy_from_device::end" << std::endl;
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