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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <map>
#include <set>

namespace libk3dselection
{

namespace detail
{

const unsigned long vertex_valency(k3d::legacy::split_edge* Edge)
{
	// Cycle through edges around vertex
	unsigned long valency = 0;
	k3d::legacy::split_edge* current_edge = Edge;
	do
	{
		// Return 0 if a surface boundary is found
		if(!current_edge->companion)
			return 0;

		++valency;

		current_edge = current_edge->companion->face_clockwise;
	}
	while(current_edge != Edge);

	return valency;
}

void select_edgeloop(k3d::legacy::split_edge* Edge, const bool VisibleSelection)
{
	// Go forward
	k3d::legacy::split_edge* current_edge = Edge;
	while(vertex_valency(current_edge->face_clockwise) == 4)
	{
		// companion is garanteed non-null thanks to vertex_valency()
		current_edge = current_edge->face_clockwise->companion->face_clockwise;

		current_edge->selection_weight = 1.0;

		if(current_edge == Edge)
			return;
	}

	// Go backward
	current_edge = Edge;
	while(vertex_valency(current_edge) == 4)
	{
		// companion is garanteed non-null thanks to vertex_valency()
		current_edge = k3d::legacy::face_anticlockwise(current_edge);
		current_edge = k3d::legacy::face_anticlockwise(current_edge->companion);

		current_edge->selection_weight = 1.0;

		if(current_edge == Edge)
			return;
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// select_edge_loops

class select_edge_loops :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	select_edge_loops(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::replace_selection(m_mesh_selection.pipeline_value(), Mesh);

		// Make a list of selected edges
		typedef std::list<k3d::legacy::split_edge*> edges_t;
		edges_t edges;
		for(k3d::legacy::mesh::polyhedra_t::iterator p = Mesh.polyhedra.begin(); p != Mesh.polyhedra.end(); ++p)
		{
			k3d::legacy::polyhedron& polyhedron = **p;
			for(k3d::legacy::polyhedron::faces_t::iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					if(edge->selection_weight)
						edges.push_back(edge);

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}
		}

		// For each selected edge, select edgeloop
		for(edges_t::iterator edge = edges.begin(); edge != edges.end(); ++edge)
		{
			detail::select_edgeloop(*edge, true);

			// Reset selection
			(*edge)->selection_weight = 1.0;
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
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
};

/////////////////////////////////////////////////////////////////////////////
// select_edgeloops_factory

k3d::iplugin_factory& select_edgeloops_factory()
{
	return select_edge_loops::get_factory();
}

} // namespace libk3dselection

