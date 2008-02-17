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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>

namespace module
{

namespace bitmap
{

/////////////////////////////////////////////////////////////////////////////
// add

class add :
	public simple_modifier
{
	typedef simple_modifier base;

public:
	add(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_value(init_owner(*this) + init_name("value") + init_label(_("Add value")) + init_description(_("Add value to each pixel's Red, Green and Blue component")) + init_value(0.0))
	{
		m_value.changed_signal().connect(make_update_bitmap_slot());
	}

	struct functor
	{
		functor(const double Value) :
			value(Value)
		{
		}

		k3d::pixel operator()(const k3d::pixel& Input) const
		{
			return k3d::pixel(
				boost::gil::get_color(Input, boost::gil::red_t()) + value,
				boost::gil::get_color(Input, boost::gil::green_t()) + value,
				boost::gil::get_color(Input, boost::gil::blue_t()) + value,
				boost::gil::get_color(Input, boost::gil::alpha_t()));
		}

		const double value;
	};

	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		boost::gil::transform_pixels(const_view(Input), view(Output), functor(m_value.pipeline_value()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0x30d648f9, 0x74844196, 0x801a34f9, 0x6ff15920),
				"BitmapAdd",
				_("Add value to each pixel color component"),
				"Bitmap",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_value;
};

/////////////////////////////////////////////////////////////////////////////
// add

k3d::iplugin_factory& add_factory()
{
	return add::get_factory();
}

} // namespace bitmap

} // namespace module

