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

#define ALL_MESH_INFO 0xFFFF
#define MESH_POINTS 0x0001
#define MESH_SELECTION 0x0002
#define POLYHEDRA_ALL_POLYGONS 0x0004
#define POLYHEDRA_ALL_FACES 0x0008
#define POLYHEDRA_ALL_LOOPS 0x0010
#define POLYHEDRA_ALL_EDGES 0x0020
#define ALL_POLYHEDRA_INFO 0x003C

#include <k3dsdk/mesh.h>
#include "cuda_entry_points.h"

class cuda_device_polyhedra
{
    public:
        /// Constructors
    	cuda_device_polyhedra ( );
        cuda_device_polyhedra ( const k3d::mesh::polyhedra_t& host_polyhedra );

        /// Destructor
        ~cuda_device_polyhedra ();

		void set_polyhedra( const k3d::mesh::polyhedra_t& host_polyhedra );
        void copy_to_device (  k3d::uint32_t what_to_copy = ALL_MESH_INFO  );
        void copy_from_device ( const k3d::mesh::polyhedra_t& destination_polyhedra,  k3d::uint32_t what_to_copy = ALL_MESH_INFO );

        void allocate_on_device ( k3d::uint32_t what_to_allocate = ALL_MESH_INFO );

        void output_debug_info ();

        k3d::uint32_t* get_per_edge_points_pointer();
        k3d::uint32_t* get_per_edge_clockwise_edges_pointer();

        k3d::uint32_t* get_per_loop_first_edges_pointer();

        k3d::uint32_t* get_per_face_first_loops_pointer();
        k3d::uint32_t* get_per_face_loop_counts_pointer();
        float* get_per_face_selection_pointer();

        float* get_per_edge_selection_pointer();

        void resize_edges ( k3d::uint32_t new_number_of_edges, bool clear = false );
        void resize_loops ( k3d::uint32_t new_number_of_loops, bool clear = false );
        void resize_faces ( k3d::uint32_t new_number_of_faces, bool clear = false );

    private:
        /// a pointer to uint arrays allocated on the device
        k3d::uint32_t* pdev_per_polygon_first_face;
        k3d::uint32_t* pdev_per_polygon_face_count;

        /// a pointer to a int array allocated on the device
        int32_t* pdev_per_polygon_types;

        /// the number of polyhedra
        k3d::uint32_t m_number_of_polygons;


        /// a pointer to uint arrays allocated on the device
        k3d::uint32_t* pdev_per_face_first_loops;
        k3d::uint32_t* pdev_per_face_loop_count;

        /// a pointer to a float array on the device
        float* pdev_per_face_selection;

        /// the number of faces - ie.  The length of the above 3 arrays
        k3d::uint32_t m_number_of_faces;

        /// a pointer to a uint array allocated on the device
        k3d::uint32_t* pdev_per_loop_first_edge;
        /// the number of loops - ie.  The length of the above array
        k3d::uint32_t m_number_of_loops;

        /// a pointer to uint arrays allocated on the device
        k3d::uint32_t* pdev_per_edge_point;
        k3d::uint32_t* pdev_per_edge_clockwise_edge;
        /// a pointer to a float array on the device
        float* pdev_per_edge_selection;

        /// the number of edges - ie.  The length of the above 3 arrays
        k3d::uint32_t m_number_of_edges;

        /// a pointer to the host version of the polyhedra_t
        const k3d::mesh::polyhedra_t* m_p_input_polyhedra;
};
