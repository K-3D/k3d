#ifndef MODULES_LUXRENDER_TEXTURE2_H
#define MODULES_LUXRENDER_TEXTURE2_H

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
	\author Tim Shead <tshead@k-3d.com>
*/

#include <k3dsdk/data.h>
#include <k3dsdk/node.h>
#include <k3dsdk/types.h>
#include <k3dsdk/vector3.h>

#include <iosfwd>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// texture2

/// Abstract base class for LuxRender 2D texture objects
class texture2 :
	public k3d::node
{
public:
	texture2(k3d::iplugin_factory& Factory, k3d::idocument& Document);

protected:
	void setup_texture2(std::ostream& Stream);

private:
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_mapping;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_u_scale;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v_scale;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_u_delta;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v_delta;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_u_axis;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v_axis;
};

} // namespace luxrender

} // namespace module

#endif // !MODULES_LUXRENDER_TEXTURE2_H

