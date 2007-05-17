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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "mesh_selection_modifier.h"
#include "shared_pointer.h"

namespace k3d
{

namespace dev
{

mesh_selection_modifier::mesh_selection_modifier(iplugin_factory& Factory, idocument& Document) :
	base(Factory, Document)
{
}

void mesh_selection_modifier::on_create_mesh(const mesh& Input, mesh& Output)
{
	Output = Input;
}

void mesh_selection_modifier::on_update_mesh(const mesh& Input, mesh& Output)
{
	on_select_mesh(Input, Output);
}

} // namespace dev

} // namespace k3d

