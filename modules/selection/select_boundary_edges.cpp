// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
// select_boundary_edges

class select_boundary_edges :
	public k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_selection_modifier<k3d::node> > base;

public:
	select_boundary_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.property_changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_boundary_edges,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xfdc48d90, 0x2c442a36, 0x9f399c88, 0x88a4e27f),
				"SelectBoundaryEdges",
				_("Selects edges that are on the boundary of the mesh"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
	
private:
	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// Merge with the desired input selection
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(polyhedron)
			{
				k3d::mesh::bools_t boundary_edges;
				k3d::mesh::indices_t adjacent_edges;
				k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);
				const k3d::uint_t edge_count = boundary_edges.size();
				for(k3d::uint_t edge = 0; edge != edge_count; ++edge)
				{
					if(boundary_edges[edge])
						polyhedron->edge_selections[edge] = 1.0;
				}
			}
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// select_boundary_edges_factory

k3d::iplugin_factory& select_boundary_edges_factory()
{
	return select_boundary_edges::get_factory();
}

} // namespace selection

} // namespace module

