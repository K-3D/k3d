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

#elif defined K3D_API_DARWIN
	#include <mach-o/dyld.h>

	static void ns_error_undefined_symbols(const char* symbol)
	{
		log() << error << "plugin loader : undefined symbol " << symbol << std::endl;
		exit(0);
	}

	static NSModule ns_error_multiply_defined_symbols(NSSymbol s, NSModule oldModule, NSModule newModule)
	{
		log() << warning << "plugin loader : " << NSNameOfSymbol(s) << " redefined in " << NSNameOfModule(oldModule) << std::endl;
		log() << warning << "                previously defined here : " << NSNameOfModule(newModule) << std::endl;

		return newModule;
	}

	static void ns_error_other(NSLinkEditErrors errorClass, int errorNumber, const char* fileName, const char* errorString)
	{
		log() << warning << "plugin loader : " << fileName << " " << errorString << std::endl;
	}

	static NSLinkEditErrorHandlers ns_error_handlers =
	{
		ns_error_undefined_symbols,
		ns_error_multiply_defined_symbols,
		ns_error_other
	};

	static bool ns_initialized = false;

	static void* dlopen(const char* path, int mode)
	{
		return_val_if_fail(path, 0);

		if(!ns_initialized)
		{
			NSInstallLinkEditErrorHandlers(&ns_error_handlers);
			ns_initialized = true;
		}

		NSObjectFileImage file_image = 0;
		NSObjectFileImageReturnCode return_code = NSCreateObjectFileImageFromFile(path, &file_image);

		void* handle = 0;
		switch(return_code)
		{
			case NSObjectFileImageSuccess:
				handle = NSLinkModule(file_image, path, NSLINKMODULE_OPTION_RETURN_ON_ERROR);
				NSDestroyObjectFileImage(file_image);
			break;
			case NSObjectFileImageInappropriateFile:
				handle = (void*)NSAddImage(path, NSADDIMAGE_OPTION_RETURN_ON_ERROR);
			default:
			break;
		}

		return_val_if_fail(handle, 0);

		return handle;
	}

	static void* dlsym(void* handle, const char* symbol)
	{
		return_val_if_fail(handle, 0);

		const std::string _symbol = '_' + std::string(symbol);

		NSSymbol ns_symbol = 0;
		if(MH_MAGIC == static_cast<mach_header*>(handle)->magic)
		{
			return_val_if_fail(NSIsSymbolNameDefinedInImage(static_cast<mach_header*>(handle), _symbol.c_str()), 0);
			ns_symbol = NSLookupSymbolInImage(static_cast<mach_header*>(handle), _symbol.c_str(), NSLOOKUPSYMBOLINIMAGE_OPTION_BIND);
		}
		else
		{
			ns_symbol = NSLookupSymbolInModule(static_cast<NSModule>(handle), _symbol.c_str());
		}

		return_val_if_fail(ns_symbol, 0);

		return NSAddressOfSymbol(ns_symbol);
	}

	static const char* dlerror()
	{
		return 0;
	}

	void os_load_module(const filesystem::path& FilePath, register_plugins_entry_point& RegisterPlugins)
	{
		void* module = dlopen(FilePath.native_filesystem_string().c_str(), 0);
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

#else // POSIX
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

#endif // POSIX

} // namespace k3d

