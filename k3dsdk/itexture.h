#ifndef K3DSDK_ITEXTURE_H
#define K3DSDK_ITEXTURE_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "almost_equal.h"
#include "iunknown.h"

namespace k3d
{

/// Abstract interface implemented by objects that can act as geometric surface materials
class itexture :
	public virtual iunknown
{
protected:
	itexture() {}
	itexture(const itexture& Other) : iunknown(Other) {}
	itexture& operator=(const itexture&) { return *this; }
	virtual ~itexture() {}
};

/// Specialization of almost_equal that tests itexture pointers for equality
template<>
class almost_equal<itexture*>
{
public:
	almost_equal(const boost::uint64_t) { } 
	inline bool_t operator()(itexture* const A, itexture* const B) const { return A == B; }
};

} // namespace k3d

#endif // !K3DSDK_ITEXTURE_H

