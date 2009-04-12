#ifndef K3DSDK_MESH_SOURCE_H
#define K3DSDK_MESH_SOURCE_H

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
#include "k3d-i18n-config.h"
#include "ipipeline_profiler.h"
#include "imesh_source.h"
#include "mesh.h"
#include "pointer_demand_storage.h"

namespace k3d
{

template<typename base_t>
class mesh_source :
	public base_t,
	public imesh_source
{
public:
	iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	/// Returns a slot that should be connected to input properties to signal that the output has changed
	sigc::slot<void, ihint*> make_update_mesh_slot()
	{
		return m_output_mesh.make_slot();
	}

protected:
	mesh_source(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description("Output mesh"))
	{
		m_output_mesh.set_update_slot(sigc::mem_fun(*this, &mesh_source<base_t>::execute));
	}

	/// Stores the output mesh, which is created / updated on-demand.
	k3d_data(mesh*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output_mesh;

private:
	/// Called whenever the output has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, mesh& Mesh)
	{
		bool_t update_topology = false;
		bool_t update_geometry = false;

		for(uint_t i = 0; i != Hints.size(); ++i)
		{
			// Update our geometry ...
			if(dynamic_cast<hint::mesh_geometry_changed*>(Hints[i]))
			{
				update_geometry = true;
			}
			// In all other cases (mesh_topology_changed, unknown hint, or no hint),
			// we must assume the worst and recreate everything from scratch ...
			else
			{
				update_topology = true;
				update_geometry = true;
				break;
			}
		}

		if(update_topology)
		{
			base_t::document().pipeline_profiler().start_execution(*this, "Update Topology");
			on_update_mesh_topology(Mesh);
			base_t::document().pipeline_profiler().finish_execution(*this, "Update Topology");
		}

		if(update_geometry)
		{
			base_t::document().pipeline_profiler().start_execution(*this, "Update Geometry");
			on_update_mesh_geometry(Mesh);
			base_t::document().pipeline_profiler().finish_execution(*this, "Update Geometry");
		}
	}

	/// Implement this in derived classes to setup the topology of the output mesh.  Note that the 
	/// output mesh is empty the first time this method is called, but will retain its state thereafter,
	/// so implementations must be written to ensure that they don't simply append to the existing state.
	virtual void on_update_mesh_topology(mesh& Output) = 0;

	/// Implement this in derived classes to setup the geometry of the output mesh.
	virtual void on_update_mesh_geometry(mesh& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_MESH_SOURCE_H

