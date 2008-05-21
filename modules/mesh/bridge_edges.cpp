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
		\author Romain Behar <romainbehar@yahoo.com>
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>

#include <set>

namespace module
{

namespace mesh
{

namespace detail
{

typedef std::set<k3d::legacy::split_edge*> edges_t;
typedef std::map<k3d::legacy::point*, k3d::legacy::split_edge*> point_edge_t;
typedef std::vector<k3d::legacy::split_edge*> edge_line_t;

void get_connected_edges(edges_t& Edges, point_edge_t& EdgeStarts, point_edge_t& EdgeEnds, edge_line_t& Line)
{
	if(!Edges.size())
		return;

	// Find an edge which vertex isn't at the end of another
	edges_t::iterator first_edge_i = Edges.begin();
	for(; first_edge_i != Edges.end(); ++first_edge_i)
	{
		point_edge_t::iterator edge_i = EdgeEnds.find((*first_edge_i)->vertex);
		if(edge_i == EdgeEnds.end())
		{
			break;
		}
	}

	if(first_edge_i == Edges.end())
		return;

	// Add all connected edges to the set
	k3d::legacy::split_edge* edge = *first_edge_i;
	Edges.erase(first_edge_i);
	Line.push_back(edge);

	point_edge_t::iterator edge_start_i = EdgeStarts.find(edge->face_clockwise->vertex);
	while(edge_start_i != EdgeStarts.end())
	{
		edge = edge_start_i->second;

		// Move it
		edges_t::iterator original_edge = Edges.find(edge);
		return_if_fail(original_edge != Edges.end());

		Edges.erase(original_edge);
		Line.push_back(edge);

		edge_start_i = EdgeStarts.find(edge->face_clockwise->vertex);
	}
}

k3d::legacy::split_edge* bridge(edge_line_t& Line1, edge_line_t& Line2)
{
	if(!Line1.size() || !Line2.size())
		return 0;

	// Make a polygon that links the two lines
	std::vector<k3d::legacy::split_edge*> edges;

	for(edge_line_t::reverse_iterator line1_i = Line1.rbegin(); line1_i != Line1.rend(); ++line1_i)
	{
		k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge((*line1_i)->face_clockwise->vertex);
		edges.push_back(new_edge);

		k3d::legacy::join_edges(*new_edge, **line1_i);
	}

	k3d::legacy::split_edge* end1 = new k3d::legacy::split_edge((*Line1.begin())->vertex);
	edges.push_back(end1);

	for(edge_line_t::reverse_iterator line2_i = Line2.rbegin(); line2_i != Line2.rend(); ++line2_i)
	{
		k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge((*line2_i)->face_clockwise->vertex);
		edges.push_back(new_edge);

		k3d::legacy::join_edges(*new_edge, **line2_i);
	}

	k3d::legacy::split_edge* end2 = new k3d::legacy::split_edge((*Line2.begin())->vertex);
	edges.push_back(end2);

	k3d::legacy::loop_edges(edges.begin(), edges.end());
	return edges.front();
}

k3d::legacy::split_edge* bridge_edge_set(edges_t& Edges)
{
	if(!Edges.size())
		return 0;

	// Make two sets of connected edges (source, destination)
	point_edge_t edge_starts;
	point_edge_t edge_ends;
	for(edges_t::iterator edge = Edges.begin(); edge != Edges.end(); ++edge)
	{
		edge_starts.insert(std::make_pair((*edge)->vertex, *edge));
		edge_ends.insert(std::make_pair((*edge)->face_clockwise->vertex, *edge));
	}

	// Line 1
	edge_line_t line1;
	get_connected_edges(Edges, edge_starts, edge_ends, line1);

	// Line 2
	edge_line_t line2;
	get_connected_edges(Edges, edge_starts, edge_ends, line2);

	// Bridge only if all edges fit either in set1 or in set2
	if(Edges.size())
		return 0;

	return bridge(line1, line2);
}

struct bridge_edges
{
	bridge_edges()
	{
	}

	void operator()(k3d::legacy::polyhedron* Polyhedron)
	{
		edges_t selected_border_edges;
		k3d::imaterial* material;
		for(k3d::legacy::polyhedron::faces_t::iterator face = Polyhedron->faces.begin(); face != Polyhedron->faces.end(); ++face)
		{
			// Get selected border edges
			k3d::legacy::split_edge* edge = (*face)->first_edge;
			do
			{
				if(edge->selection_weight && !edge->companion)
				{
					selected_border_edges.insert(edge);
					material = (*face)->material;
				}

				edge = edge->face_clockwise;
			}
			while(edge != (*face)->first_edge);
		}

		// Bridge edges
		k3d::legacy::split_edge* new_face_start = bridge_edge_set(selected_border_edges);
		if(new_face_start)
		{
			k3d::legacy::face* new_face = new k3d::legacy::face(new_face_start, material);
			Polyhedron->faces.push_back(new_face);

			new_face->selection_weight = 1.0;
		}
	}
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// bridge_edges_implementation

class bridge_edges_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	bridge_edges_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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

		std::for_each(Mesh.polyhedra.begin(), Mesh.polyhedra.end(), detail::bridge_edges());
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bridge_edges_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xa360bb82, 0x223a46df, 0x91e4b594, 0xf9b6b7a3),
				"BridgeEdges",
				"Creates new polygons bridging two sets of connected border edges",
				"Polygons",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bridge_edges_factory

k3d::iplugin_factory& bridge_edges_factory()
{
	return bridge_edges_implementation::get_factory();
}

} // namespace mesh

} // namespace module

