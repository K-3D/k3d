#ifndef K3DSDK_ISCRIPTED_ACTION_H
#define K3DSDK_ISCRIPTED_ACTION_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "iscript_engine.h"
#include "types.h"

namespace k3d
{

/// Abstract interface implemented by objects that can execute a script on-demand
class iscripted_action :
	public virtual iunknown
{
public:
	/// When called, the underlying object will execute a script using the given context
	virtual const k3d::bool_t execute(iscript_engine::context_t& Context) = 0;

protected:
	iscripted_action() {}
	iscripted_action(const iscripted_action&) {}
	iscripted_action& operator = (const iscripted_action&) { return *this; }
	virtual ~iscripted_action() {}
};

} // namespace k3d

#endif // K3DSDK_ISCRIPTED_ACTION_H

