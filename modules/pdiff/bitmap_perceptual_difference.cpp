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

#include "CompareArgs.h"
#include "Metric.h"
#include "RGBAImage.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/bitmap.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/ibitmap_sink.h>
#include <k3dsdk/ibitmap_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace module
{

namespace pdiff
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_perceptual_difference

class bitmap_perceptual_difference :
	public k3d::persistent<k3d::node>,
	public k3d::ibitmap_source,
	public k3d::ibitmap_sink
{
	typedef k3d::persistent<k3d::node> base;

public:
	bitmap_perceptual_difference(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_bitmap_a(init_owner(*this) + init_name("input_a") + init_label(_("Input A")) + init_description(_("A input bitmap")) + init_value<k3d::bitmap*>(0)),
		m_bitmap_b(init_owner(*this) + init_name("input_b") + init_label(_("Input B")) + init_description(_("B input bitmap")) + init_value<k3d::bitmap*>(0)),
		m_field_of_view(init_owner(*this) + init_name("field_of_view") + init_label(_("Field-of-view")) + init_description(_("Field-of-view (degrees)")) + init_value(45.0) + init_step_increment(0.01)),
		m_gamma(init_owner(*this) + init_name("gamma") + init_label(_("Gamma")) + init_description(_("Gamma")) + init_value(2.2) + init_step_increment(0.01)),
		m_luminance(init_owner(*this) + init_name("luminance") + init_label(_("Luminance")) + init_description(_("Display Luminance (candela per square meter)")) + init_value(100.0) + init_step_increment(1.0)),
		m_difference(init_owner(*this) + init_name("difference") + init_label(_("Difference")) + init_description(_("The count of perceivably-different pixels")) + init_slot(sigc::mem_fun(*this, &bitmap_perceptual_difference::get_difference_pixels))),
		m_bitmap_o(init_owner(*this) + init_name("output") + init_label(_("Output Bitmap")) + init_description(_("Output bitmap")) + init_slot(sigc::mem_fun(*this, &bitmap_perceptual_difference::get_difference_image))),
		m_difference_pixels(std::numeric_limits<unsigned long>::max()),
		m_difference_image(0)
	{
		m_bitmap_a.changed_signal().connect(make_reset_output_slot());
		m_bitmap_b.changed_signal().connect(make_reset_output_slot());
		m_field_of_view.changed_signal().connect(make_reset_output_slot());
		m_gamma.changed_signal().connect(make_reset_output_slot());
		m_luminance.changed_signal().connect(make_reset_output_slot());
	}

	k3d::iproperty& bitmap_source_output()
	{
		return m_bitmap_o;
	}

	k3d::iproperty& bitmap_sink_input()
	{
		return m_bitmap_a;
	}

	sigc::slot<void, iunknown*> make_reset_output_slot()
	{
		return sigc::mem_fun(*this, &bitmap_perceptual_difference::reset_output);
	}

	void reset_output(k3d::iunknown* Hint)
	{
		m_difference_pixels = std::numeric_limits<unsigned long>::max();

		delete m_difference_image;
		m_difference_image = 0;

		m_difference.reset(Hint);
		m_bitmap_o.reset(0, Hint);
	}

	RGBAImage* convert(k3d::bitmap& Source)
	{
		RGBAImage* const result = new RGBAImage(Source.width(), Source.height());

		const k3d::bitmap::view_t& source = view(Source);

		unsigned int i = 0;
		const k3d::bitmap::view_t::iterator begin = source.begin();
		const k3d::bitmap::view_t::iterator end = source.end();
		for(k3d::bitmap::view_t::iterator pixel = begin; pixel != end; ++pixel, ++i)
		{
			result->Set(
				boost::gil::channel_convert<boost::gil::bits8>(get_color(*pixel, boost::gil::red_t())),
				boost::gil::channel_convert<boost::gil::bits8>(get_color(*pixel, boost::gil::green_t())),
				boost::gil::channel_convert<boost::gil::bits8>(get_color(*pixel, boost::gil::blue_t())),
				boost::gil::channel_convert<boost::gil::bits8>(get_color(*pixel, boost::gil::alpha_t())),
				i
				);
		}

		return result;
	}

	void convert(RGBAImage& Source, k3d::bitmap& Destination)
	{
		Destination.recreate(Source.Get_Width(), Source.Get_Height());

		const k3d::bitmap::view_t& destination = view(Destination);

		unsigned int i = 0;
		const k3d::bitmap::view_t::iterator begin = destination.begin();
		const k3d::bitmap::view_t::iterator end = destination.end();
		for(k3d::bitmap::view_t::iterator pixel = begin; pixel != end; ++pixel, ++i)
		{
			get_color(*pixel, boost::gil::red_t()) = boost::gil::channel_convert<boost::gil::bits16f>(Source.Get_Red(i));
			get_color(*pixel, boost::gil::green_t()) = boost::gil::channel_convert<boost::gil::bits16f>(Source.Get_Green(i));
			get_color(*pixel, boost::gil::blue_t()) = boost::gil::channel_convert<boost::gil::bits16f>(Source.Get_Blue(i));
			get_color(*pixel, boost::gil::alpha_t()) = boost::gil::channel_convert<boost::gil::bits16f>(Source.Get_Alpha(i));
		}
	
	}

	void update_output()
	{
		if(m_difference_image)
			return;

		k3d::bitmap* const bitmap_a = m_bitmap_a.pipeline_value();
		if(!bitmap_a)
		{
			m_difference_pixels = std::numeric_limits<unsigned long>::max();
			return;
		}

		k3d::bitmap* const bitmap_b = m_bitmap_b.pipeline_value();
		if(!bitmap_b)
		{
			m_difference_pixels = std::numeric_limits<unsigned long>::max();
			return;
		}

		if(bitmap_a->width() != bitmap_b->width() || bitmap_a->height() != bitmap_b->height())
		{
			m_difference_pixels = std::numeric_limits<unsigned long>::max();
			return;
		}

		CompareArgs args;
		args.ImgA = convert(*bitmap_a);
		args.ImgB = convert(*bitmap_b);
		args.ImgDiff = new RGBAImage(bitmap_a->width(), bitmap_a->height());
		args.Verbose = false;
		args.FieldOfView = m_field_of_view.pipeline_value();
		args.Gamma = m_gamma.pipeline_value();
		args.Luminance = m_luminance.pipeline_value();
		args.ThresholdPixels = 0;

		Yee_Compare(args);
		
		m_difference_pixels = args.FailedPixels;
		m_difference_image = args.ImgDiff;

		args.ImgDiff = 0;
	}

	unsigned long get_difference_pixels()
	{
		update_output();
		return m_difference_pixels;
	}

	void get_difference_image(k3d::bitmap& Bitmap)
	{
		update_output();

		if(m_difference_image)
			convert(*m_difference_image, Bitmap);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_perceptual_difference,
				k3d::interface_list<k3d::ibitmap_source,
				k3d::interface_list<k3d::ibitmap_sink> > > factory(
					k3d::uuid(0x2f0ffccf, 0xaa40e2f3, 0xb5221a9f, 0x20131d9f),
					"BitmapPerceptualDifference",
					_("Calculate a perceptual difference metric between two bitmap images"),
					"Bitmap Test",
					k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bitmap*, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_bitmap_a;
	k3d_data(k3d::bitmap*, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_bitmap_b;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_field_of_view;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gamma;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_luminance;
	k3d_data(k3d::int32_t, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_difference;
	k3d_data(k3d::bitmap*, immutable_name, change_signal, no_undo, demand_storage, no_constraint, read_only_property, no_serialization) m_bitmap_o;

	unsigned long m_difference_pixels;
	RGBAImage* m_difference_image;
};

/////////////////////////////////////////////////////////////////////////////
// bitmap_perceptual_difference_factory

k3d::iplugin_factory& bitmap_perceptual_difference_factory()
{
	return bitmap_perceptual_difference::get_factory();
}

} // namespace pdiff

} // namespace module

