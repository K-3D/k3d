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
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>

#include "helpers.h"

#include <list>
#include <map>

namespace libk3dmesh
{

namespace detail
{

typedef std::map<k3d::legacy::point*, unsigned long> point_and_face_number_t;
typedef std::vector<k3d::legacy::face*> faces_t;

struct face_edge_t
{
	face_edge_t(k3d::legacy::split_edge* Edge, k3d::imaterial* Material) :
		edge(Edge), material(Material)
	{
	}

	k3d::legacy::split_edge* edge;
	k3d::imaterial* material;
};

class fill_hole
{
public:
	fill_hole(point_and_face_number_t& BorderPointValencies, faces_t& Faces) :
		m_border_points(BorderPointValencies),
		faces(Faces)
	{
	}

	void operator()(face_edge_t& Edge)
	{
		// Unselect edge
		Edge.edge->selection_weight = 0;

		// Exit if hole was previously filled
		if(Edge.edge->companion)
			return;

		// Find a border edge loop that will make a new polygon
		typedef std::vector<k3d::legacy::split_edge*> edges_t;
		edges_t border_edges;

		// Handle holes that share one vertex
		typedef std::map<k3d::legacy::point*, edges_t> shared_points_t;
		shared_points_t shared_points;

		k3d::legacy::split_edge* edge = Edge.edge;
		do
		{
			border_edges.push_back(edge);

			// Find next border edge
			edge = edge->face_clockwise;
			while(edge->companion)
			{
				// Turn around vertex until a border is found
				edge = edge->companion->face_clockwise;
			}

			point_and_face_number_t::iterator point = m_border_points.find(edge->vertex);
			if(point->second > 1)
			{
				// More than one hole meet at this point
				k3d::legacy::point* vertex = edge->face_clockwise->vertex;
				shared_points_t::iterator edges = shared_points.find(vertex);
				if(edges == shared_points.end())
				{
					edges_t list;
					list.push_back(edge);

					shared_points.insert(std::make_pair(vertex, list));
				}
				else
				{
					// Add edge to vertex' list
					edges->second.push_back(edge);
				}
			}
		}
		while(edge != Edge.edge);

		if(!shared_points.size())
		{
			// Create polygon
			k3d::legacy::face* face = helpers::fill_hole(border_edges.rbegin(), border_edges.rend(), Edge.material);
			faces.push_back(face);

			return;
		}

		// Edge loop goes through several holes linked at single points
		//while(border_edges.size())
		{
			// Compute shared face normal (using the summation method)
			k3d::vector3 normal(0, 0, 0);
			for(edges_t::iterator edge_i = border_edges.begin(); edge_i != border_edges.end(); ++edge_i)
			{
				edges_t::iterator next_edge = edge_i + 1;
				if(next_edge == border_edges.end())
					next_edge = border_edges.begin();

				const k3d::point3& i = (*edge_i)->vertex->position;
				const k3d::point3& j = (*next_edge)->vertex->position;

				normal[0] += (i[1] + j[1]) * (j[2] - i[2]);
				normal[1] += (i[2] + j[2]) * (j[0] - i[0]);
				normal[2] += (i[0] + j[0]) * (j[1] - i[1]);
			}

			// Find separate holes
			edges_t hole_edges;
			k3d::legacy::split_edge* first_edge = border_edges.front();

			edge = first_edge;

			do
			{
				k3d::legacy::split_edge* next_edge = edge->face_clockwise;
				shared_points_t::iterator edges = shared_points.find(next_edge->vertex);
				if(edges != shared_points.end())
				{
					// Find edge with smallest clockwise angle with next_edge
					k3d::legacy::split_edge* best_edge = 0;
					double best_cos = 0;
					bool best_first_half = true;
					for(edges_t::iterator shared_edge = edges->second.begin(); shared_edge != edges->second.end(); ++shared_edge)
					{
						// Compute angle with previous edge
						k3d::vector3 vector1 = k3d::to_vector(edge->vertex->position - edge->face_clockwise->vertex->position);
						k3d::vector3 point2 = k3d::to_vector(next_edge->face_clockwise->vertex->position - next_edge->vertex->position);
						const double cos = k3d::normalize(vector1) * k3d::normalize(point2);

						vector1 = k3d::to_vector(edge->face_clockwise->vertex->position - edge->vertex->position);
						point2 = k3d::to_vector(next_edge->vertex->position - next_edge->vertex->position);
						const k3d::vector3 edges_normal = vector1 ^ point2;
						const bool first_half = (edges_normal * normal > 0);

						if(!best_edge)
						{
							best_edge = *shared_edge;
							best_cos = cos;
							best_first_half = first_half;
						}
						else
						{
							if(best_first_half)
							{
								if(first_half)
								{
									if(cos > best_cos)
									{
										best_edge = *shared_edge;
										best_cos = cos;
										best_first_half = first_half;
									}
								}
							}
							else
							{
								if(first_half)
								{
									best_edge = *shared_edge;
									best_cos = cos;
									best_first_half = first_half;
								}
								else
								{
									if(cos < best_cos)
									{
										best_edge = *shared_edge;
										best_cos = cos;
										best_first_half = first_half;
									}
								}
							}
						}
					}

					return_if_fail(best_edge);
					next_edge = best_edge;
				}

				hole_edges.push_back(next_edge);

				edge = next_edge;
			}
			while(edge->face_clockwise != first_edge);
		}
	}

private:
	point_and_face_number_t& m_border_points;
	faces_t& faces;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// fill_selected_holes_implementation

class fill_selected_holes_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	fill_selected_holes_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		k3d::replace_selection(m_mesh_selection.value(), Mesh);

		m_new_faces.clear();
		typedef std::list<detail::face_edge_t> edges_t;
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron_i = Mesh.polyhedra.begin(); polyhedron_i != Mesh.polyhedra.end(); ++polyhedron_i)
		{
			k3d::legacy::polyhedron& polyhedron = **polyhedron_i;

			// Find selected border edges and count how many faces share each border vertex
			edges_t selected_border_edges;
			detail::point_and_face_number_t border_points;

			for(k3d::legacy::polyhedron::faces_t::const_iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					// Save selected border edges
					if(edge->selection_weight > 0 && !edge->companion)
					{
						selected_border_edges.push_back(detail::face_edge_t(edge, (*face)->material));
					}

					// Save shared face number for border points
					if(!edge->companion)
					{
						detail::point_and_face_number_t::iterator point = border_points.find(edge->vertex);
						if(point == border_points.end())
							border_points.insert(std::make_pair(edge->vertex, 1));
						else
							++(border_points[edge->vertex]);
					}

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}

			// Fill holes
			detail::faces_t new_faces;
			std::for_each(selected_border_edges.begin(), selected_border_edges.end(), detail::fill_hole(border_points, new_faces));

			// Save new faces
			polyhedron.faces.insert(polyhedron.faces.end(), new_faces.begin(), new_faces.end());
			m_new_faces.insert(m_new_faces.end(), new_faces.begin(), new_faces.end());
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<fill_selected_holes_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x34524730, 0x7ba14abf, 0xb1922641, 0xf64c68ce),
				"FillSelectedHoles",
				"Fills holes by creating polygons containing selected border edges",
				"Mesh",
				k3d::iplugin_factory::DEPRECATED);

		return factory;
	}
private:
	detail::faces_t m_new_faces;
};

/////////////////////////////////////////////////////////////////////////////
// fill_selected_holes_factory

k3d::iplugin_factory& fill_selected_holes_factory()
{
	return fill_selected_holes_implementation::get_factory();
}

} // namespace libk3dmesh

