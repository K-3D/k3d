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
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// connect_vertices_implementation

class connect_vertices_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	connect_vertices_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		// Create output geometry
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		// Keep track of newly-created edges ...
		new_edges_t new_edges;
		
		// Connect face vertices
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			new_faces_t new_faces;

			// For each face
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				const edge_groups_t edge_groups = get_edge_groups(**face);
				
				if(4 == edge_groups.size() && edge_groups[0].size() == edge_groups[2].size())
				{
					const size_t count = edge_groups[0].size();
					for(size_t i = 0; i != count; ++i)
					{
						split_face(**face, *edge_groups[0][count - i - 1], *edge_groups[2][i], new_faces, new_edges);
					}
				}
			}

			(*polyhedron)->faces.insert((*polyhedron)->faces.end(), new_faces.begin(), new_faces.end());
		}

		// Update output selection ...
		std::for_each(Mesh.points.begin(), Mesh.points.end(), k3d::selection::set_weight(0));
		std::for_each(new_edges.begin(), new_edges.end(), k3d::selection::set_weight(1));
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<connect_vertices_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xbbe1c426, 0x07a54105, 0xbbdb5e49, 0xcaf1825e),
				"ConnectVertices",
				"Creates edges between selected vertices",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef std::vector<k3d::legacy::face*> new_faces_t;
	typedef std::vector<k3d::legacy::split_edge*> new_edges_t;

	typedef std::vector<k3d::legacy::split_edge*> edge_group_t;
	typedef std::vector<edge_group_t> edge_groups_t;

	const edge_groups_t get_edge_groups(const k3d::legacy::face& Face)
	{
		edge_groups_t edge_groups;

		for(k3d::legacy::split_edge* edge = Face.first_edge; edge; edge = edge->face_clockwise)
		{
			if(edge_groups.empty())
			{
				edge_groups.push_back(edge_group_t(1, edge));
			}
			else
			{
				const bool group_selected = edge_groups.back().back()->vertex->selection_weight ? true : false;
				const bool edge_selected = edge->vertex->selection_weight ? true : false;

				if(group_selected != edge_selected)
				{
					edge_groups.push_back(edge_group_t(1, edge));
				}
				else
				{
					edge_groups.back().push_back(edge);
				}
			}

			if(edge->face_clockwise == Face.first_edge)
				break;
		}

		// If the first and last groups have the same selection-state, they should be one big group ...
		if(edge_groups.size() > 1)
		{
			const bool first_selected = edge_groups.front().front()->vertex->selection_weight ? true : false;
			const bool last_selected = edge_groups.back().back()->vertex->selection_weight ? true : false;
			if(first_selected == last_selected)
			{
				edge_groups.back().insert(edge_groups.back().end(), edge_groups.front().begin(), edge_groups.front().end());
				edge_groups.erase(edge_groups.begin());
			}
		}

		// If there is more tnan one group, see to it that the first group is selected ...
		if(edge_groups.size() > 1)
		{
			const bool first_selected = edge_groups.front().front()->vertex->selection_weight ? true : false;
			if(!first_selected)
			{
				edge_groups.insert(edge_groups.end(), edge_groups.front());
				edge_groups.erase(edge_groups.begin());
			}
		}

		return edge_groups;
	}

	/// Connects two vertices belonging to the same face
	void split_face(k3d::legacy::face& Face, k3d::legacy::split_edge& Edge1, k3d::legacy::split_edge& Edge2, new_faces_t& NewFaces, new_edges_t& NewEdges)
	{
		// Make new edge loops
		k3d::legacy::split_edge* const new_edge1 = new k3d::legacy::split_edge(Edge1.vertex, &Edge2);
		k3d::legacy::split_edge* const new_edge2 = new k3d::legacy::split_edge(Edge2.vertex, &Edge1);

		k3d::legacy::split_edge* const anti_edge1 = k3d::legacy::face_anticlockwise(&Edge1);
		k3d::legacy::split_edge* const anti_edge2 = k3d::legacy::face_anticlockwise(&Edge2);

		anti_edge1->face_clockwise = new_edge1;
		anti_edge2->face_clockwise = new_edge2;

		k3d::legacy::join_edges(*new_edge1, *new_edge2);

		k3d::legacy::face* const new_face = new k3d::legacy::face(&Edge1, Face.material);
		if(Face.first_edge == &Edge1)
			Face.first_edge = new_edge1;

		NewFaces.push_back(new_face);
		NewEdges.push_back(new_edge1);
		NewEdges.push_back(new_edge2);
	}
};

/////////////////////////////////////////////////////////////////////////////
// connect_vertices_factory

k3d::iplugin_factory& connect_vertices_factory()
{
	return connect_vertices_implementation::get_factory();
}

} // namespace mesh

} // namespace module

