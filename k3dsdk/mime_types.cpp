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

/// Storage for an application plugin that gets created on-demand
template<typename interface_t>
class on_demand_instance
{
public:
	on_demand_instance(iplugin_factory& Factory) :
		m_factory(&Factory),
		m_instance(0)
	{
	}

	interface_t* operator->() const
//	interface_t* instance() const
	{
		if(!m_instance)
			m_instance = plugin::create<interface_t>(*m_factory);

		return m_instance;
	}

private:
	iplugin_factory* m_factory;
	mutable interface_t* m_instance;
};

typedef on_demand_instance<imime_type_handler> handler_t;
/// Storage for an ordered collection of MIME-type handlers
typedef std::multimap<k3d::uint32_t, handler_t> handlers_t;

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
			iplugin_factory::metadata_t metadata = (**factory).metadata();
			
			if(!metadata.count("k3d:load-order"))
			{
				k3d::log() << error << "MIME Type Handler [" << (**factory).name() << "] without k3d:load-order metadata will not be used" << std::endl;
				continue;
			}

			const k3d::uint32_t load_order = k3d::from_string<k3d::uint32_t>(metadata["k3d:load-order"], 255);

			handlers.insert(std::make_pair(load_order, handler_t(**factory)));
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

