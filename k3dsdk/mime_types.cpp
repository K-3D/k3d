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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "imime_type_handler.h"
#include "mime_types.h"
#include "plugin.h"

#include <map>

namespace k3d
{

namespace mime
{

namespace detail
{

/// Storage for an ordered collection of MIME-type handlers
typedef std::multimap<k3d::uint8_t, imime_type_handler*> handlers_t;

const handlers_t& get_handlers()
{
	// Cache a collection of MIME-type handlers ...
	static handlers_t handlers;
	static bool initialized = false;

	if(!initialized)
	{
		initialized = true;

		const plugin::factory::collection_t factories = plugin::factory::lookup<imime_type_handler>();
		for(plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
		{
			if(imime_type_handler* const handler = plugin::create<imime_type_handler>(**factory))
				handlers.insert(std::make_pair(handler->order(), handler));
		}
	}

	return handlers;
}

} // namespace detail

const string_t type(const filesystem::path& File)
{
	static const detail::handlers_t& handlers = detail::get_handlers();

	string_t file_type;
	for(detail::handlers_t::const_iterator handler = handlers.begin(); handler != handlers.end(); ++handler)
	{
		if(handler->second->identify_mime_type(File, file_type))
			break;
	}
	return file_type;
}

const string_t type(const string_t& Data)
{
	static const detail::handlers_t& handlers = detail::get_handlers();

	string_t data_type;
	for(detail::handlers_t::const_iterator handler = handlers.begin(); handler != handlers.end(); ++handler)
	{
		if(handler->second->identify_mime_type(Data, data_type))
			break;
	}
	return data_type;
}

} // namespace mime

} // namespace k3d

