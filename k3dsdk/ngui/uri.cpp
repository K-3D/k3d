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

#include "messages.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/iuri_handler.h>
#include <k3dsdk/string_cast.h>

#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace ngui
{

namespace uri
{

void open(const std::string& URI)
{
	// Look for any plugins that could be used ...
	const plugin::factory::collection_t factories = plugin::factory::lookup<iuri_handler>();
	if(factories.empty())
	{
		k3d::ngui::error_message(k3d::string_cast(boost::format(_("Couldn't display %1%")) % URI), _("No plugin is available to open URIs."));
		return;
	}

	for(plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		boost::scoped_ptr<iuri_handler> handler(k3d::plugin::create<iuri_handler>(**factory));
		if(handler)
		{
			if(handler->open_uri(URI))
				return;
		}
	}

	k3d::ngui::error_message(k3d::string_cast(boost::format(_("Couldn't display %1%")) % URI), _("No application is configured to handle the given URI."));
}

} // namespace uri

} // namespace ngui

} // namespace k3d

