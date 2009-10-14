#ifndef K3DSDK_METADATA_H
#define K3DSDK_METADATA_H

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

#include <k3dsdk/imetadata.h>

namespace k3d
{

namespace metadata
{

/// Provides a boilerplate implementation of imetadata
class storage :
	public imetadata 
{
public:
	void set_metadata_value(const string_t& Name, const string_t& Value);
	void set_metadata(const metadata_t& Values);
	metadata_t get_metadata();
	const string_t get_metadata_value(const string_t& Name);
	void erase_metadata_value(const string_t& Name);
	sigc::connection connect_metadata_changed_signal(const sigc::slot<void>& Slot);

private:
	metadata_t m_storage;
	sigc::signal<void> m_changed_signal;
};

//////////////////////////////////////////////////////////////////////////////////////////
// property

/// Wraps a k3d_data object to add metadata
template<typename property_t>
class property :
	public property_t,
	public storage
{
public:
	template<typename init_t>
	property(const init_t& Init) :
		property_t(Init)
	{
	}
};

} // namespace metadata

} // namespace k3d

#endif // !K3DSDK_METADATA_H

