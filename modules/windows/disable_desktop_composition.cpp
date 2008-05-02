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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/log.h>
#include <k3dsdk/types.h>
#include <k3dsdk/win32.h>

#include <iostream>

namespace module
{

namespace windows
{

/// Disables desktop composition on Vista when instantiated.
/// Thanks to Yaroukh from the forum and someone with nick "Wojta" for pointing out "Desktop Composition" was the problem.
class disable_desktop_composition : public k3d::iunknown
{
	typedef HRESULT (*DwmEnableComposition_t)(k3d::uint_t);
public:
	disable_desktop_composition()
	{
		HINSTANCE library = ::LoadLibrary("dwmapi.dll");
		if (library != 0)
		{
			DwmEnableComposition_t DwmEnableComposition = DwmEnableComposition_t(::GetProcAddress(library, "DwmEnableComposition"));
			if (DwmEnableComposition != 0)
			{
				HRESULT result = DwmEnableComposition(0);
				if (result != S_OK)
				{
					k3d::log() << error << "Failed to disable Vista Desktop Composition" << std::endl;
				}
			}
			else 
			{
				k3d::log() << error << "Failed to find the DwmEnableComposition function" << std::endl; 
			}
			::FreeLibrary(library);
		}
	}
	
	static k3d::iplugin_factory& get_factory()
		{
			k3d::iplugin_factory::metadata_t metadata;
			metadata["k3d:application-start"] = "";
			static k3d::application_plugin_factory<disable_desktop_composition,
				k3d::interface_list<k3d::iunknown> > factory(
					k3d::uuid(0xecd15ac7, 0x4abf42ca, 0x9ec918a4, 0xbdf4b9de),
					"DisableDesktopComposition",
					_("Disables Desktop Composition on Vista"),
					"Desktop",
					k3d::iplugin_factory::EXPERIMENTAL,
					metadata);

			return factory;
		}
};

k3d::iplugin_factory& disable_desktop_composition_factory()
{
	return disable_desktop_composition::get_factory();
}

} // namespace windows

} // namespace module
