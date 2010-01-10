// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

namespace module
{

namespace polyhedron
{

extern k3d::iplugin_factory& bevel_faces_factory();
extern k3d::iplugin_factory& bevel_points_factory();
extern k3d::iplugin_factory& bridge_edges_factory();
extern k3d::iplugin_factory& cap_holes_factory();
extern k3d::iplugin_factory& collapse_edges_factory();
extern k3d::iplugin_factory& collapse_faces_factory();
extern k3d::iplugin_factory& collapse_points_factory();
extern k3d::iplugin_factory& connect_vertices_factory();
extern k3d::iplugin_factory& delete_components_factory();
extern k3d::iplugin_factory& dissolve_components_factory();
extern k3d::iplugin_factory& extrude_faces_factory();
extern k3d::iplugin_factory& fillet_edges_factory();
extern k3d::iplugin_factory& flip_orientation_factory();
extern k3d::iplugin_factory& make_sds_factory();
extern k3d::iplugin_factory& merge_collinear_edges_factory();
extern k3d::iplugin_factory& merge_coplanar_faces_factory();
extern k3d::iplugin_factory& move_first_edge_factory();
extern k3d::iplugin_factory& sds_corner_factory();
extern k3d::iplugin_factory& sds_crease_factory();
extern k3d::iplugin_factory& sds_hole_factory();
extern k3d::iplugin_factory& subdivide_edges_factory();
extern k3d::iplugin_factory& subdivide_faces_factory();
extern k3d::iplugin_factory& triangulate_faces_factory();

} // namespace polyhedron

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::polyhedron::bevel_faces_factory());
	Registry.register_factory(module::polyhedron::bevel_points_factory());
	Registry.register_factory(module::polyhedron::bridge_edges_factory());
	Registry.register_factory(module::polyhedron::cap_holes_factory());
	Registry.register_factory(module::polyhedron::collapse_edges_factory());
	Registry.register_factory(module::polyhedron::collapse_faces_factory());
	Registry.register_factory(module::polyhedron::collapse_points_factory());
	Registry.register_factory(module::polyhedron::connect_vertices_factory());
	Registry.register_factory(module::polyhedron::delete_components_factory());
	Registry.register_factory(module::polyhedron::dissolve_components_factory());
	Registry.register_factory(module::polyhedron::extrude_faces_factory());
	Registry.register_factory(module::polyhedron::fillet_edges_factory());
	Registry.register_factory(module::polyhedron::flip_orientation_factory());
	Registry.register_factory(module::polyhedron::make_sds_factory());
	Registry.register_factory(module::polyhedron::merge_collinear_edges_factory());
	Registry.register_factory(module::polyhedron::merge_coplanar_faces_factory());
	Registry.register_factory(module::polyhedron::move_first_edge_factory());
	Registry.register_factory(module::polyhedron::sds_corner_factory());
	Registry.register_factory(module::polyhedron::sds_crease_factory());
	Registry.register_factory(module::polyhedron::sds_hole_factory());
	Registry.register_factory(module::polyhedron::subdivide_edges_factory());
	Registry.register_factory(module::polyhedron::subdivide_faces_factory());
	Registry.register_factory(module::polyhedron::triangulate_faces_factory());
K3D_MODULE_END

