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

#include "array.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// array

array::array()
{
}

array::array(const metadata_t& Metadata) :
	metadata(Metadata)
{
}

array::~array()
{
}

void array::set_metadata_value(const string_t& Name, const string_t& Value)
{
	metadata[Name] = Value;
}

void array::set_metadata(const metadata_t& Values)
{
	// Note ... we don't use insert() here because we want to overwrite any existing values
	for(metadata_t::const_iterator pair = Values.begin(); pair != Values.end(); ++pair)
		metadata[pair->first] = pair->second;
}

array::metadata_t array::get_metadata() const
{
	return metadata;
}

const string_t array::get_metadata_value(const string_t& Name) const
{
	metadata_t::const_iterator pair = metadata.find(Name);
	return pair != metadata.end() ? pair->second : string_t();
}

void array::erase_metadata_value(const string_t& Name)
{
	metadata.erase(Name);
}

} // namespace k3d

