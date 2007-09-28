// K-3D
// Copyright (c) 2004-2006, Romain Behar
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
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include "helpers.h"

#include <set>

namespace libk3dmesh
{

namespace detail
{

/// Caches new geometry for better interactive performance
class middle_point
{
public:
	middle_point(const k3d::point3& StartPosition, const k3d::point3& EndPosition, k3d::legacy::point& Point) :
		start_position(StartPosition),
		end_position(EndPosition),
		point(&Point)
	{
	}

	void update(const double Offset)
	{
		point->position = start_position + (end_position - start_position) * (0.5 + Offset);
	}

private:
	k3d::point3 start_position;
	k3d::point3 end_position;
	k3d::legacy::point* point;
};

typedef std::vector<middle_point> middle_points_t;

struct delete_bogus_edge
{
	void operator()(k3d::legacy::split_edge* Edge)
	{
		Edge->companion = 0;
		delete Edge;
	}
};

/// Joins face vertices to barycenter
struct center_to_points
{
	center_to_points(k3d::legacy::mesh* Mesh, std::vector<k3d::legacy::face*>& NewFaces, k3d::legacy::polyhedron& Polyhedron) :
		mesh(Mesh), new_faces(NewFaces), polyhedron(Polyhedron)
	{
	}

	void operator()(k3d::legacy::face* Face)
	{
		if(!Face->selection_weight)
			return;

		// Create center point
		k3d::point3 face_barycenter = k3d::point3(0, 0, 0);
		k3d::legacy::point* center_point = new k3d::legacy::point(face_barycenter);
		mesh->points.push_back(center_point);

		// Create subfaces
		k3d::legacy::split_edge* previous_interior_edge = 0;
		k3d::legacy::split_edge* last_interior_edge = 0;

		k3d::legacy::split_edge* first_edge = Face->first_edge;
		k3d::legacy::split_edge* edge = first_edge;
		bool used_original_face = false;
		std::vector<k3d::legacy::split_edge*> removed_edges;
		do
		{
			removed_edges.push_back(edge);

			face_barycenter += k3d::to_vector(edge->vertex->position);

			// Create edges
			k3d::legacy::split_edge* edge0 = new k3d::legacy::split_edge(edge->vertex);
			k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(edge->face_clockwise->vertex);
			k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(center_point);
			edge0->face_clockwise = edge1;
			edge1->face_clockwise = edge2;
			edge2->face_clockwise = edge0;

			// Set companions
			if(edge->companion)
				k3d::legacy::join_edges(*edge0, *edge->companion);

			if(previous_interior_edge)
				k3d::legacy::join_edges(*previous_interior_edge, *edge2);
			else
				last_interior_edge = edge2;

			previous_interior_edge = edge1;

			edge = edge->face_clockwise;

			if(edge == first_edge)
				k3d::legacy::join_edges(*edge1, *last_interior_edge);

			// Connect to face
			if(!used_original_face)
			{
				Face->first_edge = edge0;
				used_original_face = true;
			}
			else
			{
				k3d::legacy::face* face = new k3d::legacy::face(edge0, Face->material);
				face->selection_weight = 1.0;

				new_faces.push_back(face);
				polyhedron.faces.push_back(face);
			}
		}
		while(edge != first_edge);

		// Save real barycenter
		face_barycenter /= static_cast<double>(removed_edges.size());
		center_point->position = face_barycenter;

		// Insert original face to new_faces list
		new_faces.push_back(Face);

		// Delete subdivided face
		std::for_each(removed_edges.begin(), removed_edges.end(), delete_bogus_edge());
	}

	k3d::legacy::mesh* mesh;
	std::vector<k3d::legacy::face*>& new_faces;
	k3d::legacy::polyhedron& polyhedron;
};

/// Joins face midpoints to barycenter
typedef std::set<k3d::legacy::point*> point_set_t;
struct center_to_midpoints
{
	center_to_midpoints(k3d::legacy::mesh* Mesh, std::vector<k3d::legacy::face*>& NewFaces, middle_points_t& MiddlePoints, k3d::legacy::polyhedron& Polyhedron) :
		mesh(Mesh), new_faces(NewFaces), middle_points(MiddlePoints), polyhedron(Polyhedron)
	{
	}

