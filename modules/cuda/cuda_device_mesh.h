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
#include "cuda_device_polyhedra.h"

/// Start fleshing out cuda mesh structure.

class cuda_device_mesh
{
    public:
    	/// Constructors
    	cuda_device_mesh ( );
        cuda_device_mesh ( const k3d::mesh& host_mesh );
        ~cuda_device_mesh ();

        void set_mesh ( const k3d::mesh& input_mesh );
        void set_points_and_selections ( k3d::mesh::points_t input_points, k3d::mesh::selection_t input_selections );

        void copy_to_device ( k3d::uint32_t what_to_copy = ALL_MESH_INFO );
        void copy_from_device ( k3d::mesh& destination_mesh, k3d::uint32_t what_to_copy = ALL_MESH_INFO );

        void allocate_on_device ( k3d::uint32_t what_to_allocate = ALL_MESH_INFO );

        void output_debug_info ();

        cuda_device_polyhedra& get_device_polyhedra ();
        void set_device_polyhedra ( const k3d::mesh::polyhedra_t& host_polyhedra );

        float* get_points_and_selection_pointer();

        k3d::uint32_t* get_polyhedra_edge_point_indices_pointer();
        k3d::uint32_t* get_polyhedra_clockwise_edge_point_indices_pointer();
        k3d::uint32_t* get_polyhedra_loop_first_edges_pointer();

        /// resize the point array - allow for adding points in an operation
        void resize_points_and_selection ( k3d::uint32_t new_number_of_points, float default_selection = 0);

    private:
        /// a pointer to a array of float4's allocated on the device
        float* pdev_points_and_selection;
        k3d::uint32_t m_number_of_points;

        const k3d::mesh* m_p_host_mesh;
        k3d::mesh::points_t* m_p_host_points;
        k3d::mesh::selection_t* m_p_host_selections;

        cuda_device_polyhedra m_cuda_device_polyhedra;
};

