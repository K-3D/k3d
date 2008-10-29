#ifndef K3DSDK_IWRITABLE_PROPERTY_H
#define K3DSDK_IWRITABLE_PROPERTY_H

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
		\brief Declares iwritable_property, an abstract interface for a name-value pair with fixed type that can be written
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ihint.h"
#include <boost/any.hpp>

namespace k3d
{

/// Abstract interface for a property that can be written (see iproperty if you need to read the value of a property)
class iwritable_property :
	public virtual iunknown
{
public:
	/// Sets the property value (note - could fail and return false if there's a type-mismatch or other problem with the input data).  The optional Hint will be passed to observers and can provide additional information about what changed.
	virtual bool property_set_value(const boost::any Value, ihint* const Hint = 0) = 0;

protected:
	iwritable_property() {}
	iwritable_property(const iwritable_property&) {}
	iwritable_property& operator = (const iwritable_property&) { return *this; }
	virtual ~iwritable_property() {}
};

} // namespace k3d

#endif // !K3DSDK_IWRITABLE_PROPERTY_H

