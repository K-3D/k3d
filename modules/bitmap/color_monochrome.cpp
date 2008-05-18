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
	\author Anders Dahnielson (anders@dahnielson.com)
*/

#include "simple_modifier.h"

#include <k3dsdk/ipipeline_profiler.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>

namespace module
{

namespace bitmap
{

/////////////////////////////////////////////////////////////////////////////
// color_monochrome

class color_monochrome :
	public simple_modifier
{
	typedef simple_modifier base;

public:
	color_monochrome(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_red_weight(init_owner(*this) + init_name("red_weight") + init_label(_("Red weight")) + init_description(_("Scale Red componnt value")) + init_value(0.3)),
		m_green_weight(init_owner(*this) + init_name("green_weight") + init_label(_("Green weight")) + init_description(_("Scale Green component value")) + init_value(0.59)),
		m_blue_weight(init_owner(*this) + init_name("blue_weight") + init_label(_("Blue weight")) + init_description(_("Scale Blue component value")) + init_value(0.11))
	{
		m_red_weight.changed_signal().connect(make_update_bitmap_slot());
		m_green_weight.changed_signal().connect(make_update_bitmap_slot());
		m_blue_weight.changed_signal().connect(make_update_bitmap_slot());
	}

	struct functor
	{
		functor(const double RedWeight, const double GreenWeight, const double BlueWeight) :
			red_weight(RedWeight),
			green_weight(GreenWeight),
			blue_weight(BlueWeight)
		{
		}

		k3d::pixel operator()(const k3d::pixel& Input) const
		{
			const double value = 
				red_weight * boost::gil::get_color(Input, boost::gil::red_t()) +
				green_weight * boost::gil::get_color(Input, boost::gil::green_t()) + 
				blue_weight * boost::gil::get_color(Input, boost::gil::blue_t());

			return k3d::pixel(
				value,
				value,
				value,
				boost::gil::get_color(Input, boost::gil::alpha_t()));
		}

		const double red_weight;
		const double green_weight;
		const double blue_weight;
	};

	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Update Bitmap");
        boost::gil::transform_pixels(const_view(Input), view(Output), functor(m_red_weight.pipeline_value(), m_green_weight.pipeline_value(), m_blue_weight.pipeline_value()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<color_monochrome,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0x73cd5356, 0x4d3d4eaf, 0xacda0d76, 0x8070a016),
				"BitmapColorMonochrome",
				_("Make any image black and white"),
				"Bitmap",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_red_weight;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_green_weight;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blue_weight;
};

/////////////////////////////////////////////////////////////////////////////
// color_monochrome_factory

k3d::iplugin_factory& color_monochrome_factory()
{
	return color_monochrome::get_factory();
}

} // namespace bitmap

} // namespace module


