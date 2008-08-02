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
    \date 14 July 2008
*/

#include "cuda_device_polyhedra.h"

inline void copy_from_host_to_device_uint_64_bit_check ( void* device_pointer, const void* host_pointer, int size_in_bytes )
{
#ifdef K3D_UINT_T_64_BITS
    copy_from_host_to_device_64_to_32_convert (device_pointer, host_pointer, size_in_bytes );
#else
    copy_from_host_to_device (device_pointer, host_pointer, size_in_bytes );
#endif
}

inline void copy_from_device_to_host_uint_64_bit_check ( void* host_pointer, const void* device_pointer, int size_in_bytes )
{
#ifdef K3D_UINT_T_64_BITS
	copy_from_device_to_host_32_to_64_convert (host_pointer, device_pointer, size_in_bytes );
#else
  copy_from_device_to_host (host_pointer, device_pointer, size_in_bytes );
#endif
}

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

}

/// Destructor - free the allocated device pointers
cuda_device_polyhedra::~cuda_device_polyhedra ()
{
    //k3d::log() << debug << "~cuda_device_polyhedra" << std::endl;
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
void cuda_device_polyhedra::copy_to_device( k3d::uint32_t what_to_copy  )
{
    if ( what_to_copy & POLYHEDRA_ALL_POLYGONS )
    {
        m_number_of_polygons = m_p_input_polyhedra->first_faces->size();
        allocate_device_memory((void**)&pdev_per_polygon_first_face, m_number_of_polygons*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_per_polygon_face_count, m_number_of_polygons*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_per_polygon_types, m_number_of_polygons*sizeof(int32_t));

        copy_from_host_to_device_uint_64_bit_check(pdev_per_polygon_first_face, &(m_p_input_polyhedra->first_faces->front()), m_number_of_polygons*sizeof(k3d::uint32_t));
        copy_from_host_to_device_uint_64_bit_check(pdev_per_polygon_face_count, &(m_p_input_polyhedra->face_counts->front()), m_number_of_polygons*sizeof(k3d::uint32_t));
        copy_from_host_to_device(pdev_per_polygon_types, &(m_p_input_polyhedra->types->front()), m_number_of_polygons*sizeof(int32_t));
    }

    float* face_selection_temp = 0;
    if ( what_to_copy & POLYHEDRA_ALL_FACES )
    {
        m_number_of_faces = m_p_input_polyhedra->face_first_loops->size();
        allocate_device_memory((void**)&pdev_per_face_first_loops, m_number_of_faces*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_per_face_loop_count, m_number_of_faces*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_per_face_selection, m_number_of_faces*sizeof(float));

        copy_from_host_to_device_uint_64_bit_check(pdev_per_face_first_loops, &(m_p_input_polyhedra->face_first_loops->front()), m_number_of_faces*sizeof(k3d::uint32_t));
        copy_from_host_to_device_uint_64_bit_check(pdev_per_face_loop_count, &(m_p_input_polyhedra->face_loop_counts->front()), m_number_of_faces*sizeof(k3d::uint32_t));

        face_selection_temp = (float*) malloc ( m_number_of_faces*sizeof(float) );
        for ( k3d::uint32_t face = 0 ; face < m_number_of_faces ; ++face )
        {
            face_selection_temp[face] = (float)((*m_p_input_polyhedra->face_selection)[face]);
        }
        copy_from_host_to_device(pdev_per_face_selection, face_selection_temp, m_number_of_faces*sizeof(float));

    }

    if ( what_to_copy & POLYHEDRA_ALL_LOOPS )
    {
        m_number_of_loops = m_p_input_polyhedra->loop_first_edges->size();
        allocate_device_memory((void**)&pdev_per_loop_first_edge, m_number_of_loops*sizeof(k3d::uint32_t));
        copy_from_host_to_device_uint_64_bit_check(pdev_per_loop_first_edge, &(m_p_input_polyhedra->loop_first_edges->front()), m_number_of_loops*sizeof(k3d::uint32_t));
    }

    float* edge_selection_temp = 0;
    if ( what_to_copy & POLYHEDRA_ALL_EDGES )
    {
        m_number_of_edges = m_p_input_polyhedra->edge_points->size();

        allocate_device_memory((void**)&pdev_per_edge_point, m_number_of_edges*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_per_edge_clockwise_edge, m_number_of_edges*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_per_edge_selection, m_number_of_edges*sizeof(float));

        copy_from_host_to_device_uint_64_bit_check(pdev_per_edge_point, &(m_p_input_polyhedra->edge_points->front()), m_number_of_edges*sizeof(k3d::uint32_t));
        copy_from_host_to_device_uint_64_bit_check(pdev_per_edge_clockwise_edge, &(m_p_input_polyhedra->clockwise_edges->front()), m_number_of_edges*sizeof(k3d::uint32_t));

        edge_selection_temp = (float*) malloc ( m_number_of_edges*sizeof(float) );
        for ( k3d::uint32_t edge = 0 ; edge < m_number_of_edges ; ++edge )
        {
            edge_selection_temp[edge] = (float)((*m_p_input_polyhedra->edge_selection)[edge]);
        }
        copy_from_host_to_device(pdev_per_edge_selection, edge_selection_temp, m_number_of_edges*sizeof(float));

    }

    synchronize_threads();
    free ( face_selection_temp );
    free ( edge_selection_temp );

}

/**
 * Copy the polyhedra from the device
 */
void cuda_device_polyhedra::copy_from_device(const k3d::mesh::polyhedra_t& destination_polyhedra, k3d::uint32_t what_to_copy )
{
    //k3d::log() << debug << "cuda_device_polyhedra::copy_from_device" << std::endl;

    k3d::mesh::polyhedra_t* p_output_polyhedra = const_cast<k3d::mesh::polyhedra_t*>(&destination_polyhedra);

    float* edge_selection_temp = 0;
    if ( what_to_copy & POLYHEDRA_ALL_EDGES )
    {
        //k3d::log() << debug << "cuda_device_polyhedra::copy_from_device::POLYHEDRA_ALL_EDGES" << std::endl;
        edge_selection_temp = (float*) malloc ( m_number_of_edges*sizeof(float) );
        copy_from_device_to_host(edge_selection_temp, pdev_per_edge_selection, m_number_of_edges*sizeof(float));

        if ( !p_output_polyhedra->edge_points )
        {
            boost::shared_ptr<const k3d::mesh::indices_t> edge_points ( new k3d::mesh::indices_t ( m_number_of_edges ) );
            p_output_polyhedra->edge_points = edge_points;
        }
        else if ( p_output_polyhedra->edge_points->size() != m_number_of_edges )
        {
            p_output_polyhedra->edge_points.reset ( new k3d::mesh::indices_t ( m_number_of_edges ) );
        }

        if ( !p_output_polyhedra->clockwise_edges )
        {
            boost::shared_ptr<const k3d::mesh::indices_t> clockwise_edges ( new k3d::mesh::indices_t ( m_number_of_edges ) );
            p_output_polyhedra->clockwise_edges = clockwise_edges;
        }
        else if ( p_output_polyhedra->clockwise_edges->size() != m_number_of_edges )
        {
            p_output_polyhedra->clockwise_edges.reset ( new k3d::mesh::indices_t ( m_number_of_edges ) );
        }

        if ( !p_output_polyhedra->edge_selection )
        {
            boost::shared_ptr<const k3d::mesh::selection_t> edge_selection ( new k3d::mesh::selection_t ( m_number_of_edges ) );
            p_output_polyhedra->edge_selection = edge_selection;
        }
        else if ( p_output_polyhedra->edge_selection->size() != m_number_of_edges )
        {
            p_output_polyhedra->edge_selection.reset ( new k3d::mesh::selection_t ( m_number_of_edges ) );
        }

        synchronize_threads();
        double* out_selection = (double*)&(p_output_polyhedra->edge_selection->front());
        for ( k3d::uint32_t edge = 0 ; edge < m_number_of_edges ; ++edge )
        {
            (out_selection)[edge] = (double) edge_selection_temp[edge];
        }

        copy_from_device_to_host_uint_64_bit_check((void*)&(p_output_polyhedra->edge_points->front()),(const void*) pdev_per_edge_point, m_number_of_edges*sizeof(k3d::uint32_t));
        copy_from_device_to_host_uint_64_bit_check((void*)&(p_output_polyhedra->clockwise_edges->front()), (const void*) pdev_per_edge_clockwise_edge, m_number_of_edges*sizeof(k3d::uint32_t));
    }

    if ( what_to_copy & POLYHEDRA_ALL_LOOPS )
    {
        //k3d::log() << debug << "cuda_device_polyhedra::copy_from_device::POLYHEDRA_ALL_LOOPS" << std::endl;
        if ( !p_output_polyhedra->loop_first_edges )
        {
            boost::shared_ptr<const k3d::mesh::indices_t> loop_first_edges ( new k3d::mesh::indices_t ( m_number_of_loops ) );
            p_output_polyhedra->loop_first_edges = loop_first_edges;
        }
        else if ( p_output_polyhedra->loop_first_edges->size() != m_number_of_loops )
        {
            p_output_polyhedra->loop_first_edges.reset ( new k3d::mesh::indices_t ( m_number_of_loops ) );
        }

        copy_from_device_to_host_uint_64_bit_check((void*)&(p_output_polyhedra->loop_first_edges->front()), (const void*)pdev_per_loop_first_edge, m_number_of_loops*sizeof(k3d::uint32_t));
    }

    float* face_selection_temp = 0;
    if ( what_to_copy & ( POLYHEDRA_ALL_FACES ) )
    {
        //k3d::log() << debug << "cuda_device_polyhedra::copy_from_device::POLYHEDRA_ALL_FACES" << std::endl;
        // allocate memory for the float to double conversion
        face_selection_temp = (float*) malloc ( m_number_of_faces*sizeof(float) );
        copy_from_device_to_host(face_selection_temp, pdev_per_face_selection, m_number_of_faces*sizeof(float));

        if ( !p_output_polyhedra->face_first_loops )
        {
            boost::shared_ptr<const k3d::mesh::indices_t> face_first_loops ( new k3d::mesh::indices_t ( m_number_of_faces ) );
            p_output_polyhedra->face_first_loops = face_first_loops;
        }
        else if ( p_output_polyhedra->face_first_loops->size() != m_number_of_faces )
        {
            p_output_polyhedra->face_first_loops.reset ( new k3d::mesh::indices_t ( m_number_of_faces ) );
        }

        if ( !p_output_polyhedra->face_loop_counts )
        {
            boost::shared_ptr<const k3d::mesh::indices_t> face_loop_counts ( new k3d::mesh::indices_t ( m_number_of_faces ) );
            p_output_polyhedra->face_loop_counts = face_loop_counts;
        }
        else if ( p_output_polyhedra->face_loop_counts->size() != m_number_of_faces )
        {
            p_output_polyhedra->face_loop_counts.reset ( new k3d::mesh::indices_t ( m_number_of_faces ) );
        }

        if ( !p_output_polyhedra->face_selection )
        {
            boost::shared_ptr<const k3d::mesh::selection_t> face_selection ( new k3d::mesh::selection_t ( m_number_of_faces ) );
            p_output_polyhedra->face_selection = face_selection;
        }
        else if ( p_output_polyhedra->face_selection->size() != m_number_of_faces )
        {
            p_output_polyhedra->face_selection.reset ( new k3d::mesh::selection_t ( m_number_of_faces ) );
        }

        synchronize_threads();
        double* out_selection = (double*)&(p_output_polyhedra->face_selection->front());
        for ( k3d::uint32_t face = 0 ; face < m_number_of_faces ; ++face )
        {
            (out_selection)[face] = (double)face_selection_temp[face];
        }

        copy_from_device_to_host_uint_64_bit_check((void*)&(p_output_polyhedra->face_first_loops->front()), (const void*)pdev_per_face_first_loops, m_number_of_faces*sizeof(k3d::uint32_t));
        copy_from_device_to_host_uint_64_bit_check((void*)&(p_output_polyhedra->face_loop_counts->front()), (const void*)pdev_per_face_loop_count, m_number_of_faces*sizeof(k3d::uint32_t));
    }

    if ( what_to_copy & ( POLYHEDRA_ALL_POLYGONS ) )
    {
        //k3d::log() << debug << "cuda_device_polyhedra::copy_from_device::POLYHEDRA_ALL_POLYGONS" << std::endl;

        if ( !p_output_polyhedra->first_faces )
        {
            boost::shared_ptr<const k3d::mesh::indices_t> first_faces ( new k3d::mesh::indices_t ( m_number_of_polygons ) );
            p_output_polyhedra->first_faces = first_faces;
        }
        else if ( p_output_polyhedra->first_faces->size() != m_number_of_polygons )
        {
            p_output_polyhedra->first_faces.reset ( new k3d::mesh::indices_t ( m_number_of_polygons ) );
        }

        if ( !p_output_polyhedra->face_counts )
        {
            boost::shared_ptr<const k3d::mesh::indices_t> face_counts ( new k3d::mesh::indices_t ( m_number_of_polygons ) );
            p_output_polyhedra->face_counts = face_counts;
        }
        else if ( p_output_polyhedra->face_counts->size() != m_number_of_polygons )
        {
            p_output_polyhedra->face_counts.reset ( new k3d::mesh::indices_t ( m_number_of_polygons ) );
        }

        if ( !p_output_polyhedra->types )
        {
            boost::shared_ptr<const k3d::mesh::polyhedra_t::types_t> types ( new k3d::mesh::polyhedra_t::types_t ( m_number_of_polygons ) );
            p_output_polyhedra->types = types;
        }
        else if ( p_output_polyhedra->types->size() != m_number_of_polygons )
        {
            p_output_polyhedra->types.reset ( new k3d::mesh::polyhedra_t::types_t ( m_number_of_polygons ) );
        }

        copy_from_device_to_host_uint_64_bit_check((void*)&(p_output_polyhedra->first_faces->front()), (const void*)pdev_per_polygon_first_face, m_number_of_polygons*sizeof(k3d::uint32_t));
        copy_from_device_to_host_uint_64_bit_check((void*)&(p_output_polyhedra->face_counts->front()), (const void*)pdev_per_polygon_face_count, m_number_of_polygons*sizeof(k3d::uint32_t));
        copy_from_device_to_host((void*)&(p_output_polyhedra->types->front()), (const void*)pdev_per_polygon_types, m_number_of_polygons*sizeof(int32_t));

        // TODO:  constant_data, face_materlials, face_varying_data
        p_output_polyhedra->face_materials = m_p_input_polyhedra->face_materials;
        p_output_polyhedra->constant_data = m_p_input_polyhedra->constant_data;
        p_output_polyhedra->uniform_data = m_p_input_polyhedra->uniform_data;
        p_output_polyhedra->face_varying_data = m_p_input_polyhedra->face_varying_data;
    }

    synchronize_threads();
    free ( face_selection_temp );
    free ( edge_selection_temp );
    //k3d::log() << debug << "cuda_device_polyhedra::copy_from_device::end" << std::endl;
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

k3d::uint32_t* cuda_device_polyhedra::get_per_edge_points_pointer()
{
    return pdev_per_edge_point;
}

k3d::uint32_t* cuda_device_polyhedra::get_per_edge_clockwise_edges_pointer()
{
    return pdev_per_edge_clockwise_edge;
}

k3d::uint32_t* cuda_device_polyhedra::get_per_loop_first_edges_pointer()
{
    return pdev_per_loop_first_edge;
}

k3d::uint32_t* cuda_device_polyhedra::get_per_face_first_loops_pointer()
{
	return pdev_per_face_first_loops;
}

k3d::uint32_t* cuda_device_polyhedra::get_per_face_loop_counts_pointer()
{
	return pdev_per_face_loop_count;
}

float* cuda_device_polyhedra::get_per_edge_selection_pointer()
{
	return pdev_per_edge_selection;
}