	void operator()(k3d::legacy::face* Face)
	{
		if(!Face->selection_weight)
			return;

		// Create barycenter and midpoints
		std::vector<k3d::legacy::point*> new_points;
		std::vector<k3d::legacy::split_edge*> companions;
		k3d::point3 face_barycenter = k3d::point3(0, 0, 0);
		std::vector<k3d::legacy::split_edge*> removed_edges;

		bool used_original_face = false;
		k3d::legacy::split_edge* edge = Face->first_edge;
		unsigned long original_vertices = 0;
		do
		{
			removed_edges.push_back(edge);

			// Edge midpoint
			point_set_t::iterator is_before_midpoint = new_midpoints.find(edge->face_clockwise->vertex);
			point_set_t::iterator is_midpoint = new_midpoints.find(edge->vertex);

			if(is_before_midpoint == new_midpoints.end() && is_midpoint == new_midpoints.end())
			{
				const k3d::point3 first_vertex = edge->vertex->position;
				const k3d::point3 last_vertex = edge->face_clockwise->vertex->position;
				const k3d::point3 midposition = (first_vertex + last_vertex) * 0.5;
				k3d::legacy::point* midpoint = new k3d::legacy::point(midposition);
				new_midpoints.insert(midpoint);

				mesh->points.push_back(midpoint);

				new_points.push_back(edge->vertex);
				new_points.push_back(midpoint);
				middle_points.push_back(middle_point(first_vertex, last_vertex, *midpoint));

				// Split companion in two (second test checks for faces that form a loop)
				if(edge->companion && !helpers::edge_in_face(edge->companion, Face->first_edge))
				{
					k3d::legacy::split_edge* second_edge = new k3d::legacy::split_edge(midpoint);
					k3d::legacy::split_edge* first_edge = edge->companion;
					second_edge->face_clockwise = first_edge->face_clockwise;
					first_edge->face_clockwise = second_edge;
					companions.push_back(second_edge);
					companions.push_back(first_edge);
				}
				else
				{
					companions.push_back(0);
					companions.push_back(0);
				}

				face_barycenter += k3d::to_vector(edge->vertex->position);
				++original_vertices;
			}
			else
			{
				// Don't split edges that already have a midpoint
				new_points.push_back(edge->vertex);
				companions.push_back(edge->companion);

				if(is_midpoint == new_midpoints.end())
				{
					face_barycenter += k3d::to_vector(edge->vertex->position);
					++original_vertices;
				}
			}

			edge = edge->face_clockwise;
		}
		while(edge != Face->first_edge);

		face_barycenter /= static_cast<double>(original_vertices);
		k3d::legacy::point* center_point = new k3d::legacy::point(face_barycenter);
		mesh->points.push_back(center_point);

		// Create subfaces
		k3d::legacy::split_edge* previous_interior_edge = 0;
		k3d::legacy::split_edge* last_interior_edge = 0;

		const unsigned long new_points_size = new_points.size();
		for(unsigned long i = 0; i != new_points_size; i += 2)
		{
			k3d::legacy::split_edge* edge0 = new k3d::legacy::split_edge(new_points[(i+1) % new_points_size]);
			k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(new_points[(i+2) % new_points_size]);
			k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(new_points[(i+3) % new_points_size]);
			k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(center_point);

			edge0->face_clockwise = edge1;
			edge1->face_clockwise = edge2;
			edge2->face_clockwise = edge3;
			edge3->face_clockwise = edge0;

			k3d::legacy::split_edge* companion0 = companions[(i+1) % new_points_size];
			k3d::legacy::split_edge* companion1 = companions[(i+2) % new_points_size];
			if(companion0)
				k3d::legacy::join_edges(*edge0, *companion0);
			if(companion1)
				k3d::legacy::join_edges(*edge1, *companion1);

			if(!used_original_face)
			{
				Face->first_edge = edge0;
				used_original_face = true;
			}
			else
			{
				k3d::legacy::face* face = new k3d::legacy::face(edge0, Face->material);
				face->selection_weight = 1.0;

				new_faces.push_back(face);
				polyhedron.faces.push_back(face);
			}

			if(previous_interior_edge)
				k3d::legacy::join_edges(*previous_interior_edge, *edge3);
			else
				last_interior_edge = edge3;

			previous_interior_edge = edge2;

			if(i == new_points_size - 2)
				k3d::legacy::join_edges(*edge2, *last_interior_edge);
		}

		// Insert original face to new_faces list
		new_faces.push_back(Face);

		// Delete original edges
		std::for_each(removed_edges.begin(), removed_edges.end(), delete_bogus_edge());
	}

	point_set_t new_midpoints;

