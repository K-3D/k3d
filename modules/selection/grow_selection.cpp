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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>

#include <list>
#include <set>

namespace libk3dselection
{

namespace detail
{

// Selects adjacent faces
typedef std::set<k3d::legacy::split_edge*> edge_set_t;
class select_adjacent_faces
{
public:
	select_adjacent_faces(edge_set_t& SelectedEdges) :
		selected_edges(SelectedEdges)
	{
	}

	void operator()(k3d::legacy::face* Face)
	{
		for(k3d::legacy::split_edge* edge = Face->first_edge; edge; edge = edge->face_clockwise)
		{
			// Save companion
			if(edge->companion)
				selected_edges.insert(edge->companion);

			// Loop end
			if(edge->face_clockwise == Face->first_edge)
				break;
		}
	}

private:
	edge_set_t& selected_edges;
};

typedef std::list<k3d::legacy::split_edge*> edges_t;
typedef std::set<k3d::legacy::point*> points_t;

struct get_selected_edges
{
	get_selected_edges(edges_t& SelectedEdges) :
		selected_edges(SelectedEdges)
	{
	}

	void operator()(k3d::legacy::split_edge& Edge)
	{
		// Save selected edges
		if(Edge.selection_weight)
		{
			selected_edges.push_back(&Edge);

			// Reset selection state
			Edge.selection_weight = 1.0;
		}
	}

	edges_t& selected_edges;
};

// Selects adjacent edges
class grow_edge_selection
{
public:
	grow_edge_selection(const bool VisibleSelection, points_t& Points) :
		border_points(Points)
	{
	}

	void operator()(k3d::legacy::split_edge* Edge)
	{
		if(select_edges(Edge) && select_edges(Edge->face_clockwise))
			return;

		// It's a border edge, save its ends
		border_points.insert(Edge->vertex);
		border_points.insert(Edge->face_clockwise->vertex);
	}

	// Select all edges around Edge's vertex
	bool select_edges(k3d::legacy::split_edge* Edge)
	{
		k3d::legacy::split_edge* current_edge = Edge;
		do
		{
			current_edge->selection_weight = 1.0;

			if(!current_edge->companion)
				return false;

			current_edge = current_edge->companion->face_clockwise;
		}
		while(current_edge != Edge);

		return true;
	}

private:
	points_t& border_points;
};

class select_border_edges
{
public:
	select_border_edges(points_t& BorderPoints, const bool VisibleSelection) :
		border_points(BorderPoints)
	{
	}

	void operator()(k3d::legacy::split_edge& Edge)
	{
		points_t::iterator vertex;

		// Select border edges
		vertex = border_points.find(Edge.vertex);
		if(vertex != border_points.end())
		{
			Edge.selection_weight = 1.0;
		}

		vertex = border_points.find(Edge.face_clockwise->vertex);
		if(vertex != border_points.end())
		{
			Edge.selection_weight = 1.0;
		}
	}

private:
	points_t& border_points;
};

struct get_selected_points
{
	get_selected_points(points_t& SelectedPoints) :
		selected_points(SelectedPoints)
	{
	}

	void operator()(k3d::legacy::point& Point)
	{
		if(Point.selection_weight)
		{
			selected_points.insert(&Point);

			// Reset selection state
			Point.selection_weight = 1.0;
		}
	}

private:
	points_t& selected_points;
};

class select_border_points
{
public:
	select_border_points(points_t& SelectedPoints, const bool VisibleSelection) :
		selected_points(SelectedPoints)
	{
	}

	void operator()(k3d::legacy::split_edge& Edge)
	{
		points_t::iterator vertex;

		// Select border points
		vertex = selected_points.find(Edge.vertex);
		if(vertex != selected_points.end())
		{
			Edge.face_clockwise->vertex->selection_weight = 1.0;
		}

		vertex = selected_points.find(Edge.face_clockwise->vertex);
		if(vertex != selected_points.end())
		{
			Edge.vertex->selection_weight = 1.0;
		}
	}

private:
	points_t& selected_points;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// grow_selection

class grow_selection :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	grow_selection(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		// Grow face selection
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			// Get selected face companions
			typedef std::list<k3d::legacy::face*> faces_t;
			faces_t selected_faces;

			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				if((*face)->selection_weight)
				{
					selected_faces.push_back(*face);

					// Set proper selection for output
					(*face)->selection_weight = 1.0;
				}
			}

			detail::edge_set_t selected_edges;
			std::for_each(selected_faces.begin(), selected_faces.end(),
				detail::select_adjacent_faces(selected_edges));

			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				// Select faces containing an edge from previous edge set
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					// If edge (or its companion) or its vertex is selected, delete face
					detail::edge_set_t::const_iterator selected_edge = selected_edges.find(edge);
					if(selected_edge != selected_edges.end())
					{
						(*face)->selection_weight = 1.0;
						break;
					}

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}
		}

		// Grow edge selections
		detail::edges_t selected_edges;
		k3d::legacy::for_each_edge(Mesh, detail::get_selected_edges(selected_edges));

		detail::points_t border_points;
		std::for_each(selected_edges.begin(), selected_edges.end(),
			detail::grow_edge_selection(true, border_points));

		if(border_points.size())
			k3d::legacy::for_each_edge(Mesh, detail::select_border_edges(border_points, true));

		// Grow point selections
		detail::points_t selected_points;
		k3d::legacy::for_each_point(Mesh, detail::get_selected_points(selected_points));

		if(selected_points.size())
			k3d::legacy::for_each_edge(Mesh, detail::select_border_points(selected_points, true));
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<grow_selection,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xff5b603d, 0x176a4def, 0xa4f195fd, 0x30a7ec15),
				"GrowSelection",
				_("Grows the selection to include adjacent vertices, edges and faces"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// grow_selection_factory

k3d::iplugin_factory& grow_selection_factory()
{
	return grow_selection::get_factory();
}

} // namespace libk3dselection

