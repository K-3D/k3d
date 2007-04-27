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
		\brief Implements the functions necessary to export K-3D objects from the libk3dgeometry module
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the geometry plugin module, to protect public symbols from name clashes with other modules
namespace libk3dgeometry
{

extern k3d::iplugin_factory& gts_reader_factory();
extern k3d::iplugin_factory& gts_writer_factory();
extern k3d::iplugin_factory& k3d_mesh_input_factory();
extern k3d::iplugin_factory& k3d_mesh_output_factory();
extern k3d::iplugin_factory& mfx_reader_factory();
extern k3d::iplugin_factory& off_reader_factory();
extern k3d::iplugin_factory& raw_reader_factory();
extern k3d::iplugin_factory& raw_writer_factory();
//#ifndef K3D_PLATFORM_WIN32
extern k3d::iplugin_factory& rib_reader_factory();
//#endif // !K3D_PLATFORM_WIN32
/*
extern k3d::iplugin_factory& lwo2_reader_factory();
extern k3d::iplugin_factory& max_reader_factory();
*/

} // libk3dgeometry

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dgeometry::gts_reader_factory());
	Registry.register_factory(libk3dgeometry::gts_writer_factory());
	Registry.register_factory(libk3dgeometry::k3d_mesh_input_factory());
	Registry.register_factory(libk3dgeometry::k3d_mesh_output_factory());
	Registry.register_factory(libk3dgeometry::mfx_reader_factory());
	Registry.register_factory(libk3dgeometry::off_reader_factory());
	Registry.register_factory(libk3dgeometry::raw_reader_factory());
	Registry.register_factory(libk3dgeometry::raw_writer_factory());
//#ifndef K3D_PLATFORM_WIN32
	Registry.register_factory(libk3dgeometry::rib_reader_factory());
//#endif // !K3D_PLATFORM_WIN32
/*
	Registry.register_factory(libk3dgeometry::lwo2_reader_factory());
	Registry.register_factory(libk3dgeometry::max_reader_factory());
*/
K3D_MODULE_END


