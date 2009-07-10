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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// select_edge_loops

class select_edge_loops :
	public k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> > base;

public:
	select_edge_loops(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_ring_side_a(init_owner(*this) + init_name("ring_side_a") + init_label(_("Ring Side A")) + init_description(_("Find loop using the edgering on one side of the selected edge(s)")) + init_value(false)),
		m_ring_side_b(init_owner(*this) + init_name("ring_side_b") + init_label(_("Ring Side B")) + init_description(_("Find loop using the edgering on the other side of the selected edge(s)")) + init_value(false))
	{
		m_mesh_selection.property_changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));

		m_ring_side_a.property_changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));

		m_ring_side_b.property_changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_edge_loops,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x6f42e16a, 0x99804f99, 0xa00528d3, 0x702f015c),
				"SelectEdgeLoops",
				_("Selects edge loops containing selected egdes"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
	
private:
	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		const k3d::bool_t a_side = m_ring_side_a.pipeline_value();
		const k3d::bool_t b_side = m_ring_side_b.pipeline_value();

		k3d::geometry::merge_selection(m_mesh_selection.pipeline_value(), Output);

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(*primitive));
			if(!polyhedron)
				continue;

			m_companions.clear();
			m_valences.clear();
			m_boundary_edges.clear();

			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->edge_points, polyhedron->clockwise_edges, m_boundary_edges, m_companions);
			k3d::polyhedron::create_vertex_valence_lookup(Output.points->size(), polyhedron->edge_points, m_valences);
			
			const k3d::mesh::selection_t original_edge_selections = polyhedron->edge_selections;
		
			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + original_edge_selections.size();
	
			if(!a_side && !b_side) // Use the classical algorithm
			{
				for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
				{
					const k3d::double_t selection_weight = original_edge_selections[edge];
					if(selection_weight)
					{
						for(k3d::uint_t loopedge = edge; ; )
						{
							polyhedron->edge_selections[loopedge] = selection_weight;
							
							if(m_valences[polyhedron->edge_points[polyhedron->clockwise_edges[loopedge]]] != 4) // Next edge in loop is ambiguous
								break;
							
							if(m_boundary_edges[polyhedron->clockwise_edges[loopedge]]) // No companion
								break;
							
							loopedge = polyhedron->clockwise_edges[m_companions[polyhedron->clockwise_edges[loopedge]]];
							if(loopedge == edge) // loop complete
								break;
						}
					}
				}
			}
			else // Use the edgerings on either side of the edge
			{
				for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
				{
					k3d::double_t selection_weight = polyhedron->edge_selections[edge];
					if(selection_weight)
					{
						if(!m_boundary_edges[edge] && polyhedron->edge_selections[m_companions[edge]] && m_companions[edge] > edge)
							continue; // we'll catch this one when we reach its companion
						if(a_side)
							select_with_edgering(selection_weight, polyhedron->clockwise_edges[edge], polyhedron->clockwise_edges, polyhedron->edge_selections);
						if(b_side && !m_boundary_edges[edge])
							select_with_edgering(selection_weight, polyhedron->clockwise_edges[m_companions[edge]], polyhedron->clockwise_edges, polyhedron->edge_selections);
					}
				}
			}
		}
	}
	
	// Select an edge loop at the side of an edgering starting at Edge
	void select_with_edgering(const k3d::double_t SelectionWeight, const k3d::uint_t Edge, const k3d::mesh::indices_t& ClockwiseEdges, k3d::mesh::selection_t& TargetSelection)
	{
		for (k3d::uint_t ringedge = Edge; ; )
		{
			
			if (ClockwiseEdges[ClockwiseEdges[ClockwiseEdges[ClockwiseEdges[ringedge]]]] != ringedge) // Not a quad
				break;
			
			k3d::uint_t transverse_edge = ClockwiseEdges[ClockwiseEdges[ringedge]];
			k3d::uint_t loopedge = ClockwiseEdges[transverse_edge];
			TargetSelection[loopedge] = SelectionWeight;
			if (!m_boundary_edges[loopedge])
				TargetSelection[m_companions[loopedge]] = SelectionWeight;
			
			if (m_boundary_edges[transverse_edge]) // No companion
				break;
			
			ringedge = m_companions[transverse_edge];
			
			if (ringedge == Edge) // loop complete
				break;
		}
	}
	
	k3d::mesh::indices_t m_companions;
	k3d::mesh::bools_t m_boundary_edges;
	k3d::mesh::counts_t m_valences;
	
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_ring_side_a;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_ring_side_b;
};

/////////////////////////////////////////////////////////////////////////////
// select_edgeloops_factory

k3d::iplugin_factory& select_edgeloops_factory()
{
	return select_edge_loops::get_factory();
}

} // namespace selection

} // namespace module

