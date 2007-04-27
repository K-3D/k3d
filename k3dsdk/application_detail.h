#ifndef K3DSDK_APPLICATION_DETAIL_H
#define K3DSDK_APPLICATION_DETAIL_H

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
		\brief Provides an implementation of k3d::iapplication for use by embedders
		\author Tim Shead (tshead@k-3d.com)
*/

#include "signal_accumulators.h"
#include "signal_system.h"

namespace k3d
{

class iapplication;
class iplugin_factory_collection;

/// Provides an implementation of k3d::iapplication for use by embedders
class application_implementation
{
public:
	application_implementation(iplugin_factory_collection& Plugins);
	~application_implementation();

	/// Returns the underlying k3d::iapplication implementation
	iapplication& interface();

	/// Connects a slot that will be called to request application exit - observers may return false to indicate that this isn't possible (e.g. because we're embedded in a scripting engine)
	sigc::connection connect_exit_signal(const sigc::slot<bool>& Slot);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace k3d

#endif // !K3DSDK_APPLICATION_DETAIL_H

