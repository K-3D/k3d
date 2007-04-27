// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Implements procedures required to export K-3D objects from the core module
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the renderman plugin module, to protect public symbols from name clashes with other modules
namespace libk3drendermanpainters
{

extern k3d::iplugin_factory& bicubic_patch_painter_factory();
extern k3d::iplugin_factory& bilinear_patch_painter_factory();
extern k3d::iplugin_factory& blobby_painter_factory();
extern k3d::iplugin_factory& cubic_curve_painter_factory();
extern k3d::iplugin_factory& linear_curve_painter_factory();
extern k3d::iplugin_factory& multi_painter_factory();
extern k3d::iplugin_factory& multi_painter_factory();
extern k3d::iplugin_factory& nurbs_patch_painter_factory();
extern k3d::iplugin_factory& point_group_painter_factory();
extern k3d::iplugin_factory& polyhedron_painter_factory();
extern k3d::iplugin_factory& subdivision_surface_painter_factory();

} // namespace libk3drendermanpainters

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3drendermanpainters::bicubic_patch_painter_factory());
	Registry.register_factory(libk3drendermanpainters::bilinear_patch_painter_factory());
	Registry.register_factory(libk3drendermanpainters::blobby_painter_factory());
	Registry.register_factory(libk3drendermanpainters::cubic_curve_painter_factory());
	Registry.register_factory(libk3drendermanpainters::linear_curve_painter_factory());
	Registry.register_factory(libk3drendermanpainters::multi_painter_factory());
	Registry.register_factory(libk3drendermanpainters::nurbs_patch_painter_factory());
	Registry.register_factory(libk3drendermanpainters::point_group_painter_factory());
	Registry.register_factory(libk3drendermanpainters::polyhedron_painter_factory());
	Registry.register_factory(libk3drendermanpainters::subdivision_surface_painter_factory());
K3D_MODULE_END

