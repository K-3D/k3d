#ifndef K3DSDK_CONVERT_H
#define K3DSDK_CONVERT_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <boost/static_assert.hpp>

namespace k3d
{

/// Perform an explicit conversion between two types.  This template is designed to be specialized on an as-needed
/// basis so we have a consistent syntax for performing explicit conversions (for example: from k3d::ustring to QString).
template<typename Target, typename Source>
Target convert(const Source& From)
{
	// This will be triggered if this template is ever instantiated
	BOOST_STATIC_ASSERT(sizeof(Target) == 0);
};

} // namespace k3d

#endif // !K3DSDK_CONVERT_H

