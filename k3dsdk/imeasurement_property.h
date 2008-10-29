#ifndef K3DSDK_IMEASUREMENT_PROPERTY_H
#define K3DSDK_IMEASUREMENT_PROPERTY_H

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
		\brief Declares imeasurement_property, an abstract interface for discovering information about a property that uses real-world units-of-measure
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"

#include <boost/any.hpp>

#include <string>

namespace k3d
{

/// Abstract interface for discovering information about a property that uses real-world units-of-measure
class imeasurement_property :
	public virtual iunknown
{
public:
	/// Returns the suggested step increment to use when modifying the property value
	virtual const double property_step_increment() = 0;
	/// Returns the real-world units-of-measure stored by the property
	virtual const std::type_info& property_units() = 0;

protected:
	imeasurement_property() {}
	imeasurement_property(const imeasurement_property&) {}
	imeasurement_property& operator = (const imeasurement_property&) { return *this; }
	virtual ~imeasurement_property() {}
};

} // namespace k3d

#endif // !K3DSDK_IMEASUREMENT_PROPERTY_H

