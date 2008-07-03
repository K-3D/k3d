#ifndef K3DSDK_DOUBLE_SOURCE_H
#define K3DSDK_DOUBLE_SOURCE_H

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

#include "data.h"
#include "idouble_source.h"

namespace k3d
{

template<typename derived_t>
class double_source :
	public idouble_source
{
public:
	iproperty& double_source_output()
	{
		return m_output_double;
	}

	sigc::slot<void, ihint*> make_reset_double_slot()
	{
		return m_output_double.make_reset_slot();
	}

protected:
	double_source() :
		m_output_double(init_owner(*static_cast<derived_t*>(this)) + init_name("output_double") + init_label(_("Output Double")) + init_description("Output double") + init_slot(sigc::mem_fun(*this, &double_source<derived_t>::create_double)))
	{
	}

private:
	double_t create_double()
	{
		return on_create_double();
	}

	virtual double_t on_create_double() = 0;

	k3d_data(double_t, data::immutable_name, data::change_signal, data::no_undo, data::computed_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_double;
};

} // namespace k3d

#endif // !K3DSDK_DOUBLE_SOURCE_H

