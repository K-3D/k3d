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
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>

#include "helpers.h"

#include <list>
#include <set>

namespace module
{

namespace mesh
{

namespace detail
{

typedef std::set<k3d::legacy::point*> point_set_t;

/// Removes used k3d::legacy::point* elements from PointSet
class remove_used_points
{
public:
	remove_used_points(point_set_t& PointSet) :
		point_set(PointSet)
	{
	}

	void operator()(k3d::legacy::point&) { }
	void operator()(k3d::legacy::face&) { }

	void operator()(k3d::legacy::split_edge& Edge)
	{
		if(!point_set.size())
			return;

		point_set_t::iterator point = point_set.find(Edge.vertex);
		if(point != point_set.end())
			point_set.erase(point);
	}

	void operator()(k3d::legacy::linear_curve& Curve)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::linear_curve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
		{
			point_set_t::iterator point = point_set.find(*curve_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::linear_curve_group&) { }

	void operator()(k3d::legacy::cubic_curve& Curve)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::cubic_curve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
		{
			point_set_t::iterator point = point_set.find(*curve_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::cubic_curve_group&) { }

	void operator()(k3d::legacy::nucurve& Curve)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::nucurve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
		{
			point_set_t::iterator point = point_set.find(curve_point->position);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::nucurve_group&) { }

	void operator()(k3d::legacy::bilinear_patch& Patch)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::bilinear_patch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
		{
			point_set_t::iterator point = point_set.find(*control_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::bicubic_patch& Patch)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::bicubic_patch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
		{
			point_set_t::iterator point = point_set.find(*control_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::nupatch& Patch)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::nupatch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
		{
			point_set_t::iterator point = point_set.find(control_point->position);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::polyhedron&) { }
	void operator()(k3d::legacy::mesh&) { }

private:
	point_set_t& point_set;
};

/// Returns whether k3d::legacy::point* is in given set
struct is_in_point_set
{
	is_in_point_set(point_set_t& PointSet) :
		point_set(PointSet)
	{
	}

	bool operator()(k3d::legacy::point* Point)
	{
		point_set_t::const_iterator point = point_set.find(Point);
		if(point != point_set.end())
			return true;

		return false;
	}

	point_set_t& point_set;
};

/// Deletes an edge, taking care of face's first_edge
void delete_edge(k3d::legacy::face* Face, k3d::legacy::split_edge* RemovedEdge)
{
	if(Face->first_edge == RemovedEdge)
		Face->first_edge = RemovedEdge->face_clockwise;

	delete RemovedEdge;
}

/// Merges two faces in one at given edge, returns whether Face2 is to be deleted
typedef std::map<k3d::legacy::split_edge*, k3d::legacy::face*> edge_to_face_t;
bool dissolve_edge(k3d::legacy::face* Face1, k3d::legacy::face* Face2, k3d::legacy::split_edge* CommonEdge, edge_to_face_t& EdgeToFace, point_set_t& TouchedPoints)
{
	// Prevent a face from enclosing another one
	if(Face1 == Face2)
	{
		// Deselect edge, it won't be dissolved
		CommonEdge->selection_weight = 0;
		if(CommonEdge->companion)
			CommonEdge->companion->selection_weight = 0;

		return false;
	}

	// Assign Face2 edges to Face1, and void Face2
	k3d::legacy::split_edge* edge = Face2->first_edge;
	do
	{
		EdgeToFace[edge] = Face1;

		edge = edge->face_clockwise;
	}
	while(edge != Face2->first_edge);

	Face2->first_edge = 0;

	// Merge edge loops at CommonEdge
	k3d::legacy::split_edge* Edge1 = k3d::legacy::face_anticlockwise(CommonEdge);
	k3d::legacy::split_edge* Edge2 = k3d::legacy::face_anticlockwise(CommonEdge->companion);
	Edge1->face_clockwise = CommonEdge->companion->face_clockwise;
	Edge2->face_clockwise = CommonEdge->face_clockwise;

	// Change Face1 first_edge if it was CommonEdge
	if(Face1->first_edge == CommonEdge)
		Face1->first_edge = CommonEdge->face_clockwise;

	// Delete CommonEdge and mark its vertices
	TouchedPoints.insert(CommonEdge->companion->vertex);
	delete_edge(Face1, CommonEdge->companion);
	TouchedPoints.insert(CommonEdge->vertex);
	delete_edge(Face1, CommonEdge);

	// Remove created creases
	while(Edge1->companion == Edge1->face_clockwise)
	{
		k3d::legacy::split_edge* previous_edge = k3d::legacy::face_anticlockwise(Edge1);
		k3d::legacy::split_edge* next_edge = Edge1->face_clockwise->face_clockwise;
		if(next_edge == Edge1)
		{
			// Manifold completely dissolved!
			delete Edge1->face_clockwise;
			delete Edge1;
			Face1->first_edge = 0;

			return true;
		}

		previous_edge->face_clockwise = next_edge;

		TouchedPoints.insert(Edge1->face_clockwise->vertex);
		delete_edge(Face1, Edge1->face_clockwise);
		TouchedPoints.insert(Edge1->vertex);
		delete_edge(Face1, Edge1);

		Edge1 = previous_edge;
	}
	while(Edge2->companion == Edge2->face_clockwise)
	{
		k3d::legacy::split_edge* previous_edge = k3d::legacy::face_anticlockwise(Edge2);
		k3d::legacy::split_edge* next_edge = Edge2->face_clockwise->face_clockwise;

		previous_edge->face_clockwise = next_edge;

		TouchedPoints.insert(Edge2->face_clockwise->vertex);
		delete_edge(Face1, Edge2->face_clockwise);
		TouchedPoints.insert(Edge2->vertex);
		delete_edge(Face1, Edge2);

		Edge2 = previous_edge;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Dissolve points

typedef std::vector<k3d::legacy::split_edge*> edge_list_t;
typedef std::map<k3d::legacy::point*, edge_list_t> dissolved_points_t;
typedef std::set<k3d::legacy::face*> faces_t;

/// Dissolve polyhedron : dissolves polyhedron points, edges and faces
struct marked_edge
{
	marked_edge(k3d::legacy::split_edge* Edge) :
		edge(Edge), selection_weight(Edge->selection_weight)
	{
	}

	k3d::legacy::split_edge* edge;
	double selection_weight;
};

void dissolve_polyhedron(k3d::legacy::polyhedron& Polyhedron, k3d::legacy::mesh& Mesh)
{
	point_set_t touched_points;

	// Create an edge-to-face map, and prepare 'DissolvePoints'
	edge_to_face_t edge_to_face;

	// For each selected vertex, make a list of edges ending at them
	dissolved_points_t dissolved_points;

	ResetDissolvePoints:
	for(k3d::legacy::polyhedron::faces_t::iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		// Trivial case: remove edges with a selected vertex and no companion
		k3d::legacy::split_edge* edge = (*face)->first_edge;
		do
		{
			if(!edge->companion && edge->vertex->selection_weight)
			{
				if(edge == edge->face_clockwise || edge == edge->face_clockwise->face_clockwise)
					break;

				// Remove edge from the loop
				k3d::legacy::split_edge* previous_edge = k3d::legacy::face_anticlockwise(edge);
				previous_edge->face_clockwise = edge->face_clockwise;

				if((*face)->first_edge == edge)
					(*face)->first_edge = previous_edge;

				touched_points.insert(edge->vertex);
				delete edge;

				edge = previous_edge->face_clockwise;
			}
			else
			{
				edge = edge->face_clockwise;
			}
		}
		while(edge != (*face)->first_edge);

		if(edge == edge->face_clockwise || edge == edge->face_clockwise->face_clockwise)
		{
			// Face has only one or two edges, delete it
			touched_points.insert(edge->vertex);
			touched_points.insert(edge->face_clockwise->vertex);
			delete *face;
			*face = 0;

			continue;
		}

		// General case
		edge = (*face)->first_edge;
		do
		{
			edge_to_face.insert(std::make_pair(edge, *face));

			// Get every edge ending with a selected vertex
			k3d::legacy::point* const endpoint = edge->face_clockwise->vertex;
			if(endpoint->selection_weight)
			{
				// Add edge to the point's list
				dissolved_points_t::iterator selected_endpoint = dissolved_points.find(endpoint);
				if(selected_endpoint == dissolved_points.end())
				{
					edge_list_t first_edge;
					first_edge.push_back(edge);

					dissolved_points.insert(std::make_pair(endpoint, first_edge));
				}
				else
				{
					selected_endpoint->second.push_back(edge);
				}
			}

			edge = edge->face_clockwise;
		}
		while(edge != (*face)->first_edge);
	}

	// Erase deleted faces
	Polyhedron.faces.erase(std::remove_if(Polyhedron.faces.begin(), Polyhedron.faces.end(), std::bind2nd(std::equal_to<k3d::legacy::face*>(), (k3d::legacy::face*)0)), Polyhedron.faces.end());

	// DissolvePoints : select each edge connected to a selected point
	bool possible_invalidation = false;
	for(dissolved_points_t::iterator point = dissolved_points.begin(); point != dissolved_points.end(); ++point)
	{
		k3d::legacy::point* vertex = point->first;
		edge_list_t& edges = point->second;

		touched_points.insert(vertex);

		if(!edges.size())
			continue;

		k3d::legacy::split_edge* edge = edges.front();

		if(edges.size() == 1)
		{
			// Edges with selected vertex and no companion
			assert_not_reached();
		}
		else if(edges.size() == 2 && edge->companion && edge->face_clockwise->companion && edge->face_clockwise->companion->face_clockwise->companion == edge)
		{
			if(possible_invalidation)
			{
				edge_to_face.clear();
				dissolved_points.clear();
				goto ResetDissolvePoints;
			}

			possible_invalidation = true;

			//  other_edge  ->-> edge->companion
			//   next_edge  <-<- edge
			k3d::legacy::split_edge* next_edge = edge->face_clockwise;

			// Merge other_edge and edge->companion
			k3d::legacy::split_edge* other_edge = next_edge->companion;
			return_if_fail(other_edge);
			return_if_fail(other_edge->face_clockwise == edge->companion);

			other_edge->face_clockwise = edge->companion->face_clockwise;

			edge_to_face_t::iterator face_i = edge_to_face.find(edge->companion);
			return_if_fail(face_i != edge_to_face.end());
			if(face_i->second->first_edge == edge->companion)
				face_i->second->first_edge = other_edge;

			delete edge->companion;

			// Merge edge and next_edge
			edge->face_clockwise = next_edge->face_clockwise;

			edge_to_face_t::iterator face_i2 = edge_to_face.find(next_edge);
			return_if_fail(face_i2 != edge_to_face.end());
			if(face_i2->second->first_edge == next_edge)
				face_i2->second->first_edge = edge;

			delete next_edge;

			// Set new companions
			k3d::legacy::join_edges(*edge, *other_edge);
		}
		else // edges.size() > 1
		{
			// Select edges, they will be dissolved by DissolvePoints
			for(edge_list_t::iterator edge_i = edges.begin(); edge_i != edges.end(); ++edge_i)
			{
				(*edge_i)->selection_weight = 1.0;
			}
		}
	}

	// DissolveFaces : select each edge common to two selected faces
	typedef std::vector<marked_edge> marked_edges_t;
	marked_edges_t marked_edges;
	for(k3d::legacy::polyhedron::faces_t::iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		if(!(*face)->selection_weight)
			continue;

		k3d::legacy::split_edge* edge = (*face)->first_edge;
		do
		{
			if(edge->companion)
			{
				// Mark edge as part of a selected face (select it)
				marked_edges.push_back(marked_edge(edge));
				edge->selection_weight = 1.0;
			}

			edge = edge->face_clockwise;
		}
		while(edge != (*face)->first_edge);
	}

	// DissolveFaces : deselect edges that are not common to selected faces
	unsigned long common_edge_number = 0;
	for(marked_edges_t::iterator common_edge = marked_edges.begin(); common_edge != marked_edges.end(); ++common_edge)
	{
		k3d::legacy::split_edge* edge = common_edge->edge;
		if(!edge->selection_weight || !edge->companion->selection_weight)
		{
			edge->selection_weight = common_edge->selection_weight;
			++common_edge_number;
		}
	}

	// Dissolve edges
	faces_t removed_faces;
	for(k3d::legacy::polyhedron::faces_t::iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		// Make sure face wasn't dissolved
		faces_t::const_iterator dissolved_face = removed_faces.find(*face);
		if(dissolved_face != removed_faces.end())
			continue;

		DissolveEdges:
		k3d::legacy::split_edge* edge = (*face)->first_edge;
		do
		{
			k3d::legacy::split_edge* next_edge = edge->face_clockwise;

			// Dissolve doesn't remove border edges
			if(edge->companion && (edge->selection_weight || edge->companion->selection_weight))
			{
				edge_to_face_t::iterator other_face_i = edge_to_face.find(edge->companion);
				return_if_fail(other_face_i != edge_to_face.end());
				k3d::legacy::face* companion_face = other_face_i->second;

				if(dissolve_edge(*face, companion_face, edge, edge_to_face, touched_points))
				{
					removed_faces.insert(companion_face);
				}

				if(!(*face)->first_edge)
				{
					removed_faces.insert(*face);
					break;
				}

				goto DissolveEdges;
			}

			edge = next_edge;
		}
		while(edge != (*face)->first_edge);
	}

	// Delete removed faces
	for(k3d::legacy::polyhedron::faces_t::iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		if(removed_faces.find(*face) == removed_faces.end())
			continue;

		delete *face;
		*face = 0;
	}

	// Erase deleted faces
	Polyhedron.faces.erase(std::remove_if(Polyhedron.faces.begin(), Polyhedron.faces.end(), std::bind2nd(std::equal_to<k3d::legacy::face*>(), (k3d::legacy::face*)0)), Polyhedron.faces.end());

	// Delete floating points
	k3d::legacy::for_each_component(Mesh, remove_used_points(touched_points));
	if(touched_points.size())
	{
		Mesh.points.erase(std::remove_if(Mesh.points.begin(), Mesh.points.end(), is_in_point_set(touched_points)), Mesh.points.end());
		std::for_each(touched_points.begin(), touched_points.end(), k3d::delete_object());
	}

	// Sanity check
	assert_warning(is_valid(Polyhedron));
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// dissolve_components_implementation

class dissolve_components_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	dissolve_components_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Mesh);

		for(k3d::legacy::mesh::polyhedra_t::iterator p = Mesh.polyhedra.begin(); p != Mesh.polyhedra.end(); ++p)
			detail::dissolve_polyhedron(**p, Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<dissolve_components_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x6c639ea2, 0x6ab542c2, 0xa8f2f2d6, 0xf5265e7c),
				"Dissolve",
				"Dissolves selected faces, edges and vertices",
				"Polygon",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// dissolve_components_factory

k3d::iplugin_factory& dissolve_components_factory()
{
	return dissolve_components_implementation::get_factory();
}

} // namespace mesh

} // namespace module

