#ifndef K3DSDK_LONG_SOURCE_H
#define K3DSDK_LONG_SOURCE_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
#include "ilong_source.h"

namespace k3d
{

template<typename base_t>
class long_source :
	public base_t,
	public ilong_source
{
public:
	long_source(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_output_long(init_owner(*this) + init_name("output_long") + init_label(_("Output Long")) + init_description("Output long") + init_slot(sigc::mem_fun(*this, &long_source<base_t>::create_long)))
	{
	}

	iproperty& long_source_output()
	{
		return m_output_long;
	}

	sigc::slot<void, k3d::iunknown*> make_reset_long_slot()
	{
		return m_output_long.make_reset_slot();
	}

private:
	long create_long()
	{
		return on_create_long();
	}

	virtual long on_create_long() = 0;

	k3d_data(k3d::int32_t, data::immutable_name, data::change_signal, data::no_undo, data::computed_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_long;
};

} // namespace k3d

#endif // !K3DSDK_LONG_SOURCE_H

