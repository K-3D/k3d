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

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/imeta_object.h>
#include <k3dsdk/istreaming_bitmap_source.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/result.h>
#include <k3dsdk/signal_system.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>
#include <sstream>

#include <aqsis/ri/ri.h>
#include <aqsis/math/region.h>
#include <libs/core/ddmanager/iddmanager.h>
#include <libs/core/renderer.h>

namespace module
{

namespace aqsis
{

typedef sigc::signal<void, k3d::istreaming_bitmap_source::coordinate, k3d::istreaming_bitmap_source::coordinate> bitmap_start_signal_t;
typedef sigc::signal<void, k3d::istreaming_bitmap_source::coordinate, k3d::istreaming_bitmap_source::coordinate, const k3d::istreaming_bitmap_source::bucket&> bitmap_bucket_signal_t;
typedef sigc::signal<void> bitmap_finish_signal_t;

/////////////////////////////////////////////////////////////////////////////
// display_manager

/// Custom Aqsis display manager that forwards display data using signals
class display_manager : public Aqsis::IqDDManager
{
public:
	display_manager(bitmap_start_signal_t& BitmapStartSignal, bitmap_bucket_signal_t& BitmapBucketSignal, bitmap_finish_signal_t BitmapFinishSignal) :
		bitmap_start_signal(BitmapStartSignal),
		bitmap_bucket_signal(BitmapBucketSignal),
		bitmap_finish_signal(BitmapFinishSignal)
	{
	}

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
		return 0;
	}

	TqInt ClearDisplays()
	{
		return 0;
	}

	TqInt OpenDisplays(TqInt width, TqInt height)
	{
		bitmap_start_signal.emit(width, height);
		return 0;
	}

	TqInt CloseDisplays()
	{
		bitmap_finish_signal.emit();
		return 0;
	}

	TqInt DisplayBucket(const Aqsis::CqRegion& Region, const Aqsis::IqChannelBuffer* Buffer)
	{
		bitmap.recreate(Buffer->width(), Buffer->height());
		k3d::istreaming_bitmap_source::bitmap::view_t bucket = view(bitmap);

		const int index = Buffer->getChannelIndex("rgb");
		for(int y = 0; y != Buffer->height(); ++y)
		{
			for(int x = 0; x != Buffer->width(); ++x)
			{
				Aqsis::IqChannelBuffer::TqConstChannelPtr values = (*Buffer)(x, y, index);
				*bucket.at(x, y) = k3d::istreaming_bitmap_source::pixel(values[0], values[1], values[2], values[3]);
			}
		}

		bitmap_bucket_signal.emit(Region.xMin(), Region.yMin(), bucket);
		return 0;
	}

	bool fDisplayNeeds(const TqChar* var)
	{
		if(std::string(var) == "rgba")
			return true;
		if(std::string(var) == "Ci")
			return true;
		if(std::string(var) == "Oi")
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

	bitmap_start_signal_t& bitmap_start_signal;
	bitmap_bucket_signal_t& bitmap_bucket_signal;
	bitmap_finish_signal_t& bitmap_finish_signal;

	k3d::istreaming_bitmap_source::bitmap bitmap;
};

/////////////////////////////////////////////////////////////////////////////
// engine

class engine :
	public k3d::istreaming_bitmap_source,
	public k3d::imeta_object
{
public:
	virtual sigc::connection connect_bitmap_start_signal(const sigc::slot<void, coordinate, coordinate>& Slot)
	{
		return bitmap_start_signal.connect(Slot);
	}

	virtual sigc::connection connect_bitmap_bucket_signal(const sigc::slot<void, coordinate, coordinate, const bucket&>& Slot)
	{
		return bitmap_bucket_signal.connect(Slot);
	}

	virtual sigc::connection connect_bitmap_finish_signal(const sigc::slot<void>& Slot)
	{
		return bitmap_finish_signal.connect(Slot);
	}

	virtual const boost::any execute(const k3d::string_t& Command, const std::vector<boost::any>& Arguments)
	{
		return_val_if_fail(Command == "render", boost::any());

		static RtFloat fov = 45, intensity = 0.5;
		static RtFloat Ka = 0.5, Kd = 0.8, Ks = 0.2;
		static RtPoint from = {0,0,1}, to = {0,10,0};
		RiBegin(RI_NULL);

		display_manager* const manager = new display_manager(bitmap_start_signal, bitmap_bucket_signal, bitmap_finish_signal);
		Aqsis::QGetRenderContext()->SetDisplayManager(manager);

		RiFormat(512, 512, 1);
		RiPixelSamples(2, 2);
		RiFrameBegin(1);
		RiDisplay("test1.tiff", "framebuffer", "rgba", RI_NULL);
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

		return boost::any();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<engine> factory(
			k3d::uuid(0xe01447a1, 0x9642d8f8, 0xb5d73db8, 0xccaf2bf7),
			"EmbeddedAqsisRenderManEngine",
			_("Provides an embedded Aqsis RenderMan engine."),
			"RenderMan",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	bitmap_start_signal_t bitmap_start_signal;
	bitmap_bucket_signal_t bitmap_bucket_signal;
	bitmap_finish_signal_t bitmap_finish_signal;
};

} // namespace aqsis

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::aqsis::engine::get_factory());
K3D_MODULE_END

