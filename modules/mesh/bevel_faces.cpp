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
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>

#include "helpers.h"

#include <set>

namespace libk3dmesh
{

namespace detail
{

/// Caches new geometry for better interactive performance
class bevel_edge
{
public:
	bevel_edge(const k3d::point3& StartPosition, const k3d::point3& EndPosition, k3d::legacy::point* Point) :
		start_position(StartPosition),
		end_position(EndPosition),
		point(Point)
	{
	}

	void update(const double Offset, const bool Length)
	{
		double edge_offset = Offset;
		if(!Length)
		{
			// Offset gives position on the edge
			if(Offset > 1)
				edge_offset = 1;
		}
		else
		{
			// Offset is a length from start_position
			const double length = k3d::distance(end_position, start_position);
			if(length)
				edge_offset = Offset / length;
		}

		point->position = start_position + (end_position - start_position) * edge_offset;
	}

private:
	k3d::point3 start_position;
	k3d::point3 end_position;
	k3d::legacy::point* point;
};

typedef std::vector<bevel_edge> bevel_edges_t;

class face_point
{
public:
	face_point(const k3d::point3& OldPosition, const k3d::vector3& InsetDirection, k3d::legacy::point& Point) :
		old_position(OldPosition),
		inset_direction(InsetDirection),
		point(&Point)
	{
	}

