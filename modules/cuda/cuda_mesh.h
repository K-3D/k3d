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


class cuda_polyhedra
{
    public:
        /// Constructors
        cuda_polyhedra ( const k3d::mesh::polyhedra_t& input_polyhedra );
        /// Destructor
        ~cuda_polyhedra ();
        
        /// initialize the polyhedra on the device
        void init_device_version ();
        
    private:    
        /// a pointer to a uint2 array allocated on the device
        k3d::uint_t* pdev_per_polygon_first_face_and_face_count;
        /// a pointer to a int array allocated on the device
        int32_t* pdev_per_polygon_types;
        
        /// the number of polyhedra - ie.  The lenghth of the above 2 arrays        
        k3d::uint_t number_of_polygons;
        
        /// a pointer to a uint2 array allocated on the device
        k3d::uint_t* pdev_per_face_first_loops_and_loop_count;
        /// a pointer to a float array on the device
        float* pdev_per_face_selection;
        
        /// the number of faces - ie.  The length of the above 2 arrays
        k3d::uint_t number_of_faces;
        
        /// a pointer to a uint array allocated on the device
        k3d::uint_t* pdev_per_loop_first_edge;
        /// the number of loops - ie.  The length of the above array
        k3d::uint_t number_of_loops;
        
        /// a pointer to a uint2 array allocated on the device
        k3d::uint_t* pdev_per_edge_point_and_clockwise_edge;
        /// a pointer to a float array on the device
        float* pdev_per_edge_selection;
        
        /// the number of edges - ie.  The length of the above 2 arrays
        k3d::uint_t number_of_edges;
        
        /// a reference to the host version of the polyhedra_t   
        const k3d::mesh::polyhedra_t& m_polyhedra_reference;
        
};

/// Start fleshing out cuda mesh structure.

class cuda_mesh
{
    public:
        /// Construct a CUDA mesh from an existing mesh data structure
        cuda_mesh ( k3d::mesh& intput_mesh );
        ~cuda_mesh ();
        
        /// initialize the mesh on the device
        void init_device_version ();

    private:
        /// a pointer to a array of float4's allocated on the device
        float* pdev_points_and_selection;
        k3d::uint_t number_of_points;
        
        /// a reference to the host verion of the mesh        
        k3d::mesh& m_mesh_reference;
        
        cuda_polyhedra m_cuda_polyhedra;
};        