	k3d::legacy::mesh* mesh;
	std::vector<k3d::legacy::face*>& new_faces;
	middle_points_t& middle_points;
	k3d::legacy::polyhedron& polyhedron;
};

/// Joins face midpoints
struct midpoints
{
	midpoints(k3d::legacy::mesh* Mesh, std::vector<k3d::legacy::face*>& NewFaces, middle_points_t& MiddlePoints, k3d::legacy::polyhedron& Polyhedron) :
		mesh(Mesh), new_faces(NewFaces), middle_points(MiddlePoints), polyhedron(Polyhedron)
	{
	}

	void operator()(k3d::legacy::face* Face)
	{
		if(!Face->selection_weight)
			return;

		// Create midpoints
		std::vector<k3d::legacy::point*> new_points;
		std::vector<k3d::legacy::split_edge*> companions;
		std::vector<k3d::legacy::split_edge*> removed_edges;

		k3d::legacy::split_edge* edge = Face->first_edge;
		do
		{
			removed_edges.push_back(edge);

			// Edge midpoint
			point_set_t::iterator is_before_midpoint = new_midpoints.find(edge->face_clockwise->vertex);
			point_set_t::iterator is_midpoint = new_midpoints.find(edge->vertex);

			if(is_before_midpoint == new_midpoints.end() && is_midpoint == new_midpoints.end())
			{
				const k3d::point3 first_vertex = edge->vertex->position;
				const k3d::point3 last_vertex = edge->face_clockwise->vertex->position;
				const k3d::point3 midposition = (first_vertex + last_vertex) * 0.5;
				k3d::legacy::point* midpoint = new k3d::legacy::point(midposition);
				new_midpoints.insert(midpoint);

				mesh->points.push_back(midpoint);

				new_points.push_back(edge->vertex);
				new_points.push_back(midpoint);
				middle_points.push_back(middle_point(first_vertex, last_vertex, *midpoint));

				// Split companion in two (second test checks for faces that form a loop)
				if(edge->companion && !helpers::edge_in_face(edge->companion, Face->first_edge))
				{
					k3d::legacy::split_edge* second_edge = new k3d::legacy::split_edge(midpoint);
					k3d::legacy::split_edge* first_edge = edge->companion;
					second_edge->face_clockwise = first_edge->face_clockwise;
					first_edge->face_clockwise = second_edge;
					companions.push_back(second_edge);
					companions.push_back(first_edge);
				}
				else
				{
					companions.push_back(0);
					companions.push_back(0);
				}
			}
			else
			{
				// Don't split edges that already have a midpoint
				new_points.push_back(edge->vertex);
				companions.push_back(edge->companion);
			}

			edge = edge->face_clockwise;
		}
		while(edge != Face->first_edge);

		// Create subfaces
		const unsigned long new_points_size = new_points.size();
		std::vector<k3d::legacy::split_edge*> middle_face;
		for(unsigned long i = 0; i != new_points_size; i += 2)
		{
			k3d::legacy::split_edge* edge0 = new k3d::legacy::split_edge(new_points[(i+1) % new_points_size]);
			k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(new_points[(i+2) % new_points_size]);
			k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(new_points[(i+3) % new_points_size]);

			edge0->face_clockwise = edge1;
			edge1->face_clockwise = edge2;
			edge2->face_clockwise = edge0;

			k3d::legacy::split_edge* companion0 = companions[(i+1) % new_points_size];
			k3d::legacy::split_edge* companion1 = companions[(i+2) % new_points_size];
			if(companion0)
				k3d::legacy::join_edges(*edge0, *companion0);
			if(companion1)
				k3d::legacy::join_edges(*edge1, *companion1);
			middle_face.push_back(edge2);

			k3d::legacy::face* face = new k3d::legacy::face(edge0, Face->material);
			face->selection_weight = 1.0;

			new_faces.push_back(face);
			polyhedron.faces.push_back(face);
		}

		// Middle face replaces original one
		std::vector<k3d::legacy::split_edge*> middle_edges;
		for(std::vector<k3d::legacy::split_edge*>::iterator edge = middle_face.begin(); edge != middle_face.end(); ++edge)
		{
			k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge((*edge)->face_clockwise->vertex);
			middle_edges.push_back(new_edge);
			k3d::legacy::join_edges(*new_edge, **edge);
		}

		k3d::legacy::loop_edges(middle_edges.begin(), middle_edges.end());
		Face->first_edge = middle_edges.front();

		// Insert original face to new_faces list
		new_faces.push_back(Face);

		// Delete original edges
		std::for_each(removed_edges.begin(), removed_edges.end(), delete_bogus_edge());
	}

private:
	point_set_t new_midpoints;

