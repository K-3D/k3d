#ifndef K3DSDK_FILE_FILTER_H
#define K3DSDK_FILE_FILTER_H

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
		\brief Declares convenience functions for working with file filter objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "auto_ptr.h"
#include "ifile_format.h"
#include "plugin.h"
#include "result.h"

#include <map>

#ifdef	interface
#undef	interface
#endif	// interface

namespace k3d
{

class idocument;
class inode;
class iplugin_factory;

/// Returns a new file format object from a factory (could return NULL)
template<typename interface_t>
interface_t* file_filter(iplugin_factory& Factory)
{
	k3d::auto_ptr<iunknown> filter(plugin::create(Factory));
	return_val_if_fail(filter.get(), 0);

	interface_t* const interface = dynamic_cast<interface_t*>(filter.get());
	return_val_if_fail(interface, 0);

	filter.release();
	return interface;
}

/// Returns a new file format object from a factory (could return NULL)
template<typename interface_t>
interface_t* file_filter(const uuid& ClassID)
{
	k3d::auto_ptr<iunknown> filter(plugin::create(ClassID));
	return_val_if_fail(filter.get(), 0);

	interface_t* const interface = dynamic_cast<interface_t*>(filter.get());
	return_val_if_fail(interface, 0);

	filter.release();
	return interface;
}

/// Attempts to find a new file format object that can read/write the given file (could return NULL)
template<typename interface_t>
interface_t* auto_file_filter(const filesystem::path& File)
{
	// Create a set of filters that can handle the file, sorted by priority ...
	typedef std::map<unsigned long, interface_t*> sorted_filters_t;
	sorted_filters_t sorted_filters;

	// Get the set of objects that support the requested interface, and test them against the actual file ...
	plugin::factory::collection_t factories = plugin::factory::lookup<interface_t>();
	for(plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		// Create a file filter object ...
		iunknown* filter = plugin::create(**factory);
		if(!filter)
		{
			log() << error << (**factory).name() << " couldn't create plugin!" << std::endl;
			continue;
		}

		// If the object doesn't support ifile_format, we're done ...
		ifile_format* const file_format = dynamic_cast<ifile_format*>(filter);
		if(!file_format)
		{
			log() << error << (**factory).name() << " doesn't support k3d::ifile_format!" << std::endl;
			delete dynamic_cast<ideletable*>(filter);
			continue;
		}

		// If the object doesn't support this file, we're done ...
		if(!file_format->query_can_handle(File))
		{
			delete dynamic_cast<ideletable*>(filter);
			continue;
		}

		// If the object doesn't support the requested interface, we're done ...
		interface_t* const interface = dynamic_cast<interface_t*>(filter);
		if(!interface)
		{
			log() << error << (**factory).name() << " doesn't support promised interface!" << std::endl;
			delete dynamic_cast<ideletable*>(filter);
			continue;
		}

		// Insert this format into the sorted collection ...
		sorted_filters.insert(std::make_pair(file_format->priority(), interface));
	}

	// Couldn't find a filter to fit this filetype ...
	if(sorted_filters.empty())
		return 0;

	// And the winner is ...
	interface_t* const result = sorted_filters.rbegin()->second;
	return result;
}

} // namespace k3d

#endif // !K3DSDK_FILE_FILTER_H


