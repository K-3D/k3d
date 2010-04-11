#ifndef MODULES_ODE_ISIMULATION_H
#define MODULES_ODE_ISIMULATION_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/signal_system.h>

#include <ode/ode.h>

namespace module
{

namespace ode
{

/////////////////////////////////////////////////////////////////////////////
// isimulation

class isimulation :
	public k3d::iunknown
{
public:
	virtual const dWorldID world() = 0;
	virtual const dSpaceID space() = 0;

	virtual sigc::connection connect_simulation_changed_signal(const sigc::slot<void>& Slot) = 0;
	virtual sigc::connection connect_initial_state_signal(const sigc::slot<void>& Slot) = 0;
	virtual sigc::connection connect_update_state_signal(const sigc::slot<void>& Slot) = 0;

	virtual void simulation_changed() = 0;
	virtual void update() = 0;
};

} // namespace ode

} // namespace module

#endif // !MODULES_ODE_ISIMULATION_H

