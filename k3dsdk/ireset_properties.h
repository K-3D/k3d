#ifndef K3DSDK_IRESET_PROPERTIES_H
#define K3DSDK_IRESET_PROPERTIES_H

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

namespace k3d
{

/// All nodes must provide sensible default values for properties, but some
/// nodes may be able to provide better default values depending on their inputs.
///
/// For example, a node that manipulates NURBS knot vectors may set an initial vector
/// based on the order of an input curve.  A node that modifies mesh texture coordinates
/// would pick a default coordinate array to modify, out of the arrays available in an
/// input mesh, etc.
class ireset_properties :
	public virtual iunknown
{
public:
	/// Implementations should update property values using whatever logic is appropriate.
	/// Note that this is the only time (other than in a constructor) when a node is allowed
	/// to modify one of its own properties!
	virtual void reset_properties() = 0;

protected:
	ireset_properties() {}
	ireset_properties(const ireset_properties&) {}
	ireset_properties& operator=(const ireset_properties&) { return *this; }
	virtual ~ireset_properties() {}
};

} // namespace k3d

#endif // K3DSDK_IRESET_PROPERTIES_H

