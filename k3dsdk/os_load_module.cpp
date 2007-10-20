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

#include "log.h"
#include "os_load_module.h"
#include "path.h"
#include "result.h"

namespace k3d
{

//////////////////////////////////////////////////////////////////////////////
// os_load_module

#if defined K3D_API_WIN32

	#include "win32.h"

	void os_load_module(const filesystem::path& FilePath, register_plugins_entry_point& RegisterPlugins)
	{
		const UINT old_error_mode = SetErrorMode(SetErrorMode(SEM_FAILCRITICALERRORS)); // Disable error dialogs when loading DLLs
		HINSTANCE module = LoadLibrary(FilePath.native_filesystem_string().c_str());
		SetErrorMode(old_error_mode);

		if(!module)
		{
			log() << error << "Module [" << FilePath.native_console_string() << "] could not be loaded: error " << GetLastError() << std::endl;
			return;
		}

		RegisterPlugins = register_plugins_entry_point(GetProcAddress(module, "register_k3d_plugins"));
		if(!RegisterPlugins)
			RegisterPlugins = register_plugins_entry_point(GetProcAddress(module, "_register_k3d_plugins"));
		if(!RegisterPlugins)
		{
			log() << error << "Module " << FilePath.leaf().raw() << " does not contain required register_k3d_plugins() entry point" << std::endl;
			return;
		}
	}

#else // !K3D_API_WIN32

	#include <dlfcn.h>

	void os_load_module(const filesystem::path& FilePath, register_plugins_entry_point& RegisterPlugins)
	{
		void* module = dlopen(FilePath.native_filesystem_string().c_str(), RTLD_GLOBAL | RTLD_LAZY);
		if(!module)
		{
			log() << error << "Module " << FilePath.leaf().raw() << ": " << dlerror() << std::endl;
			return;
		}

		RegisterPlugins = register_plugins_entry_point(dlsym(module, "register_k3d_plugins"));
		if(!RegisterPlugins)
		{
			log() << error << "Module " << FilePath.leaf().raw() << " does not contain required register_k3d_plugins() entry point" << std::endl;
			return;
		}
	}

#endif // !K3D_API_WIN32

} // namespace k3d

