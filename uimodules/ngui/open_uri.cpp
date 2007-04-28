// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include <k3dsdk/auto_ptr.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/iuri.h>
#include <k3dsdk/log.h>
#include <k3dsdk/options.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/system.h>

#include <boost/format.hpp>

#include <iostream>

namespace libk3dngui
{

bool open_uri(const std::string& URI)
{
	// Look for any plugins that could be used ...
	const k3d::factories_t factories = k3d::plugins<k3d::iuri>();
	for(k3d::factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		k3d::auto_ptr<k3d::iunknown> plugin(k3d::create_plugin(**factory));
		if(k3d::iuri* const uri = dynamic_cast<k3d::iuri*>(plugin.get()))
		{
			if(uri->open_uri(URI))
				return true;
		}
	}

	// Fall-back on the configured viewer ...
	try
	{
		boost::format command_line(k3d::options::get_command(k3d::options::command::html_viewer()));
		command_line % URI;

		return_val_if_fail(k3d::system::spawn_async(command_line.str()), false);

		return true;
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "Caught exception: " << e.what() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "Caught unknown exception" << std::endl;
	}

	return false;
}

} // namespace libk3dngui

