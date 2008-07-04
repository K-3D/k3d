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


#include <k3dsdk/mesh.h>
#include "cuda_entry_points.h"

class cuda_device_polyhedra
{
    public:
        /// Constructors
        cuda_device_polyhedra ( const k3d::mesh::polyhedra_t& host_polyhedra );
        /// Destructor
        ~cuda_device_polyhedra ();
        
        void copy_to_device ();
        void copy_from_device ( k3d::mesh::polyhedra_t& destination_polyhedra );
        
        void output_debug_info ();
        
        void allocate_additional_polyhedra ( k3d::uint_t number_of_additional_polyhedra );
        void allocate_additional_faces ( k3d::uint_t number_of_additional_polyhedra );
        void allocate_additional_loops ( k3d::uint_t number_of_additional_loops );
        void allocate_additional_edges ( k3d::uint_t number_of_additional_edges );
        
        k3d::uint_t* get_per_edge_points_pointer();
        k3d::uint_t* get_additional_per_edge_points_pointer();
        k3d::uint_t* get_per_edge_clockwise_edges_pointer();
        k3d::uint_t* get_additional_per_edge_clockwise_edges_pointer();
    private:    
        /// a pointer to uint arrays allocated on the device
        k3d::uint_t* pdev_per_polygon_first_face;
        k3d::uint_t* pdev_per_polygon_face_count;
        
        /// a pointer to a int array allocated on the device
        int32_t* pdev_per_polygon_types;
        
        /// the number of polyhedra        
        k3d::uint_t m_number_of_polygons;
        
        /// storage for additional polyhedra
        k3d::uint_t* pdev_additional_per_polygon_first_face;
        k3d::uint_t* pdev_additional_per_polygon_face_count;
        int32_t* pdev_additional_per_polygon_types;
        k3d::uint_t m_number_of_additional_polyhedra;
        
        
        /// a pointer to uint arrays allocated on the device
        k3d::uint_t* pdev_per_face_first_loops;
        k3d::uint_t* pdev_per_face_loop_count;
        
        /// a pointer to a float array on the device
        float* pdev_per_face_selection;
        
        /// the number of faces - ie.  The length of the above 3 arrays
        k3d::uint_t m_number_of_faces;
        
        /// storage for additional faces
        k3d::uint_t* pdev_additional_per_face_first_loops;
        k3d::uint_t* pdev_additional_per_face_loop_count;
        float* pdev_additional_per_face_selection;
        k3d::uint_t m_number_of_additional_faces;
        
        /// a pointer to a uint array allocated on the device
        k3d::uint_t* pdev_per_loop_first_edge;
        /// the number of loops - ie.  The length of the above array
        k3d::uint_t m_number_of_loops;
        
        /// storage for additional loops
        k3d::uint_t* pdev_additional_per_loop_first_edge;
        k3d::uint_t m_number_of_additional_loops;
        
        /// a pointer to uint arrays allocated on the device
        k3d::uint_t* pdev_per_edge_point;
        k3d::uint_t* pdev_per_edge_clockwise_edge;       
        /// a pointer to a float array on the device
        float* pdev_per_edge_selection;
        
        /// the number of edges - ie.  The length of the above 3 arrays
        k3d::uint_t m_number_of_edges;
        
        /// storage for additional edges
        k3d::uint_t* pdev_additional_per_edge_point;
        k3d::uint_t* pdev_additional_per_edge_clockwise_edge;       
        float* pdev_additional_per_edge_selection;
        k3d::uint_t m_number_of_additional_edges;
        
        /// a reference to the host version of the polyhedra_t   
        const k3d::mesh::polyhedra_t* m_p_input_polyhedra;
};

/// Start fleshing out cuda mesh structure.

class cuda_device_mesh
{
    public:
        cuda_device_mesh ( const k3d::mesh& host_mesh );
        ~cuda_device_mesh ();
        
        void copy_to_device ();
        void copy_from_device ( k3d::mesh& destination_mesh );
        
        void output_debug_info ();
        
        void allocate_additional_points ( k3d::uint_t number_of_new_points );
        
        cuda_device_polyhedra& get_device_polyhedra ();
        
        float* get_points_and_selection_pointer();
        float* get_additional_points_and_selection_pointer();
    private:
        /// a pointer to a array of float4's allocated on the device
        float* pdev_points_and_selection;       
        k3d::uint_t m_number_of_points;
        
        float* pdev_additional_points;
        k3d::uint_t m_number_of_additional_points;
        
        const k3d::mesh* m_p_host_mesh;
        cuda_device_polyhedra m_cuda_device_polyhedra;        
};        

