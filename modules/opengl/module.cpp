// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

/// Namespace reserved for the opengl plugin module, to protect public symbols from name clashes with other modules
namespace libk3dopengl
{

extern k3d::iplugin_factory& bicubic_patch_painter_factory();
extern k3d::iplugin_factory& bilinear_patch_painter_factory();
extern k3d::iplugin_factory& blobby_point_painter_factory();
extern k3d::iplugin_factory& cubic_curve_painter_factory();
extern k3d::iplugin_factory& edge_painter_factory();
extern k3d::iplugin_factory& face_normal_painter_factory();
extern k3d::iplugin_factory& face_numbering_painter_factory();
extern k3d::iplugin_factory& face_orientation_painter_factory();
extern k3d::iplugin_factory& face_painter_factory();
extern k3d::iplugin_factory& linear_curve_painter_factory();
extern k3d::iplugin_factory& material_factory();
extern k3d::iplugin_factory& multi_painter_factory();
extern k3d::iplugin_factory& nurbs_curve_painter_factory();
extern k3d::iplugin_factory& nurbs_patch_painter_factory();
extern k3d::iplugin_factory& point_painter_factory();
extern k3d::iplugin_factory& point_numbering_painter_factory();
extern k3d::iplugin_factory& render_engine_factory();

} // namespace libk3dopengl

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dopengl::bicubic_patch_painter_factory());
	Registry.register_factory(libk3dopengl::bilinear_patch_painter_factory());
	Registry.register_factory(libk3dopengl::blobby_point_painter_factory());
	Registry.register_factory(libk3dopengl::cubic_curve_painter_factory());
	Registry.register_factory(libk3dopengl::edge_painter_factory());
	Registry.register_factory(libk3dopengl::face_normal_painter_factory());
	Registry.register_factory(libk3dopengl::face_numbering_painter_factory());
	Registry.register_factory(libk3dopengl::face_orientation_painter_factory());
	Registry.register_factory(libk3dopengl::face_painter_factory());
	Registry.register_factory(libk3dopengl::linear_curve_painter_factory());
	Registry.register_factory(libk3dopengl::material_factory());
	Registry.register_factory(libk3dopengl::multi_painter_factory());
	Registry.register_factory(libk3dopengl::nurbs_curve_painter_factory());
	Registry.register_factory(libk3dopengl::nurbs_patch_painter_factory());
	Registry.register_factory(libk3dopengl::point_painter_factory());
	Registry.register_factory(libk3dopengl::point_numbering_painter_factory());
	Registry.register_factory(libk3dopengl::render_engine_factory());
K3D_MODULE_END