	k3d::legacy::mesh* mesh;
	std::vector<k3d::legacy::face*>& new_faces;
	middle_points_t& middle_points;
	k3d::legacy::polyhedron& polyhedron;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// subdivide_faces

class subdivide_faces :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	subdivide_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_subdivision_type(init_owner(*this) + init_name("subdivision_type") + init_label(_("Subdivision type")) + init_description(_("Join : center to midpoints, midpoints, center to points")) + init_value(CENTERTOMIDPOINTS) + init_enumeration(subdivision_values())),
		m_middle_offset(init_owner(*this) + init_name("middle_offset") + init_label(_("Middle offset")) + init_description(_("Middle offset, for midpoints only")) + init_value(0.0) + init_step_increment(0.05) + init_units(typeid(k3d::measurement::distance)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_subdivision_type.changed_signal().connect(make_reset_mesh_slot());

		m_middle_offset.changed_signal().connect(make_update_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		m_middle_points.clear();
		m_new_faces.clear();

		// Create output geometry ...
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		const subdivision_t subdivision_type = m_subdivision_type.pipeline_value();

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron_i = Mesh.polyhedra.begin(); polyhedron_i != Mesh.polyhedra.end(); ++polyhedron_i)
		{
			k3d::legacy::polyhedron& polyhedron = **polyhedron_i;

			// Get the set of all selected faces ...
			k3d::legacy::polyhedron::faces_t selected_faces;
			k3d::copy_if(polyhedron.faces.begin(), polyhedron.faces.end(), std::inserter(selected_faces, selected_faces.end()), k3d::is_selected());

			if(selected_faces.empty())
				selected_faces = polyhedron.faces;

			// Subdivide each selected face
			switch(subdivision_type)
			{
				case CENTERTOPOINTS:
					std::for_each(selected_faces.begin(), selected_faces.end(), detail::center_to_points(&Mesh, m_new_faces, polyhedron));
				break;

				case CENTERTOMIDPOINTS:
					std::for_each(selected_faces.begin(), selected_faces.end(), detail::center_to_midpoints(&Mesh, m_new_faces, m_middle_points, polyhedron));
				break;

				case CONTIGUOUSMIDPOINTS:
					std::for_each(selected_faces.begin(), selected_faces.end(), detail::midpoints(&Mesh, m_new_faces, m_middle_points, polyhedron));
				break;

				default:
					assert_not_reached();
			}

			assert_warning(is_valid(polyhedron));
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const double offset = m_middle_offset.pipeline_value();

		for(detail::middle_points_t::iterator point = m_middle_points.begin(); point != m_middle_points.end(); ++point)
			point->update(offset);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<subdivide_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xb9d4d815, 0x241c473d, 0xa22d3523, 0x406fa390),
				"SubdivideFaces",
				"Subdivide faces by joining face center with vertices or edge midpoints, or contiguous midpoints",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		CENTERTOPOINTS,
		CENTERTOMIDPOINTS,
		CONTIGUOUSMIDPOINTS,
	} subdivision_t;

	friend std::ostream& operator << (std::ostream& Stream, const subdivision_t& Value)
	{
		switch(Value)
		{
			case CENTERTOPOINTS:
				Stream << "center";
				break;
			case CENTERTOMIDPOINTS:
				Stream << "centermidpoints";
				break;
			case CONTIGUOUSMIDPOINTS:
				Stream << "midpoints";
				break;
		}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, subdivision_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "center")
			Value = CENTERTOPOINTS;
		else if(text == "centermidpoints")
			Value = CENTERTOMIDPOINTS;
		else if(text == "midpoints")
			Value = CONTIGUOUSMIDPOINTS;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& subdivision_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Center", "center", "Joins each face vertex to face center; fun fact: produces triangles only"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("CenterMidpoints", "centermidpoints", "Joins face center to each edge middle; innovative info: produces quads only"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Midpoints", "midpoints", "Joins contiguous edge midpoints; amusing anecdote: produces one triangle per original vertex, plus a polygon in the middle with as many edges as the original"));
		}

		return values;
	}

	k3d_data(subdivision_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_subdivision_type;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_middle_offset;

private:
	/// Caches new geometry for better interactive performance
	detail::middle_points_t m_middle_points;
	k3d::legacy::polyhedron::faces_t m_new_faces;
};

/////////////////////////////////////////////////////////////////////////////
// subdivide_faces_factory

k3d::iplugin_factory& subdivide_faces_factory()
{
	return subdivide_faces::get_factory();
}

} // namespace libk3dmesh

