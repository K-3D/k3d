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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>

namespace module
{

namespace polyhedron
{

/*
namespace detail
{

typedef std::list<k3d::legacy::split_edge*> edges_t;
typedef std::set<k3d::legacy::split_edge*> face_edges_t;

typedef std::map<k3d::legacy::point*, k3d::legacy::point*> point_map_t;

/// Insert a collapsed edge to point map
void insert_collapsed_edge(k3d::legacy::point* Start, k3d::legacy::point* End, point_map_t& PointMap)
{
	PointMap.insert(std::make_pair(Start, End));

	// Make sure successive collapses point to the right endpoint
	bool found_update;
	do
	{
		found_update = false;

		for(point_map_t::iterator edge = PointMap.begin(); edge != PointMap.end(); ++edge)
		{
			k3d::legacy::point* end_point = edge->second;

			point_map_t::iterator collapsed_edge = PointMap.find(end_point);
			if(collapsed_edge != PointMap.end())
			{
				found_update = true;

				// Replace endpoint with final one
				edge->second = collapsed_edge->second;

				// Prevent loops
				if(edge->first == edge->second)
				{
					PointMap.erase(edge);
					break;
				}
			}
		}
	}
	while(found_update);
}

/// Collapses given edge, selects its (possible) companion for later collapsing
// returns true if the face is flattened and removed, false otherwise
bool collapse_split_edge(k3d::legacy::face* Face, k3d::legacy::split_edge* Edge, point_map_t& PointMap)
{
	// Map start_point to end_point
	insert_collapsed_edge(Edge->vertex, Edge->face_clockwise->vertex, PointMap);

	// Select companion for deletion
	if(Edge->companion)
	{
		Edge->companion->companion = 0;
		Edge->companion->selection_weight = 1.0;
	}

	// If face has more than 3 edges, just delete Edge
	if(helpers::edge_number(Edge) > 3)
	{
		k3d::legacy::split_edge* anticlockwise = k3d::legacy::face_anticlockwise(Edge);
		anticlockwise->face_clockwise = Edge->face_clockwise;

		if(Face->first_edge == Edge)
			Face->first_edge = anticlockwise;

		delete Edge;

		return false;
	}

	// Flatten triangle ...
	k3d::legacy::split_edge* glue_edge1 = Edge->face_clockwise;
	k3d::legacy::split_edge* glue_edge2 = Edge->face_clockwise->face_clockwise;
	assert_warning(glue_edge2->face_clockwise == Edge);

	// Save selection
	bool selected = false;
	if(glue_edge1->selection_weight || glue_edge2->selection_weight)
		selected = true;

	// Glue the two other edges
	k3d::legacy::split_edge* companion1 = glue_edge1->companion;
	k3d::legacy::split_edge* companion2 = glue_edge2->companion;

	if(companion1)
		companion1->companion = companion2;
	if(companion2)
		companion2->companion = companion1;

	// Set selection
	if(selected)
	{
		if(companion1)
			companion1->selection_weight = 1.0;
		if(companion2)
			companion2->selection_weight = 1.0;
	}

	// Delete face
	delete Edge;
	glue_edge1->companion = 0;
	delete glue_edge1;
	glue_edge2->companion = 0;
	delete glue_edge2;
	Face->first_edge = 0;

	return true;
}

/// Collapses first selected edge found and its companion, returns false when no selected edge is found
bool collapse_selected_edge(k3d::legacy::polyhedron& Polyhedron, point_map_t& PointMap)
{
	k3d::legacy::polyhedron::faces_t::iterator face;
	k3d::legacy::split_edge* selected_edge = 0;
	k3d::legacy::face* edge_face = 0;
	for(face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		k3d::legacy::split_edge* edge = (*face)->first_edge;
		do
		{
			if(edge->selection_weight || (edge->companion && edge->companion->selection_weight))
			{
				selected_edge = edge;
				edge_face = *face;

				goto FoundEdge;
			}

			edge = edge->face_clockwise;
		}
		while(edge != (*face)->first_edge);
	}

	return false;

	FoundEdge:

	// Save companion
	k3d::legacy::split_edge* companion = selected_edge->companion;

	// Collapse edge
	if(collapse_split_edge(*face, selected_edge, PointMap))
	{
		delete *face;
		Polyhedron.faces.erase(face);

		return true;
	}

	// Collapse companion
	if(!companion)
		return true;

	// Find companion's face
	for(face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
		if(helpers::edge_in_face(companion, (*face)->first_edge))
			break;

	return_val_if_fail(face != Polyhedron.faces.end(), false);

	if(collapse_split_edge(*face, companion, PointMap))
	{
		delete *face;
		Polyhedron.faces.erase(face);
	}

	return true;
}

} // namespace detail
*/

/////////////////////////////////////////////////////////////////////////////
// collapse_faces

class collapse_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	collapse_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

/*
		// Change face selection to an edge selection, then apply CollapseEdges
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				const double weight = (*face)->selection_weight ? 1.0 : 0.0;

				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					edge->selection_weight = weight;

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}
		}

		// Keep track of replaced points
		detail::point_map_t point_map;

		// Collapse edges
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			bool found_edge = false;
			do
			{
				found_edge = detail::collapse_selected_edge(**polyhedron, point_map);
			}
			while(found_edge);
		}

		// Update point positions
		typedef std::vector<k3d::legacy::point*> point_list_t;
		typedef std::map<k3d::legacy::point*, point_list_t> collapsed_points_t;
		collapsed_points_t collapsed_points;
		for(detail::point_map_t::iterator pair = point_map.begin(); pair != point_map.end(); ++pair)
		{
			// List original positions for each final point
			collapsed_points_t::iterator list = collapsed_points.find(pair->second);
			if(list == collapsed_points.end())
			{
				point_list_t first_position;
				first_position.push_back(pair->first);

				collapsed_points.insert(std::make_pair(pair->second, first_position));
			}
			else
			{
				list->second.push_back(pair->first);
			}
		}

		for(collapsed_points_t::iterator pair = collapsed_points.begin(); pair != collapsed_points.end(); ++pair)
		{
			point_list_t& list = pair->second;
			k3d::legacy::point* end = pair->first;

			// More than one point were collapsed to the destination, set centroid
			k3d::point3 sum = end->position;
			for(point_list_t::const_iterator vertex = list.begin(); vertex != list.end(); ++vertex)
				sum += k3d::to_vector((*vertex)->position);

			end->position = sum / static_cast<double>((list.size() + 1));
		}

		// Replace removed points in geometric primitives
		k3d::legacy::for_each_component(Mesh, replace_removed_points(point_map));

		// Delete removed points and select points resulting from collapsed edges
		for(detail::point_map_t::iterator point = point_map.begin(); point != point_map.end(); ++point)
		{
			Mesh.points.erase(std::remove(Mesh.points.begin(), Mesh.points.end(), point->first), Mesh.points.end());

			point->second->selection_weight = 1.0;
		}

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
			assert_warning(k3d::legacy::is_valid(**polyhedron));
*/
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

/*
	class replace_removed_points
	{
	public:
		replace_removed_points(detail::point_map_t& PointMap) :
			point_map(PointMap)
		{
		}

		void operator()(k3d::legacy::point&) { }
		void operator()(k3d::legacy::face&) { }
		void operator()(k3d::legacy::split_edge& Edge)
		{
			point = point_map.find(Edge.vertex);
			if(point != point_map.end())
			{
				Edge.vertex = point->second;
			}
		}
		void operator()(k3d::legacy::linear_curve& Curve)
		{
			for(k3d::legacy::linear_curve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
			{
				point = point_map.find(*curve_point);
				if(point != point_map.end())
				{
					*curve_point = point->second;
				}
			}
		}
		void operator()(k3d::legacy::linear_curve_group&) { }
		void operator()(k3d::legacy::cubic_curve& Curve)
		{
			for(k3d::legacy::cubic_curve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
			{
				point = point_map.find(*curve_point);
				if(point != point_map.end())
				{
					*curve_point = point->second;
				}
			}
		}
		void operator()(k3d::legacy::cubic_curve_group&) { }
		void operator()(k3d::legacy::nucurve& Curve)
		{
			for(k3d::legacy::nucurve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
			{
				point = point_map.find(curve_point->position);
				if(point != point_map.end())
				{
					curve_point->position = point->second;
				}
			}
		}
		void operator()(k3d::legacy::nucurve_group&) { }
		void operator()(k3d::legacy::bilinear_patch& Patch)
		{
			for(k3d::legacy::bilinear_patch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
			{
				point = point_map.find(*control_point);
				if(point != point_map.end())
				{
					*control_point = point->second;
				}
			}
		}
		void operator()(k3d::legacy::bicubic_patch& Patch)
		{
			for(k3d::legacy::bicubic_patch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
			{
				point = point_map.find(*control_point);
				if(point != point_map.end())
				{
					*control_point = point->second;
				}
			}
		}
		void operator()(k3d::legacy::nupatch& Patch)
		{
			for(k3d::legacy::nupatch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
			{
				point = point_map.find(control_point->position);
				if(point != point_map.end())
				{
					control_point->position = point->second;
				}
			}
		}
		void operator()(k3d::legacy::polyhedron&) { }
		void operator()(k3d::legacy::mesh&) { }

	private:
		detail::point_map_t& point_map;
		detail::point_map_t::iterator point;
	};
*/

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<collapse_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x13f8a224, 0x5e284ac4, 0xb5b58ebd, 0x10f65bb6),
				"CollapseFaces",
				"For each selected face, collapses its vertices to the centroid",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// factories

k3d::iplugin_factory& collapse_faces_factory()
{
	return collapse_faces::get_factory();
}

} // namespace polyhedron

} // namespace module

