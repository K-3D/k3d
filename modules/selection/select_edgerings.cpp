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
#include <k3dsdk/i18n.h>
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

const unsigned long vertex_valency1(k3d::legacy::split_edge* Edge)
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

void select_edgering(k3d::legacy::split_edge* Edge, const bool Extended, const bool VisibleSelection)
{
	const unsigned long valency1 = vertex_valency1(Edge);
	const unsigned long valency2 = vertex_valency1(Edge->face_clockwise);
	return_if_fail(valency1 == valency2);

	k3d::legacy::split_edge* current_edge = Edge;
	do
	{
		k3d::legacy::split_edge* next_edge = current_edge->face_clockwise->face_clockwise;

		if(!Extended)
		{
			// Make sure next edge endpoints have a valency of 4
			if(vertex_valency1(next_edge) != 4)
				return;

			if(vertex_valency1(next_edge->face_clockwise) != 4)
				return;
		}

		// Make sure current face forms a quad
		if(next_edge->face_clockwise->face_clockwise != current_edge)
			return;

		next_edge->selection_weight = 1.0;

		current_edge = next_edge->companion;
	}
	while(current_edge && current_edge != Edge);
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// select_edge_rings

class select_edge_rings :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	select_edge_rings(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_extended(init_owner(*this) + init_name("extended") + init_label(_("Extended mode")) + init_description(_("Extended mode follows quads instead of edgeloops")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_extended.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::replace_selection(m_mesh_selection.value(), Mesh);

		const bool extended = m_extended.value();

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

		// For each selected edge, select edgering
		for(edges_t::iterator edge = edges.begin(); edge != edges.end(); ++edge)
		{
			detail::select_edgering(*edge, extended, true);

			if((*edge)->companion)
				detail::select_edgering((*edge)->companion, extended, true);

			// Reset selection
			(*edge)->selection_weight = 1.0;
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
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
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_extended;
};

/////////////////////////////////////////////////////////////////////////////
// select_edgerings_factory

k3d::iplugin_factory& select_edgerings_factory()
{
	return select_edge_rings::get_factory();
}

} // namespace libk3dselection

