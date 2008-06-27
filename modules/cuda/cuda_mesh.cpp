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
#include "cuda_entry_points.h"

/// Constructor
cuda_polyhedra::cuda_polyhedra ( const k3d::mesh::polyhedra_t& input_polyhedra ) :
    m_polyhedra_reference(input_polyhedra)
{
    number_of_polygons = m_polyhedra_reference.first_faces->size();
    number_of_faces = m_polyhedra_reference.face_first_loops->size();
    number_of_loops = m_polyhedra_reference.loop_first_edges->size();
    number_of_edges = m_polyhedra_reference.edge_points->size();
    
    // allocate the device memory for the polygons, faces, loops and edges
    allocate_device_memory((void**)&pdev_per_polygon_first_face_and_face_count, number_of_polygons*2*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_polygon_types, number_of_polygons*sizeof(int32_t));
    
    allocate_device_memory((void**)&pdev_per_face_first_loops_and_loop_count, number_of_faces*2*sizeof(k3d::uint_t));        
    allocate_device_memory((void**)&pdev_per_face_selection, number_of_faces*sizeof(float));
    
    allocate_device_memory((void**)&pdev_per_loop_first_edge, number_of_loops*sizeof(k3d::uint_t));
    
    allocate_device_memory((void**)&pdev_per_edge_point_and_clockwise_edge, number_of_edges*2*sizeof(k3d::uint_t));
    allocate_device_memory((void**)&pdev_per_edge_selection, number_of_edges*sizeof(float));
            
    // copy the data to the device (pre-convert if needed)
            
}

/// Destructor - free the allocated device pointers
cuda_polyhedra::~cuda_polyhedra ()
{
    free_device_memory((void*)pdev_per_polygon_first_face_and_face_count);
    free_device_memory((void*)pdev_per_polygon_types);
    
    free_device_memory((void*)pdev_per_face_first_loops_and_loop_count);        
    free_device_memory((void*)pdev_per_face_selection);
    
    free_device_memory((void*)pdev_per_loop_first_edge);
    
    free_device_memory((void*)pdev_per_edge_point_and_clockwise_edge);
    free_device_memory((void*)pdev_per_edge_selection);
}   


/// Constructor
cuda_mesh::cuda_mesh(k3d::mesh& input_mesh):
    m_mesh_reference(input_mesh),
    m_cuda_polyhedra(*(input_mesh.polyhedra))
{
    number_of_points = m_mesh_reference.points->size();
    
    // allocate the memory for the points and the point selection
    allocate_device_memory((void**)&pdev_points_and_selection, number_of_points*4*sizeof(float));
    
    // convert the point coordinates and selection to floats and copy to the device
    
    // create a cuda_polyhedron
            
}

/// Destructor - frees the allocated device pointers
cuda_mesh::~cuda_mesh()
{
    free_device_memory((void*)pdev_points_and_selection);
}
    
