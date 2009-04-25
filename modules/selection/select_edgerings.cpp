// K-3D
// Copyright (c) 2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
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
// select_edge_rings

class select_edge_rings :
	public k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> > base;
public:
	select_edge_rings(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.property_changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_edge_rings,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x7e34dbd5, 0xbe9847b2, 0xac9f313e, 0xe1dcbb68),
				"SelectEdgeRings",
				_("Selects edge rings containing selected egdes"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:	
	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);
		
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(*primitive));
			if(!polyhedron)
				continue;
	
			k3d::mesh::indices_t companions;
			k3d::mesh::bools_t boundary_edges;
			k3d::mesh::counts_t valences;

			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->edge_points, polyhedron->clockwise_edges, boundary_edges, companions);
			k3d::polyhedron::create_vertex_valence_lookup(Output.points->size(), polyhedron->edge_points, valences);
			
			const k3d::mesh::selection_t original_edge_selections = polyhedron->edge_selections;

			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + original_edge_selections.size();
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				const k3d::double_t selection_weight = original_edge_selections[edge];
				if(selection_weight)
				{
					for(k3d::uint_t ringedge = edge; ; )
					{
						polyhedron->edge_selections[ringedge] = selection_weight;
						
						if(polyhedron->clockwise_edges[polyhedron->clockwise_edges[polyhedron->clockwise_edges[polyhedron->clockwise_edges[ringedge]]]] != ringedge) // Not a quad
							break;
						
						k3d::uint_t transverse_edge = polyhedron->clockwise_edges[polyhedron->clockwise_edges[ringedge]];
						polyhedron->edge_selections[transverse_edge] = selection_weight;
						
						if(boundary_edges[transverse_edge]) // No companion
							break;
						
						ringedge = companions[transverse_edge];
						
						if(ringedge == edge) // loop complete
							break;
					}
				}
			}
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// select_edgerings_factory

k3d::iplugin_factory& select_edgerings_factory()
{
	return select_edge_rings::get_factory();
}

} // namespace selection

} // namespace module

