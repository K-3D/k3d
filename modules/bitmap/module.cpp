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
		\brief Implements procedures required to export K-3D objects from the libk3dbitmap module
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the libk3dbitmap plugin module, to protect public symbols from name clashes with other modules
namespace libk3dbitmap
{

extern k3d::iplugin_factory& bitmap_add_factory();
extern k3d::iplugin_factory& bitmap_checker_factory();
extern k3d::iplugin_factory& bitmap_color_monochrome_factory();
extern k3d::iplugin_factory& bitmap_gamma_factory();
extern k3d::iplugin_factory& bitmap_invert_factory();
extern k3d::iplugin_factory& bitmap_matte_colordiff_factory();
extern k3d::iplugin_factory& bitmap_matte_invert_factory();
extern k3d::iplugin_factory& bitmap_multiply_factory();
extern k3d::iplugin_factory& bitmap_reader_factory();
extern k3d::iplugin_factory& bitmap_solid_factory();
extern k3d::iplugin_factory& bitmap_sub_factory();
extern k3d::iplugin_factory& bitmap_threshold_factory();
/*
extern k3d::iplugin_factory& bitmap_image_add_factory();
extern k3d::iplugin_factory& bitmap_image_atop_factory();
extern k3d::iplugin_factory& bitmap_image_div_factory();
extern k3d::iplugin_factory& bitmap_image_inside_factory();
extern k3d::iplugin_factory& bitmap_image_max_factory();
extern k3d::iplugin_factory& bitmap_image_min_factory();
extern k3d::iplugin_factory& bitmap_image_mix_factory();
extern k3d::iplugin_factory& bitmap_image_multiply_factory();
extern k3d::iplugin_factory& bitmap_image_outside_factory();
extern k3d::iplugin_factory& bitmap_image_over_factory();
extern k3d::iplugin_factory& bitmap_image_screen_factory();
extern k3d::iplugin_factory& bitmap_image_sub_abs_factory();
extern k3d::iplugin_factory& bitmap_image_sub_factory();
extern k3d::iplugin_factory& bitmap_image_under_factory();
extern k3d::iplugin_factory& bitmap_image_xor_factory();
extern k3d::iplugin_factory& bitmap_sequence_in_factory();
extern k3d::iplugin_factory& bitmap_sequence_out_factory();
extern k3d::iplugin_factory& bitmap_size_factory();
*/

} // namespace libk3dbitmap

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dbitmap::bitmap_add_factory());
	Registry.register_factory(libk3dbitmap::bitmap_checker_factory());
	Registry.register_factory(libk3dbitmap::bitmap_color_monochrome_factory());
	Registry.register_factory(libk3dbitmap::bitmap_gamma_factory());
	Registry.register_factory(libk3dbitmap::bitmap_invert_factory());
	Registry.register_factory(libk3dbitmap::bitmap_matte_colordiff_factory());
	Registry.register_factory(libk3dbitmap::bitmap_matte_invert_factory());
	Registry.register_factory(libk3dbitmap::bitmap_multiply_factory());
	Registry.register_factory(libk3dbitmap::bitmap_reader_factory());
	Registry.register_factory(libk3dbitmap::bitmap_solid_factory());
	Registry.register_factory(libk3dbitmap::bitmap_sub_factory());
	Registry.register_factory(libk3dbitmap::bitmap_threshold_factory());
/*
	Registry.register_factory(libk3dbitmap::bitmap_image_add_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_atop_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_div_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_inside_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_max_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_min_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_mix_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_multiply_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_outside_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_over_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_screen_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_sub_abs_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_sub_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_under_factory());
	Registry.register_factory(libk3dbitmap::bitmap_image_xor_factory());
	Registry.register_factory(libk3dbitmap::bitmap_sequence_in_factory());
	Registry.register_factory(libk3dbitmap::bitmap_sequence_out_factory());
	Registry.register_factory(libk3dbitmap::bitmap_size_factory());
*/
K3D_MODULE_END

