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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/shared_pointer.h>

#include "mesh_topology_data.h"

namespace module
{

namespace development
{

/////////////////////////////////////////////////////////////////////////////
// select_edge_loops

class select_edge_loops :
	public k3d::mesh_selection_modifier
{
	typedef k3d::mesh_selection_modifier base;

public:
	select_edge_loops(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		mesh_sink_input().property_changed_signal().connect(sigc::mem_fun(*this, &select_edge_loops::mesh_changed));
	}
	
	/// Clears cached valencies and companions if the mesh topology is changed
	void mesh_changed(k3d::iunknown* Hint)
	{
		if (!Hint || dynamic_cast<k3d::hint::mesh_topology_changed_t*>(Hint))
		{
			m_companions.reset(0);
			m_valencies.reset(0);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_edge_loops,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xeb328b86, 0x864fa1ec, 0x18415c8c, 0xbd75dcb6),
				"SelectEdgeLoopsNew",
				_("Selects edge loops containing selected egdes"),
				"Development",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	
	void on_select_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if (!m_companions.get() || !m_valencies.get())
		{
			m_companions.reset(new companions(Input));
			m_valencies.reset(new valencies(Input));
		}
		
		// Make sure the Output selection arrays contain the correct selection
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Output);
		
		const companions& comps = *m_companions;
		const valencies& vals = *m_valencies;
		const k3d::mesh::indices_t& edge_points = *Input.polyhedra->edge_points;
		const k3d::mesh::selection_t edge_selection = *Output.polyhedra->edge_selection;
		const k3d::mesh::indices_t& clockwise_edges = *Input.polyhedra->clockwise_edges;
		const k3d::uint_t edge_count = edge_selection.size();
		k3d::mesh::polyhedra_t* target_polyhedra = k3d::make_unique(Output.polyhedra);
		k3d::mesh::selection_t& target_selection = *k3d::make_unique(target_polyhedra->edge_selection);
		for (k3d::uint_t edge = 0; edge != edge_count; ++edge)
		{
			double selection_weight = edge_selection[edge];
			if (selection_weight)
			{
				for (k3d::uint_t loopedge = edge; ; )
				{
					target_selection[loopedge] = selection_weight;
					
					if (vals[edge_points[clockwise_edges[loopedge]]] != 4) // Next edge in loop is ambiguous
						break;
					
					if (comps[clockwise_edges[loopedge]] == clockwise_edges[loopedge]) // No companion
						break;
					
					loopedge = clockwise_edges[comps[clockwise_edges[loopedge]]];
					if (loopedge == edge) // loop complete
						break;
				}
			}
		}
	}
	
	std::auto_ptr<companions> m_companions;
	std::auto_ptr<valencies> m_valencies;
};

/////////////////////////////////////////////////////////////////////////////
// select_edgeloops_factory

k3d::iplugin_factory& select_edgeloops_factory()
{
	return select_edge_loops::get_factory();
}

} // namespace development

} // namespace module
