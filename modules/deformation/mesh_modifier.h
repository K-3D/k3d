#ifndef _K3DSDK_MESH_MODIFIER_H
#define _K3DSDK_MESH_MODIFIER_H

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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/data.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/pointer_demand_storage.h>

namespace _k3d
{

using namespace k3d;

template<typename base_t>
class mesh_modifier :
	public base_t,
	public imesh_sink,
	public imesh_source
{
public:
	mesh_modifier(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<mesh*>(0)),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description(_("Output mesh")))
	{
		k3d::log() << debug << "this is the mesh_modifier sandbox" << std::endl;
		m_input_mesh.changed_signal().connect(hint::converter<
				hint::convert<hint::mesh_geometry_changed, hint::unchanged,
				hint::convert<hint::mesh_topology_changed, hint::unchanged,
				hint::convert<hint::selection_changed, hint::unchanged,
				hint::convert<hint::any, hint::none> > > > >(m_output_mesh.make_slot()));

		m_output_mesh.set_update_slot(sigc::mem_fun(*this, &mesh_modifier<base_t>::execute));
	}

	iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	sigc::slot<void, ihint*> make_update_mesh_slot()
	{
		return m_output_mesh.make_slot();
	}

protected:
	k3d_data(mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh;
	k3d_data(mesh*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output_mesh;

private:
	void execute(const std::vector<ihint*>& Hints, mesh& Output)
	{
		if(const mesh* const input = m_input_mesh.pipeline_value())
		{
			base_t::document().pipeline_profiler().start_execution(*this, "Execute");
			on_execute(Hints, *input, Output);
			base_t::document().pipeline_profiler().finish_execution(*this, "Execute");
		}
	}

	virtual void on_execute(const std::vector<ihint*>& Hints, const mesh& Input, mesh& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_MESH_MODIFIER_H
