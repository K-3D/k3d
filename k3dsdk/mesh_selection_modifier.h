#ifndef K3DSDK_MESH_SELECTION_MODIFIER_H
#define K3DSDK_MESH_SELECTION_MODIFIER_H

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
*/

#include "data.h"
#include "hints.h"
#include "imesh_sink.h"
#include "imesh_source.h"
#include "ipipeline_profiler.h"
#include "mesh.h"
#include "pointer_demand_storage.h"

namespace k3d
{

/// Mesh modifier implementation for use in plugins that alter the selection state of a mesh without altering its topology or geometry.  To create a plugin, derive from mesh_selection_modifier and implement the on_select_mesh() method.
template<typename base_t>
class mesh_selection_modifier :
	public base_t,
	public imesh_sink,
	public imesh_source
{
public:
	iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	/// Returns a slot that should be connected to input properties to signal that the output mesh has changed
	sigc::slot<void, ihint*> make_update_mesh_slot()
	{
		return m_output_mesh.make_slot();
	}

protected:
	mesh_selection_modifier(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<mesh*>(0)),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description(_("Output mesh")))
	{
		m_input_mesh.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_update_mesh_slot()));

		m_output_mesh.set_update_slot(sigc::mem_fun(*this, &mesh_selection_modifier<base_t>::execute));
	}

	/// Called whenever the output mesh has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, mesh& Mesh)
	{
		// In our case, we don't have to worry about any hints, we just execute.
		if(const mesh* const input = m_input_mesh.pipeline_value())
		{
			Mesh = *input;
			
			base_t::document().pipeline_profiler().start_execution(*this, "Update Selection");
			on_update_selection(*input, Mesh);
			base_t::document().pipeline_profiler().finish_execution(*this, "Update Selection");
		}
	}

	k3d_data(mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh;
	k3d_data(mesh*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output_mesh;

private:
	/// Implement this method in derived classes and alter the output selection(s) as-desired.
	virtual void on_update_selection(const mesh& Input, mesh& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_MESH_SELECTION_MODIFIER_H

