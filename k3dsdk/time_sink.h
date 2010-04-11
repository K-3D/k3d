#ifndef K3DSDK_TIME_SINK_H
#define K3DSDK_TIME_SINK_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/data.h>
#include <k3dsdk/itime_sink.h>

namespace k3d
{

class idocument;

/// Adds a boilerplate implementation of k3d::itime_sink to a base class, using the parameterized-inheritance idiom
template<typename base_t>
class time_sink :
	public base_t,
	public itime_sink
{
public:
	time_sink(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_time(init_owner(*this) + init_name("time") + init_label(_("Time")) + init_description(_("Current simulation time.")) + init_value(0.0))
	{
	}

	iproperty& time_sink_input()
	{
		return m_time;
	}

protected:
	k3d_data(double_t, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_time;
};

} // namespace k3d

#endif // !K3DSDK_TIME_SINK_H

