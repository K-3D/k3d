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
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "simple_bitmap_modifier.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>

namespace libk3dbitmap
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_matte_invert

class bitmap_matte_invert :
	public simple_bitmap_modifier
{
	typedef simple_bitmap_modifier base;

public:
	bitmap_matte_invert(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	struct functor
	{
		k3d::pixel operator()(const k3d::pixel& Input)
		{
			return k3d::pixel(
				boost::gil::get_color(Input, boost::gil::red_t()),
				boost::gil::get_color(Input, boost::gil::green_t()),
				boost::gil::get_color(Input, boost::gil::blue_t()),
				boost::gil::channel_invert(boost::gil::get_color(Input, boost::gil::alpha_t())));
		}
	};

	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		boost::gil::transform_pixels(const_view(Input), view(Output), functor());
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_matte_invert,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0x6856b0e9, 0x36b645d9, 0xb324d58b, 0x624072e9),
				"BitmapMatteInvert",
				_("Inverts a bitmap's alpha channel"),
				"Bitmap",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bitmap_matte_invert_factory

k3d::iplugin_factory& bitmap_matte_invert_factory()
{
	return bitmap_matte_invert::get_factory();
}

} // namespace libk3dbitmap

