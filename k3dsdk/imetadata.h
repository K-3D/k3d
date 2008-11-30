#ifndef K3DSDK_IMETADATA_H
#define K3DSDK_IMETADATA_H

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

#include "iunknown.h"
#include "signal_system.h"
#include "types.h"
#include <map>

namespace k3d
{

/// Abstract interface for objects that can store arbitrary metadata (name-value pairs).
class imetadata :
	public virtual iunknown
{
public:
	/// Defines storage for an arbitrary collection of name-value pairs
	typedef std::map<string_t, string_t> metadata_t;

	/// Sets a new name-value pair, overwriting the value if the name already exists
	virtual void set_metadata_value(const string_t& Name, const string_t& Value) = 0;
	/// Sets a collection of name-value pair, overwriting any existing values
	virtual void set_metadata(const metadata_t& Values) = 0;
	/// Returns the set of existing name-value pairs
	virtual metadata_t get_metadata() = 0;
	/// Returns a value by name, or empty-string if the name doesn't exist
	virtual const string_t get_metadata_value(const string_t& Name) = 0;
	/// Erases an existing name-value pair
	virtual void erase_metadata_value(const string_t& Name) = 0;

	/// Connects a slot to a signal that will be emitted whenever the metadata contents change
	virtual sigc::connection connect_metadata_changed_signal(const sigc::slot<void>& Slot) = 0;

protected:
	imetadata() {}
	imetadata(const imetadata&) {}
	imetadata& operator = (const imetadata&) { return *this; }
	virtual ~imetadata() {}
};

} // namespace k3d

#endif // !K3DSDK_IMETADATA_H

