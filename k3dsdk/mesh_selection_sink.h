#ifndef K3DSDK_MESH_SELECTION_SINK_H
#define K3DSDK_MESH_SELECTION_SINK_H

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
#include "k3d-i18n-config.h"
#include "imesh_selection_sink.h"
#include "selection.h"

namespace k3d
{

template<typename base_t>
class mesh_selection_sink :
	public base_t,
	public imesh_selection_sink
{
public:
	mesh_selection_sink(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_mesh_selection(init_owner(*this) + init_name("mesh_selection") + init_label(_("Mesh Selection")) + init_description(_("Input Mesh Selection")) + init_value(selection::set()))
	{
	}

	iproperty& mesh_selection_sink_input()
	{
		return m_mesh_selection;
	}

protected:
	k3d_data(selection::set, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, selection_set_serialization) m_mesh_selection;
};

} // namespace k3d

#endif // !K3DSDK_MESH_SELECTION_SINK_H


