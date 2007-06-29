#ifndef K3DSDK_NEW_MESH_SINK_H
#define K3DSDK_NEW_MESH_SINK_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include "imesh_sink.h"
#include "mesh.h"

namespace k3d
{

template<typename base_t>
class mesh_sink :
	public base_t,
	public imesh_sink
{
public:
	mesh_sink(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<mesh*>(0))
	{
	}

	iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

protected:
	k3d_data(mesh*, data::immutable_name, data::change_signal, data::no_undo, data::local_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_input_mesh;
};

} // namespace k3d

#endif // !K3DSDK_NEW_MESH_SINK_H

