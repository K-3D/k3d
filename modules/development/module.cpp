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

/// Namespace reserved for the development plugin module, to protect public symbols from name clashes with other modules
namespace libk3ddevelopment
{

extern k3d::iplugin_factory& edge_array_painter_factory();
extern k3d::iplugin_factory& face_painter_edge_normals_factory();
extern k3d::iplugin_factory& face_painter_flat_normals_factory();
extern k3d::iplugin_factory& face_painter_smooth_normals_factory();
extern k3d::iplugin_factory& glsl_painter_factory();
extern k3d::iplugin_factory& new_mesh_instance_factory();
extern k3d::iplugin_factory& point_array_painter_factory();
extern k3d::iplugin_factory& sds_edge_painter_factory();
extern k3d::iplugin_factory& sds_face_painter_factory();
extern k3d::iplugin_factory& sds_point_painter_factory();
extern k3d::iplugin_factory& sharp_edges_factory();
extern k3d::iplugin_factory& tweak_points_factory();

} // namespace libk3ddevelopment

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3ddevelopment::edge_array_painter_factory());
	Registry.register_factory(libk3ddevelopment::face_painter_edge_normals_factory());
	Registry.register_factory(libk3ddevelopment::face_painter_flat_normals_factory());
	Registry.register_factory(libk3ddevelopment::face_painter_smooth_normals_factory());
	Registry.register_factory(libk3ddevelopment::glsl_painter_factory());
	Registry.register_factory(libk3ddevelopment::new_mesh_instance_factory());
	Registry.register_factory(libk3ddevelopment::point_array_painter_factory());
	Registry.register_factory(libk3ddevelopment::sds_edge_painter_factory());
	Registry.register_factory(libk3ddevelopment::sds_face_painter_factory());
	Registry.register_factory(libk3ddevelopment::sds_point_painter_factory());
	Registry.register_factory(libk3ddevelopment::sharp_edges_factory());
	Registry.register_factory(libk3ddevelopment::tweak_points_factory());
K3D_MODULE_END

