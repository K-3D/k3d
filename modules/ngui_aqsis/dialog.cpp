// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/bitmap.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/result.h>
#include <k3dsdk/user_interface.h>

#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <aqsis/ri/ri.h>
#include <aqsis/math/region.h>

#include <libs/core/ddmanager/iddmanager.h>
#include <libs/core/renderer.h>

#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace aqsis
{

class display_manager : public Aqsis::IqDDManager
{
public:
	sigc::signal<void, int, int> open_display;
	sigc::signal<void, const Aqsis::CqRegion&, const Aqsis::IqChannelBuffer*> display_bucket;

private:
	TqInt Initialise()
	{
		return 0;
	}

	TqInt Shutdown()
	{
		return 0;
	}

	TqInt AddDisplay(const TqChar* name, const TqChar* type, const TqChar* mode, TqInt modeID, TqInt dataOffset, TqInt dataSize, std::map<std::string, void*> mapOfArguments)
	{
		k3d::log() << debug << __PRETTY_FUNCTION__ << " " << name << " " << type << " " << mode << std::endl;
		return 0;
	}

	TqInt ClearDisplays()
	{
		return 0;
	}

	TqInt OpenDisplays(TqInt width, TqInt height)
	{
		open_display.emit(width, height);
		return 0;
	}

	TqInt CloseDisplays()
	{
		return 0;
	}

	TqInt DisplayBucket(const Aqsis::CqRegion& DRegion, const Aqsis::IqChannelBuffer* pBuffer)
	{
		display_bucket(DRegion, pBuffer);
		return 0;
	}

	bool fDisplayNeeds(const TqChar* var)
	{
		if(std::string(var) == "rgb")
			return true;
		if(std::string(var) == "Ci")
			return true;
		return false;
	}

	TqInt Uses()
	{
		return 0;
	}

	TqInt numDisplayRequests()
	{
		return 0;
	}

	boost::shared_ptr<Aqsis::IqDisplayRequest> displayRequest(TqInt index)
	{
		return boost::shared_ptr<Aqsis::IqDisplayRequest>();
	}
};

class dialog :
	public application_window,
	public k3d::iunknown
{
	typedef application_window base;

public:
	dialog()
	{
		Gtk::Label* const label = new Gtk::Label(_("Rendering using in-process Aqsis ...\nClose window to cancel rendering."));
		image_widget = new Gtk::Image();

		Gtk::VBox* const box = new Gtk::VBox();
		box->pack_start(*manage(label), Gtk::PACK_SHRINK);
		box->pack_start(*manage(image_widget), Gtk::PACK_EXPAND_WIDGET);

		add(*Gtk::manage(box));

		set_border_width(10);

		set_role("aqsis");
		show_all();

		static RtFloat fov = 45, intensity = 0.5;
		static RtFloat Ka = 0.5, Kd = 0.8, Ks = 0.2;
		static RtPoint from = {0,0,1}, to = {0,10,0};
		RiBegin(RI_NULL);

		display_manager* const manager = new display_manager();
		manager->open_display.connect(sigc::mem_fun(*this, &dialog::on_open_display));
		manager->display_bucket.connect(sigc::mem_fun(*this, &dialog::on_display_bucket));
		Aqsis::QGetRenderContext()->SetDisplayManager(manager);

		RiFormat(512, 512, 1);
		RiPixelSamples(2, 2);
		RiFrameBegin(1);
		RiDisplay("test1.tiff", "framebuffer", "rgb", RI_NULL);
		RiProjection("perspective", "fov", &fov, RI_NULL);
		RiRotate(-116.344, 0, 0, 1);
		RiRotate(-47.9689, 1, 0, 0);
		RiRotate(-123.69, 0, 1, 0);
		RiTranslate(15, -20, -10);
		RiWorldBegin();
		RiSphere(5, -5, 5, 360, RI_NULL);
		RiWorldEnd();
		RiFrameEnd();
		RiEnd();
	}

	void on_open_display(int Width, int Height)
	{
		image.recreate(Width, Height);

		image_widget->set(Gdk::Pixbuf::create_from_data(reinterpret_cast<const guint8*>(&const_view(image)[0]),
			Gdk::COLORSPACE_RGB,
			false,
			8,
			image.width(),
			image.height(),
			image.width() * 3));
	}

	void on_display_bucket(const Aqsis::CqRegion& Region, const Aqsis::IqChannelBuffer* Buffer)
	{
		const int index = Buffer->getChannelIndex("rgb");

		const image_t::view_t& writable_image = view(image);
		for(int y = 0; y != Buffer->height(); ++y)
		{
			for(int x = 0; x != Buffer->width(); ++x)
			{
				Aqsis::IqChannelBuffer::TqConstChannelPtr values = (*Buffer)(x, y, index);
				double red = k3d::clamp(255.0 * values[0], 0.0, 255.0);
				double green = k3d::clamp(255.0 * values[1], 0.0, 255.0);
				double blue = k3d::clamp(255.0 * values[2], 0.0, 255.0);
				*writable_image.at(x + Region.xMin(), y + Region.yMin()) = boost::gil::rgb8_pixel_t(red, green, blue);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xa467cfdd, 0x2545d997, 0x07ecf8a2, 0x042455fb),
			"NGUIAqsis",
			_("Provides an embedded Aqsis RenderMan render engine."),
			"NGUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "dialog")
			);

		return factory;
	}

	typedef boost::gil::rgb8_image_t image_t;
	image_t image;
	Gtk::Image* image_widget;
};

} // namespace aqsis

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::aqsis::dialog::get_factory());
K3D_MODULE_END

