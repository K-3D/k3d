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

#include <k3dsdk/bitmap_source.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <iterator>

namespace module
{

namespace bitmap
{

/////////////////////////////////////////////////////////////////////////////
// checker

class checker :
	public k3d::persistent<k3d::node>,
	public k3d::bitmap_source<checker>
{
	typedef k3d::persistent<k3d::node> base;

public:
	checker(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Bitmap width")) + init_value(64L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Bitmap height")) + init_value(64L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_check_width(init_owner(*this) + init_name("check_width") + init_label(_("Check width")) + init_description(_("Check (rectangle) width")) + init_value(8L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_check_height(init_owner(*this) + init_name("check_height") + init_label(_("Check height")) + init_description(_("Check (rectangle) height")) + init_value(8L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_color1(init_owner(*this) + init_name("color1") + init_label(_("Color 1")) + init_description(_("First check color")) + init_value(k3d::color(1, 1, 1))),
		m_color2(init_owner(*this) + init_name("color2") + init_label(_("Color 2")) + init_description(_("Second check color")) + init_value(k3d::color(0, 0, 0)))
	{
		m_width.changed_signal().connect(make_reset_bitmap_slot());
		m_height.changed_signal().connect(make_reset_bitmap_slot());
		m_check_width.changed_signal().connect(make_reset_bitmap_slot());
		m_check_height.changed_signal().connect(make_reset_bitmap_slot());
		m_color1.changed_signal().connect(make_reset_bitmap_slot());
		m_color2.changed_signal().connect(make_reset_bitmap_slot());
	}

	void on_create_bitmap(k3d::bitmap& Bitmap)
	{
		const k3d::pixel_size_t width = m_width.pipeline_value();
		const k3d::pixel_size_t height = m_height.pipeline_value();
		const k3d::pixel_size_t check_width = m_check_width.pipeline_value();
		const k3d::pixel_size_t check_height = m_check_height.pipeline_value();
		const k3d::color color1 = m_color1.pipeline_value();
		const k3d::color color2 = m_color2.pipeline_value();

		Bitmap.recreate(width, height);
		k3d::checkerboard_fill(view(Bitmap), check_width, check_height, k3d::pixel(color1.red, color1.green, color1.blue, 1.0), k3d::pixel(color2.red, color2.green, color2.blue, 1.0));
	}

	void on_update_bitmap(k3d::bitmap& Bitmap)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<checker,
			k3d::interface_list<k3d::ibitmap_source> > factory(
				k3d::uuid(0x8d65a65f, 0x853e4460, 0xbc2ecbad, 0xce65b404),
				"BitmapChecker",
				_("Generates a checker-pattern bitmap"),
				"Bitmap",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_check_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_check_height;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color1;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color2;
};

/////////////////////////////////////////////////////////////////////////////
// checker_factory

k3d::iplugin_factory& checker_factory()
{
	return checker::get_factory();
}

} // namespace bitmap

} // namespace module