	void update(const double Inset)
	{
		point->position = old_position + (Inset * inset_direction);
	}

private:
	k3d::point3 old_position;
	k3d::vector3 inset_direction;
	k3d::legacy::point* point;
};

typedef std::vector<face_point> face_points_t;

/// Bevel point function
typedef std::vector<k3d::legacy::split_edge*> edges_t;
typedef std::map<k3d::legacy::point*, edges_t> points_and_edges_t;
typedef std::map<k3d::legacy::point*, k3d::imaterial*> point_materials_t;
typedef std::vector<k3d::legacy::face*> faces_t;

typedef std::vector<edges_t> edge_lists_t;
edge_lists_t link_edges;

typedef std::map<k3d::legacy::split_edge*, k3d::legacy::face*> edge_to_face_t;

// Subdivide edges
bool subdivide_edges(k3d::legacy::face& Face, k3d::legacy::mesh& Mesh, bevel_edges_t& BevelEdges, face_points_t& FacePoints, edge_to_face_t& EdgeToFace)
{
	// Calculate the face normal
	k3d::normal3 face_normal = k3d::legacy::normal(Face);
	if(0.0 == face_normal.length2())
		return false;

	face_normal = k3d::normalize(face_normal);

	// Store face edges
	edges_t face_edges;
	k3d::legacy::split_edge* edge = Face.first_edge;
	do
	{
		face_edges.push_back(edge);

		edge = edge->face_clockwise;
	}
	while(edge != Face.first_edge);

	// Offset face points
	const unsigned long n = face_edges.size();
	for(unsigned long i = 0; i < n; ++i)
	{
		edge = face_edges[i];
		k3d::legacy::split_edge* previous_edge = face_edges[(i + n-1) % n];

		const k3d::vector3 e1 = k3d::to_vector(edge->vertex->position - previous_edge->vertex->position);
		const k3d::vector3 e2 = k3d::to_vector(edge->face_clockwise->vertex->position - edge->vertex->position);
		const k3d::vector3 e3 = k3d::normalize(e1 ^ face_normal);
		const k3d::vector3 e4 = k3d::normalize(e2 ^ face_normal);

		const k3d::vector3 inset_direction = (-1 / cos(acos(e3 * e4) / 2)) * k3d::normalize(e3 + e4);
		FacePoints.push_back(face_point(edge->vertex->position, inset_direction, *edge->vertex));
	}

	// Make sure it's an interior face
	for(edges_t::reverse_iterator edge_i = face_edges.rbegin(); edge_i != face_edges.rend(); ++edge_i)
	{
		edge = *edge_i;

		if(!edge->companion)
			return false;

		k3d::legacy::split_edge* stop = k3d::legacy::face_anticlockwise(edge);

		// Get subdivided edge
		k3d::legacy::split_edge* subdivided_edge = edge->companion->face_clockwise;

		do
		{
			if(!subdivided_edge->companion)
				return false;

			k3d::legacy::split_edge* next_edge = subdivided_edge->companion->face_clockwise;

			// Next edge
			subdivided_edge = next_edge;
		}
		while(subdivided_edge->companion != stop);
	}

	std::vector<k3d::legacy::point*> new_points;
	edges_t bevel_edges;

	for(edges_t::reverse_iterator edge_i = face_edges.rbegin(); edge_i != face_edges.rend(); ++edge_i)
	{
		edge = *edge_i;

		if(!edge->companion)
			return false;

		k3d::legacy::split_edge* stop = k3d::legacy::face_anticlockwise(edge);

		// Get subdivided edge
		k3d::legacy::split_edge* subdivided_edge = edge->companion->face_clockwise;

		do
		{
			k3d::legacy::split_edge* next_edge = subdivided_edge->companion->face_clockwise;

			// Save original values
			const k3d::point3& start_position = subdivided_edge->vertex->position;
			const k3d::point3& end_position = subdivided_edge->face_clockwise->vertex->position;

			// Create new point
			k3d::legacy::point* new_point = helpers::append_edge(subdivided_edge);
			Mesh.points.push_back(new_point);

			// Cache edge
			BevelEdges.push_back(bevel_edge(start_position, end_position, new_point));

			// Save new edges' faces
			EdgeToFace.insert(std::make_pair(subdivided_edge->face_clockwise, EdgeToFace[subdivided_edge]));
			EdgeToFace.insert(std::make_pair(subdivided_edge->companion, EdgeToFace[subdivided_edge->face_clockwise->companion]));

			// Save vertices to be connected
			bevel_edges.push_back(subdivided_edge->face_clockwise);
			bevel_edges.push_back(subdivided_edge->companion);

			// Next edge
			subdivided_edge = next_edge;
		}
		while(subdivided_edge->companion != stop);
	}

	link_edges.push_back(bevel_edges);

	return true;
}

/// Connects two vertices belonging to the same face
k3d::legacy::face* connect_vertices(k3d::legacy::face* Face, k3d::legacy::split_edge* Edge1, k3d::legacy::split_edge* Edge2, edge_to_face_t& EdgeToFace, faces_t& SideFaces)
{
	// Make new edge loops
	k3d::legacy::split_edge* new_edge1 = new k3d::legacy::split_edge(Edge1->vertex, Edge2);
	k3d::legacy::split_edge* new_edge2 = new k3d::legacy::split_edge(Edge2->vertex, Edge1);

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
		SideFaces.push_back(Face);
	}
	else
	{
		new_face = new k3d::legacy::face(Edge1, Face->material);
		SideFaces.push_back(new_face);
	}

	// Assign new face to edges
	k3d::legacy::split_edge* edge = new_face->first_edge;
	do
	{
		EdgeToFace[edge] = new_face;

		edge = edge->face_clockwise;
	}
	while(edge != new_face->first_edge);

