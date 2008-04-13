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

#include "resource.h"

#include <k3dsdk/result.h>

#include <map>

namespace k3d
{

namespace resource
{

/// Storage for a single resource
class record
{
public:
	record(unsigned char* Data, const uint64_t ByteCount) :
		data(Data),
		byte_count(ByteCount)
	{
	}

	unsigned char* const data;
	const uint64_t byte_count;
};

/// Declares storage for a collection of named resources
typedef std::map<string_t, record> records_t;

/// Stores the global collection of named resources
static records_t records;

const bool_t initialize(const string_t& Path, unsigned char* Data, const uint64_t ByteCount)
{
	if(Path.empty())
	{
		k3d::log() << error << "cannot initialize resource with empty path" << std::endl;
		return false;
	}

	if(records.count(Path))
	{
		k3d::log() << error << "cannot initialize resource with duplicate path [" << Path << "]" << std::endl;
		return false;
	}

	if(!Data)
	{
		k3d::log() << error << "cannot initialize resource with NULL data" << std::endl;
		return false;
	}

	records.insert(std::make_pair(Path, record(Data, ByteCount)));

	return true;
}

const string_t get_string(const string_t& Path)
{
	if(!records.count(Path))
	{
		k3d::log() << error << "unknown resource [" << Path << "]" << std::endl;
		return string_t();
	}
	const record& result = records.find(Path)->second;

	return string_t(reinterpret_cast<char*>(result.data), result.byte_count);
}

} // namespace resource

} // namespace k3d

