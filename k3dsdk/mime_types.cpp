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

#include <k3dsdk/imime_type_handler.h>
#include <k3dsdk/mime_types.h>
#include <k3dsdk/plugin.h>

#include <boost/optional.hpp>
#include <map>

namespace k3d
{

namespace mime
{

//////////////////////////////////////////////////////////////////////////
// handler_iterator

class handler_iterator
{
public:
	static handler_iterator begin()
	{
		static bool_t initialized = false;
		if(!initialized)
		{
			initialized = true;

			// Get the set of MIME-type handler factories ...
			factories = plugin::factory::lookup<imime_type_handler>();

			// Remove any factories that are missing load-order information ...
			for(plugin::factory::collection_t::iterator factory = factories.begin(); factory != factories.end(); ++factory)
			{
				if(!(**factory).metadata().count("k3d:load-order"))
				{
					k3d::log() << error << "MIME Type Handler [" << (**factory).name() << "] without k3d:load-order metadata will not be used" << std::endl;
					*factory = 0;
					continue;
				}
			}
			factories.erase(std::remove(factories.begin(), factories.end(), static_cast<iplugin_factory*>(0)), factories.end());
			handlers.assign(factories.size(), static_cast<imime_type_handler*>(0));

			// Sort factories by load-order ...
			std::sort(factories.begin(), factories.end(), load_order);
		}

		// Return a new iterator ...
		handler_iterator result;
		result.index = get_index(0);
		return result;
	}

	static handler_iterator end()
	{
		handler_iterator result;
		return result;
	}

	bool_t operator!=(const handler_iterator& other) const
	{
		return index != other.index;
	}

	handler_iterator& operator++()
	{
		if(!index)
			return *this;

		index = get_index(*index + 1);
		return *this;
	}

	imime_type_handler* operator->() const
	{
		return handlers[*index];
	}

private:
	boost::optional<uint_t> index;

	static bool_t load_order(iplugin_factory* a, iplugin_factory* b)
	{
		return from_string<uint32_t>(a->metadata()["k3d:load-order"], 255)
			< from_string<uint32_t>(b->metadata()["k3d:load-order"], 255);
	}

	static boost::optional<uint_t> get_index(const uint_t Index)
	{
		boost::optional<uint_t> result = Index;

		while(factories.size() > Index)
		{
			if(!handlers[Index])
				handlers[Index] = plugin::create<imime_type_handler>(*factories[Index]);

			if(handlers[Index])
				break;

			factories.erase(factories.begin() + Index);
			handlers.erase(handlers.begin() + Index);
		}

		return result;
	}

	static std::vector<iplugin_factory*> factories;
	static std::vector<imime_type_handler*> handlers;
};

std::vector<iplugin_factory*> handler_iterator::factories;
std::vector<imime_type_handler*> handler_iterator::handlers;

////////////////////////////////////////////////////////////////////////////////////////////////////
// type

type::type()
{
}

const type type::lookup(const filesystem::path& File)
{
	type return_type;

	handler_iterator end = handler_iterator::end();
	for(handler_iterator handler = handler_iterator::begin(); handler != end; ++handler)
	{
		if(handler->identify_mime_type(File, return_type.value))
			break;
	}
	return return_type;
}

const type type::lookup(const string_t& Data)
{
	type return_type;

	handler_iterator end = handler_iterator::end();
	for(handler_iterator handler = handler_iterator::begin(); handler != end; ++handler)
	{
		if(handler->identify_mime_type(Data, return_type.value))
			break;
	}
	return return_type;
}

bool_t type::operator==(const string_t& RHS) const
{
	return value == RHS;
}

bool_t type::operator==(const type& RHS) const
{
	return value == RHS.value;
}

bool_t type::operator!=(const type& RHS) const
{
	return value != RHS.value;
}

const string_t type::str() const
{
	return value;
}

bool_t type::empty() const
{
	return value.empty();
}

type::operator bool_t() const
{
	return value.empty();
}

bool_t operator==(const string_t& LHS, const type& RHS)
{
	return RHS == LHS;
}

} // namespace mime

} // namespace k3d

