#ifndef K3DSDK_ICHANNEL_H
#define K3DSDK_ICHANNEL_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Declares ichannel, an abstract interface for an object encapsulating a data source that changes along an axis (usually time)
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"

namespace k3d
{

/// Abstract interface for an object encapsulating data that can change along an axis (usually time)
template<typename data_t>
class ichannel :
	public virtual iunknown
{
public:
	/// Returns the value of the channel at the current time
	virtual const data_t value() = 0;
	/// Returns the value of the channel at the given axis value
	virtual const data_t value(const double X, const double MaxError) = 0;

	/// Defines a signal that will be emitted whenever the channel is modified
	typedef sigc::signal<void> changed_signal_t;
	virtual changed_signal_t& changed_signal() = 0;

protected:
	ichannel() {}
	ichannel(const ichannel& RHS) {}
	ichannel& operator=(const ichannel& RHS) { return *this; }
	virtual ~ichannel() {}
};

} // namespace k3d

#endif // K3DSDK_ICHANNEL_H


