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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

namespace module
{

namespace bitmap
{

extern k3d::iplugin_factory& add_factory();
extern k3d::iplugin_factory& checker_factory();
extern k3d::iplugin_factory& color_monochrome_factory();
extern k3d::iplugin_factory& gamma_factory();
extern k3d::iplugin_factory& invert_factory();
extern k3d::iplugin_factory& matte_colordiff_factory();
extern k3d::iplugin_factory& matte_invert_factory();
extern k3d::iplugin_factory& multiply_factory();
extern k3d::iplugin_factory& reader_factory();
extern k3d::iplugin_factory& solid_factory();
extern k3d::iplugin_factory& subtract_factory();
extern k3d::iplugin_factory& threshold_factory();

} // namespace bitmap

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::bitmap::add_factory());
	Registry.register_factory(module::bitmap::checker_factory());
	Registry.register_factory(module::bitmap::color_monochrome_factory());
	Registry.register_factory(module::bitmap::gamma_factory());
	Registry.register_factory(module::bitmap::invert_factory());
	Registry.register_factory(module::bitmap::matte_colordiff_factory());
	Registry.register_factory(module::bitmap::matte_invert_factory());
	Registry.register_factory(module::bitmap::multiply_factory());
	Registry.register_factory(module::bitmap::reader_factory());
	Registry.register_factory(module::bitmap::solid_factory());
	Registry.register_factory(module::bitmap::subtract_factory());
	Registry.register_factory(module::bitmap::threshold_factory());
K3D_MODULE_END

