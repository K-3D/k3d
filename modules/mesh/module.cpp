// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the mesh plugin module, to protect public symbols from name clashes with other modules
namespace libk3dmesh
{

extern k3d::iplugin_factory& array_1d_factory();
extern k3d::iplugin_factory& array_2d_factory();
extern k3d::iplugin_factory& array_3d_factory();
extern k3d::iplugin_factory& bevel_faces_factory();
extern k3d::iplugin_factory& bridge_edges_factory();
extern k3d::iplugin_factory& bridge_faces_factory();
extern k3d::iplugin_factory& cap_hole_factory();
extern k3d::iplugin_factory& catmull_clark_subdivision_factory();
extern k3d::iplugin_factory& collapse_edges_factory();
extern k3d::iplugin_factory& collapse_faces_factory();
extern k3d::iplugin_factory& connect_vertices_factory();
extern k3d::iplugin_factory& delete_components_factory();
extern k3d::iplugin_factory& dissolve_components_factory();
extern k3d::iplugin_factory& face_normals_factory();
extern k3d::iplugin_factory& fillet_edges_factory();
extern k3d::iplugin_factory& flip_orientation_factory();
extern k3d::iplugin_factory& join_points_factory();
extern k3d::iplugin_factory& make_creases_factory();
extern k3d::iplugin_factory& make_sds_factory();
extern k3d::iplugin_factory& merge_mesh_factory();
extern k3d::iplugin_factory& mesh_instance_factory();
extern k3d::iplugin_factory& move_first_edge_factory();
extern k3d::iplugin_factory& sds_corner_factory();
extern k3d::iplugin_factory& sds_crease_factory();
extern k3d::iplugin_factory& sds_hole_factory();
extern k3d::iplugin_factory& set_material_factory();
extern k3d::iplugin_factory& show_component_numbering_factory();
extern k3d::iplugin_factory& show_face_orientations_factory();
extern k3d::iplugin_factory& smooth_shade_factory();
extern k3d::iplugin_factory& subdivide_edges_factory();
extern k3d::iplugin_factory& subdivide_faces_factory();
extern k3d::iplugin_factory& tag_color_factory();
extern k3d::iplugin_factory& triangulate_faces_factory();
extern k3d::iplugin_factory& weld_factory();

} // namespace libk3dmesh

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dmesh::array_1d_factory());
	Registry.register_factory(libk3dmesh::array_2d_factory());
	Registry.register_factory(libk3dmesh::array_3d_factory());
	Registry.register_factory(libk3dmesh::bevel_faces_factory());
	Registry.register_factory(libk3dmesh::bridge_edges_factory());
	Registry.register_factory(libk3dmesh::bridge_faces_factory());
	Registry.register_factory(libk3dmesh::cap_hole_factory());
	Registry.register_factory(libk3dmesh::catmull_clark_subdivision_factory());
	Registry.register_factory(libk3dmesh::collapse_edges_factory());
	Registry.register_factory(libk3dmesh::collapse_faces_factory());
	Registry.register_factory(libk3dmesh::connect_vertices_factory());
	Registry.register_factory(libk3dmesh::delete_components_factory());
	Registry.register_factory(libk3dmesh::dissolve_components_factory());
	Registry.register_factory(libk3dmesh::face_normals_factory());
	Registry.register_factory(libk3dmesh::fillet_edges_factory());
	Registry.register_factory(libk3dmesh::flip_orientation_factory());
	Registry.register_factory(libk3dmesh::join_points_factory());
	Registry.register_factory(libk3dmesh::make_creases_factory());
	Registry.register_factory(libk3dmesh::make_sds_factory());
	Registry.register_factory(libk3dmesh::merge_mesh_factory());
	Registry.register_factory(libk3dmesh::move_first_edge_factory());
	Registry.register_factory(libk3dmesh::sds_corner_factory());
	Registry.register_factory(libk3dmesh::sds_crease_factory());
	Registry.register_factory(libk3dmesh::sds_hole_factory());
	Registry.register_factory(libk3dmesh::set_material_factory());
	Registry.register_factory(libk3dmesh::smooth_shade_factory());
	Registry.register_factory(libk3dmesh::subdivide_edges_factory());
	Registry.register_factory(libk3dmesh::subdivide_faces_factory());
	Registry.register_factory(libk3dmesh::tag_color_factory());
	Registry.register_factory(libk3dmesh::triangulate_faces_factory());
	Registry.register_factory(libk3dmesh::weld_factory());
K3D_MODULE_END

