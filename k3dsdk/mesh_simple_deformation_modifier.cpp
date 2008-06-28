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

#include "mesh_simple_deformation_modifier.h"
#include "mesh_operations.h"
#include "shared_pointer.h"

namespace k3d
{

mesh_simple_deformation_modifier::mesh_simple_deformation_modifier(iplugin_factory& Factory, idocument& Document) :
	base(Factory, Document),
	m_selection_changed(true)
{
	m_mesh_selection.changed_signal().connect(sigc::mem_fun(*this, &mesh_simple_deformation_modifier::on_selection_changed));
}

void mesh_simple_deformation_modifier::on_create_mesh(const mesh& Input, mesh& Output)
{
	Output = Input;
	m_selection_changed = true; // make sure we merge selection after the mesh has been recreated
}

void mesh_simple_deformation_modifier::on_update_mesh(const mesh& Input, mesh& Output)
{
	if(!Input.points)
		return;
	if(!Output.points)
		return;
	return_if_fail(Input.points->size() == Output.points->size());

	if(m_selection_changed)
	{
		merge_selection(m_mesh_selection.pipeline_value(), Output);
		m_selection_changed = false;
	}
	return_if_fail(Output.point_selection);
	return_if_fail(Output.point_selection->size() == Output.points->size());

	const mesh::points_t& input_points = *Input.points;
	const mesh::selection_t& selection = *Output.point_selection;

	document().pipeline_profiler().start_execution(*this, "Copy points");
	mesh::points_t& output_points = *make_unique(Output.points);
	document().pipeline_profiler().finish_execution(*this, "Copy points");

	on_deform_mesh(input_points, selection, output_points);
}

void mesh_simple_deformation_modifier::on_selection_changed(iunknown* Hint)
{
	m_selection_changed = true;
}

} // namespace k3d

