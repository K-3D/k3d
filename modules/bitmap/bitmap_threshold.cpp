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
		\author Anders Dahnielson (anders@dahnielson.com)
*/

#include "simple_bitmap_modifier.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>

namespace libk3dbitmap
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_threshold

class bitmap_threshold :
	public simple_bitmap_modifier
{
	typedef simple_bitmap_modifier base;

public:
	bitmap_threshold(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_red_threshold(init_owner(*this) + init_name("red_threshold") + init_label(_("Red threshold")) + init_description(_("Clamp Red channel to threshold")) + init_value(0.0)),
		m_green_threshold(init_owner(*this) + init_name("geen_threshold") + init_label(_("Green threshold")) + init_description(_("Clamp Green channel to threshold")) + init_value(0.0)),
		m_blue_threshold(init_owner(*this) + init_name("blue_threshold") + init_label(_("Blue threshold")) + init_description(_("Clamp Blue channel to threshold")) + init_value(0.0)),
		m_alpha_threshold(init_owner(*this) + init_name("alpha_threshold") + init_label(_("Alpha threshold")) + init_description(_("Clamp Alpha channel to threshold")) + init_value(0.0))
	{
		m_red_threshold.changed_signal().connect(make_update_bitmap_slot());
		m_green_threshold.changed_signal().connect(make_update_bitmap_slot());
		m_blue_threshold.changed_signal().connect(make_update_bitmap_slot());
		m_alpha_threshold.changed_signal().connect(make_update_bitmap_slot());
	}

	struct functor
	{
		functor(const double RedThreshold, const double GreenThreshold, const double BlueThreshold, const double AlphaThreshold) :
			red_threshold(RedThreshold),
			green_threshold(GreenThreshold),
			blue_threshold(BlueThreshold),
			alpha_threshold(AlphaThreshold)
		{
		}

		k3d::pixel operator()(const k3d::pixel& Input) const
		{

			return k3d::pixel(
				std::max(red_threshold, static_cast<double>(boost::gil::get_color(Input, boost::gil::red_t()))),
				std::max(green_threshold, static_cast<double>(boost::gil::get_color(Input, boost::gil::green_t()))),
				std::max(blue_threshold, static_cast<double>(boost::gil::get_color(Input, boost::gil::blue_t()))),
				std::max(alpha_threshold, static_cast<double>(boost::gil::get_color(Input, boost::gil::alpha_t()))));
		}

		const double red_threshold;
		const double green_threshold;
		const double blue_threshold;
		const double alpha_threshold;
	};

	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		boost::gil::transform_pixels(const_view(Input), view(Output), functor(m_red_threshold.value(), m_green_threshold.value(), m_blue_threshold.value(), m_alpha_threshold.value()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_threshold,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0xcc8d625b, 0x970d4293, 0xa304d906, 0x81201d28),
				"BitmapThreshold",
				_("Cut off chanels at a certain value"),
				"Bitmap",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_red_threshold;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_green_threshold;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blue_threshold;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_alpha_threshold;
};

/////////////////////////////////////////////////////////////////////////////
// bitmap_threshold_factory

k3d::iplugin_factory& bitmap_threshold_factory()
{
	return bitmap_threshold::get_factory();
}

} // namespace libk3dbitmap

