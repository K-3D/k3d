// K-3D
// Copyright (c) 2005-2006, Romain Behar
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
		\author Romain Behar <romainbehar@yahoo.com>
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>

#include "helpers.h"

namespace libk3dmesh
{

namespace detail
{

typedef std::vector<k3d::legacy::split_edge*> edges_t;

/// Connects two vertices belonging to the same face
k3d::legacy::face* connect_vertices2(k3d::legacy::face* Face, k3d::legacy::split_edge* Edge1, k3d::legacy::split_edge* Edge2, edges_t& NewEdges)
{
	// Make new edge loops
	k3d::legacy::split_edge* new_edge1 = new k3d::legacy::split_edge(Edge1->vertex, Edge2);
	k3d::legacy::split_edge* new_edge2 = new k3d::legacy::split_edge(Edge2->vertex, Edge1);
	NewEdges.push_back(new_edge1);
	NewEdges.push_back(new_edge2);

	k3d::legacy::split_edge* anti_edge1 = k3d::legacy::face_anticlockwise(Edge1);
	k3d::legacy::split_edge* anti_edge2 = k3d::legacy::face_anticlockwise(Edge2);

	anti_edge1->face_clockwise = new_edge1;
	anti_edge2->face_clockwise = new_edge2;

	k3d::legacy::join_edges(*new_edge1, *new_edge2);

	// Create a second face for ear cut
	k3d::legacy::face* new_face = 0;
	if(helpers::edge_in_face(Face->first_edge, Edge1))
	{
		new_face = new k3d::legacy::face(Edge2, Face->material);
	}
	else
	{
		new_face = new k3d::legacy::face(Edge1, Face->material);
	}

	return new_face;
}

/// Checks whether one or several colinear edges already link given points
// be careful with parameters order; return true if a line links the two vertices, false otherwise
bool colinear_linked2(k3d::legacy::split_edge* Edge1, k3d::legacy::split_edge* Edge2)
{
	if(Edge1->face_clockwise == Edge2)
		return true;

	k3d::legacy::split_edge* edge = Edge1;
	do
	{
		k3d::vector3 vector1 = k3d::to_vector(edge->face_clockwise->vertex->position - edge->vertex->position);
		k3d::vector3 vector2 = k3d::to_vector(Edge2->vertex->position - edge->vertex->position);
		k3d::vector3 normal = vector1 ^ vector2;

		if(normal.length2() > 1e-6)
		{
			return false;
		}

		edge = edge->face_clockwise;
	}
	while(edge != Edge2);

	return true;
}

/// Connects face's selected vertices
typedef std::vector<k3d::legacy::face*> faces_t;
void connect_face_vertices2(k3d::legacy::face* Face, edges_t& NewEdges, faces_t& NewFaces)
{
	// Get edges with a selected vertex
	edges_t selected_edges;

	k3d::legacy::split_edge* edge = Face->first_edge;
	do
	{
		if(edge->vertex->selection_weight)
			selected_edges.push_back(edge);

		edge = edge->face_clockwise;
	}
	while(edge != Face->first_edge);

	// Link non-consecutive points
	while(selected_edges.size() >= 2)
	{
		// Create a triangle linking 3 remaining vertices
		if(selected_edges.size() == 3)
		{
			if(!colinear_linked2(selected_edges[0], selected_edges[1]))
				NewFaces.push_back(connect_vertices2(Face, selected_edges[0], selected_edges[1], NewEdges));
			if(!colinear_linked2(selected_edges[1], selected_edges[2]))
				NewFaces.push_back(connect_vertices2(Face, selected_edges[1], selected_edges[2], NewEdges));
			if(!colinear_linked2(selected_edges[2], selected_edges[0]))
				NewFaces.push_back(connect_vertices2(Face, selected_edges[2], selected_edges[0], NewEdges));

			return;
		}

		// Do an ear-cut on consecutive selected vertex pairs
		unsigned long first_point = 0;
		unsigned long second_point = 1;
		bool found = false;
		do
		{
			k3d::legacy::split_edge* edge1 = selected_edges[first_point];
			k3d::legacy::split_edge* edge2 = selected_edges[second_point % selected_edges.size()];

			if(edge1->face_clockwise != edge2 && edge2->face_clockwise != edge1)
					// When there are 2 selected edges, edge2 might come before edge1
			{
				// Make sure vectors are not colinear
				k3d::vector3 vector1 = k3d::to_vector(edge1->face_clockwise->vertex->position - edge1->vertex->position);
				k3d::vector3 vector2 = k3d::to_vector(edge2->vertex->position - edge1->vertex->position);
				k3d::vector3 normal = vector1 ^ vector2;

				if(normal.length2() > 1e-6)
				{
					found = true;
					break;
				}
			}

			++first_point;
			++second_point;
		}
		while(second_point <= selected_edges.size());

		if(!found)
			break;

		// Link points and remove them from the list
		NewFaces.push_back(connect_vertices2(Face, selected_edges[first_point], selected_edges[second_point], NewEdges));

		selected_edges.erase(selected_edges.begin() + second_point);
		selected_edges.erase(selected_edges.begin() + first_point);
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// connect_vertices2_implementation

class connect_vertices2_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	connect_vertices2_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		// Clear cache
		m_new_edges.clear();

		// Create output geometry
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::replace_selection(m_mesh_selection.pipeline_value(), Mesh);

		// Connect face vertices
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			detail::faces_t new_faces;

			// For each face
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				detail::connect_face_vertices2(*face, m_new_edges, new_faces);
			}

			if(new_faces.size())
				(*polyhedron)->faces.insert((*polyhedron)->faces.end(), new_faces.begin(), new_faces.end());
		}

		// Update output selection
		for(detail::edges_t::iterator edge = m_new_edges.begin(); edge != m_new_edges.end(); ++edge)
			(*edge)->selection_weight = 1.0;
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<connect_vertices2_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xee6b7115, 0xedba43a2, 0x92750f10, 0x113355a5),
				"ConnectVertices2",
				"Creates edges between selected vertices",
				"Mesh",
				k3d::iplugin_factory::DEPRECATED);

		return factory;
	}

private:
	detail::edges_t m_new_edges;
};

/////////////////////////////////////////////////////////////////////////////
// connect_vertices2_factory

k3d::iplugin_factory& connect_vertices2_factory()
{
	return connect_vertices2_implementation::get_factory();
}

} // namespace libk3dmesh

