#ifndef MODULES_SCALAR_SCALAR_SOURCE_H
#define MODULES_SCALAR_SCALAR_SOURCE_H

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
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/hints.h>
#include <k3dsdk/node.h>
#include <k3dsdk/value_demand_storage.h>

namespace module
{

namespace scalar
{

/// Convenience base-class for sources / filters that produce a scalar output value
class scalar_source :
	public k3d::node
{
	typedef k3d::node base;
public:
	scalar_source(k3d::iplugin_factory& Factory, k3d::idocument& Document, const char* const OutputDescription);

	/// Returns a slot that should be connected to input properties to signal that the output value has changed.
	sigc::slot<void, k3d::ihint*> make_update_value_slot();

private:
	/// Stores the output value
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_output;

	/// Called whenever the output value has been modified and needs to be updated.
	void execute(const std::vector<k3d::ihint*>& Hints, k3d::double_t& Output);
	/// Override this in derived classes, to return a new / modified value
	virtual void on_update_value(k3d::double_t& Output);
};

} // namespace scalar

} // namespace module

#endif // !MODULES_SCALAR_SCALAR_SOURCE_H

