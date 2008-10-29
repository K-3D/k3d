#ifndef K3DSDK_ILIST_PROPERTY_H
#define K3DSDK_ILIST_PROPERTY_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares ilist_property, an abstract interface for discovering information about an enumerated property
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include <string>
#include <vector>

namespace k3d
{

/// Abstract interface for discovering information about a list property (a property for which there is a set of predefined values, but other values are allowed)
template<typename data_t>
class ilist_property :
	public virtual iunknown
{
public:
	/// Defines a collection of list values
	typedef std::vector<data_t> values_t;
	/// Returns the (non-exclusive) set of predefined values for this property
	virtual values_t property_values() = 0;

protected:
	ilist_property() {}
	ilist_property(const ilist_property&) {}
	ilist_property& operator = (const ilist_property&) { return *this; }
	virtual ~ilist_property() {}
};

} // namespace k3d

#endif // !K3DSDK_ILIST_PROPERTY_H

