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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Evan Lezar (evanlezar@gmail.com)
 */

#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/shared_pointer.h>

#include "cuda_mesh_simple_deformation_modifier.h"

namespace k3d
{

cuda_mesh_simple_deformation_modifier::cuda_mesh_simple_deformation_modifier(iplugin_factory& Factory, idocument& Document) :
	base(Factory, Document),
	m_selection_changed(true),
	m_p_input_device_mesh(init_owner(*this) + init_name("input_device_mesh") + init_label(_("Input Device Mesh")) + init_description(_("Pointer to device mesh")) + init_value<cuda_device_mesh*>(0))
{
	m_mesh_selection.changed_signal().connect(sigc::mem_fun(*this, &cuda_mesh_simple_deformation_modifier::on_selection_changed));
	m_p_input_device_mesh.changed_signal().connect(make_reset_mesh_slot());
}

void cuda_mesh_simple_deformation_modifier::on_create_mesh(const mesh& Input, mesh& Output)
{
	k3d::log() << debug << "cuda_mesh_simple_deformation_modifier::on_create_mesh" << std::endl;

	k3d::log() << debug << "Device Pointer : " << m_p_input_device_mesh.internal_value() << std::endl;
	m_p_input_device_mesh.pipeline_value()->output_debug_info();

	//Output = Input;

	m_selection_changed = true; // make sure we merge selection after the mesh has been recreated
	k3d::log() << debug << "cuda_mesh_simple_deformation_modifier::on_create_mesh::DONE" << std::endl;
}

void cuda_mesh_simple_deformation_modifier::on_update_mesh(const mesh& Input, mesh& Output)
{
	k3d::log() << debug << "cuda_mesh_simple_deformation_modifier::on_update_mesh" << std::endl;

	m_p_input_device_mesh.pipeline_value()->output_debug_info();

	if(m_selection_changed)
	{
		// simply select all the points on the device mesh
		m_p_input_device_mesh.pipeline_value()->select_all_points();
		m_selection_changed = false;
	}

	on_deform_mesh(Output);

	k3d::log() << debug << "cuda_mesh_simple_deformation_modifier::on_update_mesh::DONE" << std::endl;
}

void cuda_mesh_simple_deformation_modifier::on_selection_changed(ihint* Hint)
{
	k3d::log() << debug << "cuda_mesh_simple_deformation_modifier::on_selection_changed" << std::endl;
	m_selection_changed = true;
	k3d::log() << debug << "cuda_mesh_simple_deformation_modifier::on_selection_changed::DONE" << std::endl;
}

} // namespace k3d