	return new_face;
}

// Connect subdivision vertices
void bevel_faces(edge_to_face_t& EdgeToFace, faces_t& NewFaces, faces_t& SideFaces)
{
	for(edge_lists_t::iterator list = link_edges.begin(); list != link_edges.end(); ++list)
	{
		edges_t& edges = *list;
		const unsigned long n = edges.size();
		for(unsigned long i = 1; i < n; i += 2)
		{
			k3d::legacy::split_edge* edge1 = edges[i];
			k3d::legacy::split_edge* edge2 = edges[(i + 1) % n];
			k3d::legacy::face* face = connect_vertices(EdgeToFace[edge1], edge1, edge2, EdgeToFace, SideFaces);
			if(face)
			{
				NewFaces.push_back(face);
			}
		}
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// bevel_faces_implementation

class bevel_faces_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	bevel_faces_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_offset(init_owner(*this) + init_name("offset") + init_label(_("Offset")) + init_description(_("Offset along external edges")) + init_value(0.3) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_inset(init_owner(*this) + init_name("inset") + init_label(_("Inset")) + init_description(_("Inset of bevelled face")) + init_value(0.3) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("Distance")) + init_description(_("Use distance instead of edge offset")) + init_value(false)),
		m_select_side_faces(init_owner(*this) + init_name("select_side_faces") + init_label(_("Select side faces")) + init_description(_("Select side faces on output, doesn't change selection if off")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());

		m_offset.changed_signal().connect(make_update_mesh_slot());
		m_inset.changed_signal().connect(make_update_mesh_slot());
		m_distance.changed_signal().connect(make_update_mesh_slot());
		m_select_side_faces.changed_signal().connect(make_update_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		// Clear previously cached data
		m_bevel_edges.clear();
		m_face_points.clear();
		m_new_faces.clear();
		m_side_faces.clear();

		detail::link_edges.clear();

		// Create output geometry
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::replace_selection(m_mesh_selection.value(), Mesh);

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron_i = Mesh.polyhedra.begin(); polyhedron_i != Mesh.polyhedra.end(); ++polyhedron_i)
		{
			k3d::legacy::polyhedron& polyhedron = **polyhedron_i;

			// Get the set of all selected faces ...
			k3d::legacy::polyhedron::faces_t selected_faces;
			k3d::copy_if(polyhedron.faces.begin(), polyhedron.faces.end(), std::inserter(selected_faces, selected_faces.end()), k3d::is_selected());

			if(selected_faces.empty())
				selected_faces = polyhedron.faces;

			// Create an edge-to-face map
			detail::edge_to_face_t edge_to_face;
			for(k3d::legacy::polyhedron::faces_t::iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					edge_to_face.insert(std::make_pair(edge, *face));

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}

			// For each face ...
			for(k3d::legacy::polyhedron::faces_t::iterator selected_face = selected_faces.begin(); selected_face != selected_faces.end(); ++selected_face)
			{
				// Save original edges and create new points
				if(detail::subdivide_edges(**selected_face, Mesh, m_bevel_edges, m_face_points, edge_to_face))
				{
					m_new_faces.push_back(*selected_face);
					break;
				}
			}

			// Join new points
			detail::faces_t new_faces;
			detail::bevel_faces(edge_to_face, new_faces, m_side_faces);
			polyhedron.faces.insert(polyhedron.faces.end(), new_faces.begin(), new_faces.end());
			m_new_faces.insert(m_new_faces.end(), new_faces.begin(), new_faces.end());

			assert_warning(k3d::legacy::is_valid(polyhedron));
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const double offset = m_offset.value();
		const bool distance = m_distance.value();

		for(detail::bevel_edges_t::iterator point = m_bevel_edges.begin(); point != m_bevel_edges.end(); ++point)
			point->update(offset, distance);

		const double inset = m_inset.value();
		for(detail::face_points_t::iterator face_point = m_face_points.begin(); face_point != m_face_points.end(); ++face_point)
			face_point->update(inset);

		const double weight = m_select_side_faces.value() ? 1.0 : 0.0;
		for(detail::faces_t::iterator face = m_side_faces.begin(); face != m_side_faces.end(); ++face)
			(*face)->selection_weight = weight;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bevel_faces_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xc3ca122a, 0x9e8b46dc, 0xa6649135, 0xd68ac1a6),
				"BevelFaces",
				"Bevels each selected face",
				"Polygons",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	detail::bevel_edges_t m_bevel_edges;
	detail::face_points_t m_face_points;
	detail::faces_t m_new_faces;
	detail::faces_t m_side_faces;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_offset;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_inset;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_distance;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_select_side_faces;
};

/////////////////////////////////////////////////////////////////////////////
// bevel_faces_factory

k3d::iplugin_factory& bevel_faces_factory()
{
	return bevel_faces_implementation::get_factory();
}

} // namespace libk3dmesh

