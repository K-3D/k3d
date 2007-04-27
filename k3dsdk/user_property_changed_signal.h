#ifndef K3DSDK_USER_PROPERTY_CHANGED_SIGNAL_H
#define K3DSDK_USER_PROPERTY_CHANGED_SIGNAL_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "signal_system.h"
#include <vector>

namespace k3d
{

class iproperty;
class iproperty_collection;

/// Tracks user properties as they are added/removed from a property collection, and emits a signal whenever any user property value changeds
class user_property_changed_signal
{
public:
	user_property_changed_signal(iproperty_collection& Collection);

	sigc::connection connect(const sigc::slot<void, iunknown*>& Slot);

private:
	void on_collection_changed(iunknown*);

	iproperty_collection& m_collection;
	sigc::signal<void, iunknown*> m_changed_signal;
	typedef std::vector<sigc::connection> connections_t;
	connections_t m_connections;
};

} // namespace k3d

#endif // K3DSDK_USER_PROPERTY_CHANGED_SIGNAL_H

