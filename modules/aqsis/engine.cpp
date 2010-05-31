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
#include <k3dsdk/istreaming_bitmap_source.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/signal_system.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>
#include <sstream>

#include <aqsis/math/region.h>
#include <libs/core/ddmanager/iddmanager.h>

namespace module
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
// engine

class engine :
	public k3d::istreaming_bitmap_source
{
public:
	virtual sigc::connection connect_bitmap_start_signal(const sigc::slot<void, dimension, dimension>& Slot)
	{
		return bitmap_start_signal.connect(Slot);
	}

	virtual sigc::connection connect_bitmap_bucket_signal(const sigc::slot<void, const bucket&>& Slot)
	{
		return bitmap_bucket_signal.connect(Slot);
	}

	virtual sigc::connection connect_bitmap_finish_signal(const sigc::slot<void>& Slot)
	{
		return bitmap_finish_signal.connect(Slot);
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
	sigc::signal<void, dimension, dimension> bitmap_start_signal;
	sigc::signal<void, const bucket&> bitmap_bucket_signal;
	sigc::signal<void> bitmap_finish_signal;
};

} // namespace aqsis

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::aqsis::engine::get_factory());
K3D_MODULE_END

