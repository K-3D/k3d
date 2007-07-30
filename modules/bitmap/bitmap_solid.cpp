// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/bitmap_source.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>

namespace libk3dbitmap
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_solid

class bitmap_solid :
	public k3d::bitmap_source<k3d::persistent<k3d::node> >
{
	typedef k3d::bitmap_source<k3d::persistent<k3d::node> > base;

public:
	bitmap_solid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Bitmap width")) + init_value(64L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(1L))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Bitmap height")) + init_value(64L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(1L))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Bitmap color")) + init_value(k3d::color(1, 1, 1)))
	{
		m_width.changed_signal().connect(make_reset_bitmap_slot());
		m_height.changed_signal().connect(make_reset_bitmap_slot());
		m_color.changed_signal().connect(make_reset_bitmap_slot());
	}

	void on_create_bitmap(k3d::bitmap& Bitmap)
	{
		const k3d::pixel_size_t width = m_width.pipeline_value();
		const k3d::pixel_size_t height = m_height.pipeline_value();
		const k3d::color color = m_color.pipeline_value();

		Bitmap.recreate(width, height);

		const k3d::bitmap::view_t& bitmap = boost::gil::view(Bitmap);
		std::fill(bitmap.begin(), bitmap.end(), k3d::pixel(color.red, color.green, color.blue, 1.0));
	}

	void on_update_bitmap(k3d::bitmap& Bitmap)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_solid,
			k3d::interface_list<k3d::ibitmap_source> > factory(
				k3d::uuid(0x3e3b43f0, 0xcd21465c, 0x9c099aba, 0x8dc117d8),
				"BitmapSolid",
				_("Generates a solid-color bitmap"),
				"Bitmap",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_width;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

/////////////////////////////////////////////////////////////////////////////
// bitmap_solid_factory

k3d::iplugin_factory& bitmap_solid_factory()
{
	return bitmap_solid::get_factory();
}

} // namespace libk3dbitmap
