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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/path.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/imime_type_handler.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>

#include <iostream>

namespace module
{

namespace file_extension
{

/// Uses the Gnome API to open a URI in the user's preferred application
class mime_type_handler :
	public k3d::imime_type_handler,
	public k3d::ideletable
{
public:
	mime_type_handler()
	{
	}

	~mime_type_handler()
	{
	}

	const k3d::uint8_t order()
	{
		return 128;
	}

	const bool test_type(const k3d::string_t& TestExtension, const k3d::string_t& TestType, const k3d::filesystem::path& File, k3d::string_t& FileType)
	{
		if(TestExtension != k3d::filesystem::extension(File).lowercase().raw())
			return false;

		FileType = TestType;

		k3d::log() << info << "Identified " << File.native_console_string() << " as " << FileType << " using " << get_factory().name() << std::endl;
		return true;
	}

	const bool identify_mime_type(const k3d::filesystem::path& File, k3d::string_t& FileType)
	{
		if(test_type(".bmp", "image/bmp", File, FileType)) return true;
		if(test_type(".jpe", "image/jpeg", File, FileType)) return true;
		if(test_type(".jpg", "image/jpeg", File, FileType)) return true;
		if(test_type(".jpeg", "image/jpeg", File, FileType)) return true;
		if(test_type(".k3dscript", "text/x-script.k3dscript", File, FileType)) return true;
		if(test_type(".png", "image/png", File, FileType)) return true;
		if(test_type(".pnm", "image/x-portable-anymap", File, FileType)) return true;
		if(test_type(".py", "text/x-python", File, FileType)) return true;
		if(test_type(".pyc", "application/x-python-bytecode", File, FileType)) return true;
		if(test_type(".sun", "image/x-sun-raster", File, FileType)) return true;
		if(test_type(".tif", "image/tiff", File, FileType)) return true;
		if(test_type(".tiff", "image/tiff", File, FileType)) return true;

		return false;
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<mime_type_handler,
			k3d::interface_list<k3d::imime_type_handler> > factory(
				k3d::uuid(0xc51f66a9, 0x104bd99e, 0x19fbadab, 0x1c84a1b3),
				"FileExtensionMIMETypeHandler",
				_("Identifies a file's MIME Type based on filename extensions"),
				"Desktop",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// mime_type_handler_factory

k3d::iplugin_factory& mime_type_handler_factory()
{
	return mime_type_handler::get_factory();
}

} // namespace file_extension

} // namespace module

