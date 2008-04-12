#ifndef K3DSDK_INT32_SOURCE_H
#define K3DSDK_INT32_SOURCE_H

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
// License aint32 with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "data.h"
#include "iint32_source.h"

namespace k3d
{

template<typename derived_t>
class int32_source :
	public iint32_source
{
public:
	iproperty& int32_source_output()
	{
		return m_output_int32;
	}

	sigc::slot<void, k3d::iunknown*> make_reset_int32_slot()
	{
		return m_output_int32.make_reset_slot();
	}

protected:
	int32_source() :
		m_output_int32(
			init_owner(*static_cast<derived_t*>(this))
			+ init_name("output_int32")
			+ init_label(_("Output Int32"))
			+ init_description("Output int32")
			+ init_slot(sigc::mem_fun(*this, &int32_source<derived_t>::create_int32)))
	{
	}

private:
	int32_t create_int32()
	{
		return on_create_int32();
	}

	virtual int32_t on_create_int32() = 0;

	k3d_data(k3d::int32_t, data::immutable_name, data::change_signal, data::no_undo, data::computed_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_int32;
};

} // namespace k3d

#endif // !K3DSDK_INT32_SOURCE_H

