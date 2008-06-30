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

class cuda_polyhedra
{
    public:
        /// Constructors
        cuda_polyhedra ( const k3d::mesh::polyhedra_t& input_polyhedra, const k3d::mesh::polyhedra_t& output_polyhedra );
        /// Destructor
        ~cuda_polyhedra ();
        
        void copy_to_device ();
        void copy_from_device ();
    private:    
        /// a pointer to uint arrays allocated on the device
        k3d::uint_t* pdev_per_polygon_first_face;
        k3d::uint_t* pdev_per_polygon_face_count;
        
        /// a pointer to a int array allocated on the device
        int32_t* pdev_per_polygon_types;
        
        /// the number of polyhedra        
        k3d::uint_t m_number_of_polygons;
        
        /// a pointer to uint arrays allocated on the device
        k3d::uint_t* pdev_per_face_first_loops;
        k3d::uint_t* pdev_per_face_loop_count;
        
        /// a pointer to a float array on the device
        float* pdev_per_face_selection;
        
        /// the number of faces - ie.  The length of the above 3 arrays
        k3d::uint_t m_number_of_faces;
        
        /// a pointer to a uint array allocated on the device
        k3d::uint_t* pdev_per_loop_first_edge;
        /// the number of loops - ie.  The length of the above array
        k3d::uint_t m_number_of_loops;
        
        /// a pointer to uint arrays allocated on the device
        k3d::uint_t* pdev_per_edge_point;
        k3d::uint_t* pdev_per_edge_clockwise_edge;       
        /// a pointer to a float array on the device
        float* pdev_per_edge_selection;
        
        /// the number of edges - ie.  The length of the above 3 arrays
        k3d::uint_t m_number_of_edges;
        
        /// a reference to the host version of the polyhedra_t   
        const k3d::mesh::polyhedra_t* m_p_input_polyhedra;
        const k3d::mesh::polyhedra_t* m_p_output_polyhedra;
};

/// Start fleshing out cuda mesh structure.

class cuda_mesh
{
    public:
        cuda_mesh ( const k3d::mesh& input_mesh, k3d::mesh& output_mesh);
        ~cuda_mesh ();
        
        void copy_to_device ();
        void copy_from_device ();

    private:
        /// a pointer to a array of float4's allocated on the device
        float* pdev_points_and_selection;       
        k3d::uint_t m_number_of_points;
        
        const k3d::mesh* m_p_input_mesh;
        k3d::mesh* m_p_output_mesh;
        
        cuda_polyhedra m_cuda_polyhedra;
};        

