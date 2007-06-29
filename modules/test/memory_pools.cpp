// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include <k3dsdk/application_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/icommand_node.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/log.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/result.h>

#include <sstream>

namespace libk3dtest
{

class memory_pools :
	public k3d::icommand_node,
	public k3d::ideletable
{
public:
	memory_pools()
	{
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		if(Command == "shuffle")
		{
			size_t quantity;
			std::string target;
			std::stringstream buffer(Arguments);
			buffer >> quantity >> target;

			return_val_if_fail(quantity, RESULT_ERROR);

			if(target == "points")
			{
				std::vector<k3d::legacy::point*> buffer(quantity);
				for(size_t i = 0; i != quantity; ++i)
					buffer[i] = new k3d::legacy::point(0, 0, 0);
				std::random_shuffle(buffer.begin(), buffer.end());
				std::for_each(buffer.begin(), buffer.end(), k3d::delete_object());

				return RESULT_CONTINUE;
			}
			else if(target == "split_edges")
			{
				std::vector<k3d::legacy::split_edge*> buffer(quantity);
				for(size_t i = 0; i != quantity; ++i)
					buffer[i] = new k3d::legacy::split_edge(0);
				std::random_shuffle(buffer.begin(), buffer.end());
				std::for_each(buffer.begin(), buffer.end(), k3d::delete_object());

				return RESULT_CONTINUE;
			}
			else if(target == "faces")
			{
				std::vector<k3d::legacy::face*> buffer(quantity);
				for(size_t i = 0; i != quantity; ++i)
					buffer[i] = new k3d::legacy::face(0, 0);
				std::random_shuffle(buffer.begin(), buffer.end());
				std::for_each(buffer.begin(), buffer.end(), k3d::delete_object());

				return RESULT_CONTINUE;
			}
			
			return RESULT_ERROR;
		}

		return RESULT_UNKNOWN_COMMAND;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<memory_pools> factory(
			k3d::uuid(0x188634e9, 0x0d4c48d9, 0xb28e4f14, 0x4866fd29),
			"MemoryPools",
			_("Low-level control of memory pools, primarily for testing"),
			"Test");

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// memory_pools_factory

k3d::iplugin_factory& memory_pools_factory()
{
	return memory_pools::get_factory();
}

} // namespace libk3dtest

