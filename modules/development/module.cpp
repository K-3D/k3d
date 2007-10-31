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

namespace module
{

/// Namespace reserved for the development plugin module, to protect public symbols from name clashes with other modules
namespace development
{

extern k3d::iplugin_factory& edge_array_painter_factory();
extern k3d::iplugin_factory& face_painter_edge_normals_factory();
extern k3d::iplugin_factory& face_painter_flat_normals_factory();
extern k3d::iplugin_factory& face_painter_smooth_normals_factory();
extern k3d::iplugin_factory& glsl_painter_factory();
extern k3d::iplugin_factory& gl_triangulated_face_painter_factory();
extern k3d::iplugin_factory& point_array_painter_factory();
extern k3d::iplugin_factory& sds_gl_edge_painter_factory();
extern k3d::iplugin_factory& sds_gl_face_painter_factory();
extern k3d::iplugin_factory& sds_gl_point_painter_factory();
extern k3d::iplugin_factory& sds_vbo_edge_painter_factory();
extern k3d::iplugin_factory& sds_vbo_face_painter_factory();
extern k3d::iplugin_factory& sds_vbo_point_painter_factory();
extern k3d::iplugin_factory& sharp_edges_factory();
extern k3d::iplugin_factory& triangulate_faces_factory();
extern k3d::iplugin_factory& tweak_points_factory();
extern k3d::iplugin_factory& vbo_triangulated_face_painter_factory();

} // namespace development

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::development::edge_array_painter_factory());
	Registry.register_factory(module::development::face_painter_edge_normals_factory());
	Registry.register_factory(module::development::face_painter_flat_normals_factory());
	Registry.register_factory(module::development::face_painter_smooth_normals_factory());
	Registry.register_factory(module::development::glsl_painter_factory());
	Registry.register_factory(module::development::gl_triangulated_face_painter_factory());
	Registry.register_factory(module::development::point_array_painter_factory());
	Registry.register_factory(module::development::sds_gl_edge_painter_factory());
	Registry.register_factory(module::development::sds_gl_face_painter_factory());
	Registry.register_factory(module::development::sds_gl_point_painter_factory());
	Registry.register_factory(module::development::sds_vbo_edge_painter_factory());
	Registry.register_factory(module::development::sds_vbo_face_painter_factory());
	Registry.register_factory(module::development::sds_vbo_point_painter_factory());
	Registry.register_factory(module::development::sharp_edges_factory());
	Registry.register_factory(module::development::triangulate_faces_factory());
	Registry.register_factory(module::development::tweak_points_factory());
	Registry.register_factory(module::development::vbo_triangulated_face_painter_factory());
K3D_MODULE_END

