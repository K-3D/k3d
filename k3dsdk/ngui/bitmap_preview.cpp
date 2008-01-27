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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "bitmap_preview.h"

#include <k3dsdk/properties.h>

#include <gdkmm/pixbuf.h>
#include <gtkmm/image.h>

#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>

namespace libk3dngui
{

namespace bitmap_preview
{

/// Specialization of bitmap_preview::data_proxy for use with iproperty objects
class property_proxy :
	public idata_proxy
{
public:
	property_proxy(k3d::iproperty& Data) :
		m_readable_data(Data)
	{
	}

	k3d::bitmap* value()
	{
		// As a special-case, if the internal property value is NULL, return the connected property (if any)
		k3d::bitmap* result = boost::any_cast<k3d::bitmap*>(m_readable_data.property_internal_value());
		if(!result)
			result = k3d::property::pipeline_value<k3d::bitmap*>(m_readable_data);

		return result;
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

private:
	property_proxy(const property_proxy& RHS);
	property_proxy& operator=(const property_proxy& RHS);

	k3d::iproperty& m_readable_data;
};

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data)
{
	return std::auto_ptr<idata_proxy>(new property_proxy(Data));
}

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	base(Gtk::BUTTONBOX_START, 2),
	ui_component(Name, &Parent),
	m_image_buffer(64, 64),
	m_alpha_buffer(64, 64),
	m_image(new Gtk::Image()),
	m_alpha(new Gtk::Image()),
	m_data(Data)
{
	pack_start(*manage(m_image), Gtk::PACK_SHRINK);
	pack_start(*manage(m_alpha), Gtk::PACK_SHRINK);

	data_changed(0);

	if(m_data.get())
		m_data->changed_signal().connect(sigc::mem_fun(*this, &control::data_changed));
}

struct extract_rgb
{
	template<typename P1, typename P2>
	void operator()(const P1& src, P2& dst) const
	{
		get_color(dst, boost::gil::red_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::red_t()));
		get_color(dst, boost::gil::green_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::green_t()));
		get_color(dst, boost::gil::blue_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::blue_t()));
	}
};

struct extract_alpha
{
	template<typename P1, typename P2>
	void operator()(const P1& src, P2& dst) const
	{
		get_color(dst, boost::gil::red_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::alpha_t()));
		get_color(dst, boost::gil::green_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::alpha_t()));
		get_color(dst, boost::gil::blue_t()) = boost::gil::channel_convert<boost::gil::bits8>(get_color(src, boost::gil::alpha_t()));
	}
};

void control::data_changed(k3d::iunknown*)
{
	return_if_fail(m_data.get());

	const boost::gil::rgb8_image_t::view_t& image = boost::gil::view(m_image_buffer);
	const boost::gil::rgb8_image_t::view_t& alpha = boost::gil::view(m_alpha_buffer);

	k3d::bitmap* const data = m_data->value();
	if(data && data->width() && data->height())
	{
		boost::gil::resize_view(
			boost::gil::color_converted_view<boost::gil::rgb8_pixel_t>(boost::gil::const_view(*data), extract_rgb()),
			image,
			boost::gil::bilinear_sampler());

		boost::gil::resize_view(
			boost::gil::color_converted_view<boost::gil::rgb8_pixel_t>(boost::gil::const_view(*data), extract_alpha()),
			alpha,
			boost::gil::bilinear_sampler());
	}
	else
	{
		k3d::checkerboard_fill(
			image,
			8,
			8,
			boost::gil::rgb8_pixel_t(32, 32, 32),
			boost::gil::rgb8_pixel_t(210, 210, 210));

		std::fill(alpha.begin(), alpha.end(), boost::gil::rgb8_pixel_t(255, 255, 255));
	}

	m_image->set(Gdk::Pixbuf::create_from_data(reinterpret_cast<guint8*>(&image[0]),
		Gdk::COLORSPACE_RGB,
		false,
		8,
		image.width(),
		image.height(),
		image.width() * 3));

	m_alpha->set(Gdk::Pixbuf::create_from_data(reinterpret_cast<guint8*>(&alpha[0]),
		Gdk::COLORSPACE_RGB,
		false,
		8,
		alpha.width(),
		alpha.height(),
		alpha.width() * 3));
}

} // namespace bitmap_preview

} // namespace k3d

