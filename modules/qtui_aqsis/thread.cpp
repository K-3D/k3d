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

#include "thread.h"

#include <k3dsdk/log.h>
#include <k3dsdk/signal_system.h>

#include <vector>

#include <aqsis/ri/ri.h>
#include <aqsis/math/region.h>
#include <libs/core/ddmanager/iddmanager.h>
#include <libs/core/renderer.h>

#include <QRect>

namespace module
{

namespace qtui
{

namespace aqsis
{

/////////////////////////////////////////////////////////////////////////////
// display_manager

/// Custom Aqsis display manager that forwards display data using signals
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

/////////////////////////////////////////////////////////////////////////////
// thread

void thread::run()
{
	static RtFloat fov = 45, intensity = 0.5;
	static RtFloat Ka = 0.5, Kd = 0.8, Ks = 0.2;
	static RtPoint from = {0,0,1}, to = {0,10,0};
	RiBegin(RI_NULL);

	display_manager* const manager = new display_manager();
	manager->open_display.connect(sigc::mem_fun(*this, &thread::on_open_display));
	manager->display_bucket.connect(sigc::mem_fun(*this, &thread::on_display_bucket));
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

void thread::on_open_display(int Width, int Height)
{
	Q_EMIT open_display(Width, Height);
}

void thread::on_display_bucket(const Aqsis::CqRegion& Region, const Aqsis::IqChannelBuffer* Buffer)
{
	Q_EMIT display_bucket(QRect(Region.xMin(), Region.yMin(), Region.width(), Region.height()), Buffer);
}

} // namespace aqsis

} // namespace qtui

} // namespace module

