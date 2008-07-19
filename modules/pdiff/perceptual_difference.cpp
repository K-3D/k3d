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
          \author Tim Shead (tshead@k-3d.com)
*/

#include "CompareArgs.h"
#include "Metric.h"
#include "RGBAImage.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/bitmap.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/ibitmap_source.h>
#include <k3dsdk/pointer_demand_storage.h>
#include <k3dsdk/value_demand_storage.h>
#include <k3dsdk/node.h>

namespace module
{

namespace pdiff
{

/////////////////////////////////////////////////////////////////////////////
// perceptual_difference

/// Computes the a difference metric between two images based on the physiology of human vision.
class perceptual_difference :
	public k3d::node,
	public k3d::ibitmap_source
{
	typedef k3d::node base;

public:
	perceptual_difference(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_bitmap_a(init_owner(*this) + init_name("input_a") + init_label(_("Input A")) + init_description(_("A input bitmap")) + init_value<k3d::bitmap*>(0)),
		m_bitmap_b(init_owner(*this) + init_name("input_b") + init_label(_("Input B")) + init_description(_("B input bitmap")) + init_value<k3d::bitmap*>(0)),
		m_field_of_view(init_owner(*this) + init_name("field_of_view") + init_label(_("Field-of-view")) + init_description(_("Field-of-view (degrees)")) + init_value(45.0) + init_step_increment(0.01)),
		m_gamma(init_owner(*this) + init_name("gamma") + init_label(_("Gamma")) + init_description(_("Gamma")) + init_value(2.2) + init_step_increment(0.01)),
		m_luminance(init_owner(*this) + init_name("luminance") + init_label(_("Luminance")) + init_description(_("Display Luminance (candela per square meter)")) + init_value(100.0) + init_step_increment(1.0)),
		m_difference(init_owner(*this) + init_name("difference") + init_label(_("Difference")) + init_description(_("The count of perceivably-different pixels")) + init_value(std::numeric_limits<k3d::uint32_t>::max())),
		m_output_bitmap(init_owner(*this) + init_name("output_bitmap") + init_label(_("Output Bitmap")) + init_description(_("Output bitmap")))
	{
		m_difference.set_update_slot(sigc::mem_fun(*this, &perceptual_difference::difference_execute));
		m_output_bitmap.set_update_slot(sigc::mem_fun(*this, &perceptual_difference::bitmap_execute));

		m_bitmap_a.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_difference.make_slot()));
		m_bitmap_b.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_difference.make_slot()));
		m_field_of_view.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_difference.make_slot()));
		m_gamma.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_difference.make_slot()));
		m_luminance.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_difference.make_slot()));

		m_bitmap_a.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_output_bitmap.make_slot()));
		m_bitmap_b.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_output_bitmap.make_slot()));
		m_field_of_view.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_output_bitmap.make_slot()));
		m_gamma.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_output_bitmap.make_slot()));
		m_luminance.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_output_bitmap.make_slot()));

	}

	k3d::iproperty& bitmap_source_output()
	{
		return m_output_bitmap;
	}

	void difference_execute(const std::vector<k3d::ihint*>& Hints, k3d::uint32_t& Difference)
	{
		Difference = std::numeric_limits<k3d::uint32_t>::max();

		k3d::bitmap* const bitmap_a = m_bitmap_a.pipeline_value();
		if(!bitmap_a)
			return;

		k3d::bitmap* const bitmap_b = m_bitmap_b.pipeline_value();
		if(!bitmap_b)
			return;

		if(bitmap_a->width() != bitmap_b->width() || bitmap_a->height() != bitmap_b->height())
			return;

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
	
		Difference = args.FailedPixels;
	}

	void bitmap_execute(const std::vector<k3d::ihint*>& Hints, k3d::bitmap& Bitmap)
	{
		k3d::bitmap* const bitmap_a = m_bitmap_a.pipeline_value();
		if(!bitmap_a)
			return;

		k3d::bitmap* const bitmap_b = m_bitmap_b.pipeline_value();
		if(!bitmap_b)
			return;

		if(bitmap_a->width() != bitmap_b->width() || bitmap_a->height() != bitmap_b->height())
			return;

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

		convert(*args.ImgDiff, Bitmap);
	}

	static RGBAImage* convert(k3d::bitmap& Source)
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

	static void convert(RGBAImage& Source, k3d::bitmap& Destination)
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

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<perceptual_difference,
				k3d::interface_list<k3d::ibitmap_source > > factory(
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
	k3d_data(k3d::uint32_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_difference;
	k3d_data(k3d::bitmap*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output_bitmap;
};

/////////////////////////////////////////////////////////////////////////////
// perceptual_difference_factory

k3d::iplugin_factory& perceptual_difference_factory()
{
	return perceptual_difference::get_factory();
}

} // namespace pdiff

} // namespace module

