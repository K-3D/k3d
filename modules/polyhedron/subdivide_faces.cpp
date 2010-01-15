// K-3D
// Copyright (c) 2004-2010, Timothy M. Shead
//
// Contact: tshead@k-3d.com
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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// subdivide_faces

class subdivide_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	subdivide_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_subdivision_type(init_owner(*this) + init_name("subdivision_type") + init_label(_("Subdivision type")) + init_description(_("Join : center to midpoints, midpoints, center to points")) + init_value(CENTER_MIDPOINTS) + init_enumeration(subdivision_values()))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_subdivision_type.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points)
			return;
		if(!Output.point_selection)
			return;

		k3d::mesh::points_t& points = Output.points.writable();
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();
		k3d::table_copier point_attributes(Output.point_attributes);

		const subdivision_t subdivision_type = m_subdivision_type.pipeline_value();

		// Don't explicitly remove any points ...
		k3d::mesh::bools_t remove_points(points.size(), false);

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Get ready to copy attributes ...
			k3d::table_copier face_attributes(polyhedron->face_attributes);
			k3d::table_copier edge_attributes(polyhedron->edge_attributes);
			k3d::table_copier vertex_attributes(polyhedron->vertex_attributes);

			// Don't explicitly delete any edges ...
			k3d::mesh::bools_t remove_edges(polyhedron->clockwise_edges.size(), false);

			// Don't explicitly delete any loops ...
			k3d::mesh::bools_t remove_loops(polyhedron->loop_first_edges.size(), false);

			// Keep track of faces to be deleted ...
			k3d::mesh::bools_t remove_faces(polyhedron->face_shells.size(), false);

			// Subdivide the faces ...
			switch(subdivision_type)
			{
				case CENTER:
				{
					// For each selected face ...
					const k3d::uint_t face_begin = 0;
					const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
					for(k3d::uint_t face = face_begin; face != face_end; ++face)
					{
						if(!polyhedron->face_selections[face])
							continue;

						const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];

						// Generate a center point ...
						k3d::mesh::indices_t center_point_indices;
						for(k3d::uint_t edge = first_edge; ; )
						{
							center_point_indices.push_back(polyhedron->vertex_points[edge]);

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}

						const k3d::uint_t center_point = points.size();
						k3d::mesh::weights_t center_point_weights(center_point_indices.size(), 1.0 / center_point_indices.size());

						k3d::point3 average(0, 0, 0);
						for(k3d::uint_t i = 0; i != center_point_indices.size(); ++i)
							average += k3d::to_vector(center_point_weights[i] * points[center_point_indices[i]]);
						points.push_back(average);
						point_selection.push_back(0);
						point_attributes.push_back(center_point_indices.size(), &center_point_indices[0], &center_point_weights[0]);
						remove_points.push_back(false);

						// Generate new faces ...
						for(k3d::uint_t edge = first_edge; ; )
						{
							polyhedron->face_shells.push_back(polyhedron->face_shells[face]);
							polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
							polyhedron->face_loop_counts.push_back(1);
							polyhedron->face_selections.push_back(1);
							polyhedron->face_materials.push_back(polyhedron->face_materials[face]);
							face_attributes.push_back(face);

							remove_faces.push_back(false);
						
							polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

							remove_loops.push_back(false);

							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() - 2);

							polyhedron->edge_selections.push_back(0);
							polyhedron->edge_selections.push_back(0);
							polyhedron->edge_selections.push_back(0);

							edge_attributes.push_back(edge);
							edge_attributes.push_back(edge);
							edge_attributes.push_back(edge);

							remove_edges.push_back(false);
							remove_edges.push_back(false);
							remove_edges.push_back(false);

							polyhedron->vertex_points.push_back(polyhedron->vertex_points[edge]);
							polyhedron->vertex_points.push_back(polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]);
							polyhedron->vertex_points.push_back(center_point);

							polyhedron->vertex_selections.push_back(0);
							polyhedron->vertex_selections.push_back(0);
							polyhedron->vertex_selections.push_back(0);

							vertex_attributes.push_back(edge);
							vertex_attributes.push_back(polyhedron->clockwise_edges[edge]);

							const k3d::uint_t vertex_indices[2] = { edge, polyhedron->clockwise_edges[edge] };
							const k3d::double_t vertex_weights[2] = { 0.5, 0.5 };
							vertex_attributes.push_back(2, vertex_indices, vertex_weights);

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}
						remove_faces[face] = true;
					}

					break;
				}
				case CENTER_MIDPOINTS:
				{
					// Generate adjacent-edge lookups ...
					k3d::mesh::bools_t boundary_edges;
					k3d::mesh::indices_t adjacent_edges;
					k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

					// Keep track of newly-created edge midpoints ...
					std::vector<boost::optional<k3d::uint_t> > edge_midpoints(polyhedron->clockwise_edges.size());
	
					// For each selected face ...
					const k3d::uint_t face_begin = 0;
					const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
					for(k3d::uint_t face = face_begin; face != face_end; ++face)
					{
						if(!polyhedron->face_selections[face])
							continue;

						const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];

						// Generate a center point ...
						k3d::mesh::indices_t center_point_indices;
						for(k3d::uint_t edge = first_edge; ; )
						{
							center_point_indices.push_back(polyhedron->vertex_points[edge]);

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}

						const k3d::uint_t center_point = points.size();
						k3d::mesh::weights_t center_point_weights(center_point_indices.size(), 1.0 / center_point_indices.size());

						k3d::point3 average(0, 0, 0);
						for(k3d::uint_t i = 0; i != center_point_indices.size(); ++i)
							average += k3d::to_vector(center_point_weights[i] * points[center_point_indices[i]]);
						points.push_back(average);
						point_selection.push_back(0);
						point_attributes.push_back(center_point_indices.size(), &center_point_indices[0], &center_point_weights[0]);
						remove_points.push_back(false);

						// Generate midpoints ...
						for(k3d::uint_t edge = first_edge; ; )
						{
							// Don't create duplicate midpoints for adjacent edges ...
							if(!boundary_edges[edge] && edge_midpoints[adjacent_edges[edge]])
							{
								edge_midpoints[edge] = edge_midpoints[adjacent_edges[edge]];
							}
							else
							{
								edge_midpoints[edge] = points.size();

								points.push_back(k3d::mix(points[polyhedron->vertex_points[edge]], points[polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]], 0.5));
								point_selection.push_back(0);
								const k3d::uint_t point_indices[2] = { polyhedron->vertex_points[edge], polyhedron->vertex_points[polyhedron->clockwise_edges[edge]] };
								const k3d::double_t point_weights[2] = { 0.5, 0.5 };
								point_attributes.push_back(2, point_indices, point_weights);
								remove_points.push_back(false);
							}

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}

						// Generate new faces ...
						for(k3d::uint_t edge = first_edge; ; )
						{
							polyhedron->face_shells.push_back(polyhedron->face_shells[face]);
							polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
							polyhedron->face_loop_counts.push_back(1);
							polyhedron->face_selections.push_back(1);
							polyhedron->face_materials.push_back(polyhedron->face_materials[face]);
							face_attributes.push_back(face);

							remove_faces.push_back(false);
						
							polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

							remove_loops.push_back(false);

							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() - 3);

							polyhedron->edge_selections.push_back(0);
							polyhedron->edge_selections.push_back(0);
							polyhedron->edge_selections.push_back(0);
							polyhedron->edge_selections.push_back(0);

							edge_attributes.push_back(edge);
							edge_attributes.push_back(polyhedron->clockwise_edges[edge]);
							edge_attributes.push_back(polyhedron->clockwise_edges[edge]);
							edge_attributes.push_back(edge);

							remove_edges.push_back(false);
							remove_edges.push_back(false);
							remove_edges.push_back(false);
							remove_edges.push_back(false);

							polyhedron->vertex_points.push_back(*edge_midpoints[edge]);
							polyhedron->vertex_points.push_back(polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]);
							polyhedron->vertex_points.push_back(*edge_midpoints[polyhedron->clockwise_edges[edge]]);
							polyhedron->vertex_points.push_back(center_point);

							polyhedron->vertex_selections.push_back(0);
							polyhedron->vertex_selections.push_back(0);
							polyhedron->vertex_selections.push_back(0);
							polyhedron->vertex_selections.push_back(0);
				
							{	
								const k3d::uint_t vertex_indices[2] = { edge, polyhedron->clockwise_edges[edge] };
								const k3d::double_t vertex_weights[2] = { 0.5, 0.5 };	
								vertex_attributes.push_back(2, vertex_indices, vertex_weights);
							}

							{	
								vertex_attributes.push_back(polyhedron->clockwise_edges[edge]);
							}

							{	
								const k3d::uint_t vertex_indices[2] = { polyhedron->clockwise_edges[edge], polyhedron->clockwise_edges[polyhedron->clockwise_edges[edge]] };
								const k3d::double_t vertex_weights[2] = { 0.5, 0.5 };
								vertex_attributes.push_back(2, vertex_indices, vertex_weights);
							}

							{	
								const k3d::uint_t vertex_indices[3] = { edge, polyhedron->clockwise_edges[edge], polyhedron->clockwise_edges[polyhedron->clockwise_edges[edge]] };
								const k3d::double_t vertex_weights[3] = { 0.25, 0.5, 0.25 };
								vertex_attributes.push_back(3, vertex_indices, vertex_weights);
							}

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}
						remove_faces[face] = true;
					}

					break;
				}
				case MIDPOINTS:
				{
					// Generate adjacent-edge lookups ...
					k3d::mesh::bools_t boundary_edges;
					k3d::mesh::indices_t adjacent_edges;
					k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

					// Keep track of newly-created edge midpoints ...
					std::vector<boost::optional<k3d::uint_t> > edge_midpoints(polyhedron->clockwise_edges.size());
	
					// For each selected face ...
					const k3d::uint_t face_begin = 0;
					const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
					for(k3d::uint_t face = face_begin; face != face_end; ++face)
					{
						if(!polyhedron->face_selections[face])
							continue;

						const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];

						// Generate midpoints ...
						for(k3d::uint_t edge = first_edge; ; )
						{
							// Don't create duplicate midpoints for adjacent edges ...
							if(!boundary_edges[edge] && edge_midpoints[adjacent_edges[edge]])
							{
								edge_midpoints[edge] = edge_midpoints[adjacent_edges[edge]];
							}
							else
							{
								edge_midpoints[edge] = points.size();

								points.push_back(k3d::mix(points[polyhedron->vertex_points[edge]], points[polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]], 0.5));
								point_selection.push_back(0);
								const k3d::uint_t point_indices[2] = { polyhedron->vertex_points[edge], polyhedron->vertex_points[polyhedron->clockwise_edges[edge]] };
								const k3d::double_t point_weights[2] = { 0.5, 0.5 };
								point_attributes.push_back(2, point_indices, point_weights);
								remove_points.push_back(false);
							}

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}

						// Generate new "corner" faces ...
						for(k3d::uint_t edge = first_edge; ; )
						{
							polyhedron->face_shells.push_back(polyhedron->face_shells[face]);
							polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
							polyhedron->face_loop_counts.push_back(1);
							polyhedron->face_selections.push_back(1);
							polyhedron->face_materials.push_back(polyhedron->face_materials[face]);
							face_attributes.push_back(face);

							remove_faces.push_back(false);
						
							polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

							remove_loops.push_back(false);

							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() - 2);

							polyhedron->edge_selections.push_back(0);
							polyhedron->edge_selections.push_back(0);
							polyhedron->edge_selections.push_back(0);

							edge_attributes.push_back(edge);
							edge_attributes.push_back(polyhedron->clockwise_edges[edge]);
							
							const k3d::uint_t edge_indices[2] = { edge, polyhedron->clockwise_edges[edge] };
							const k3d::double_t edge_weights[2] = { 0.5, 0.5 };
							edge_attributes.push_back(2, edge_indices, edge_weights);

							remove_edges.push_back(false);
							remove_edges.push_back(false);
							remove_edges.push_back(false);

							polyhedron->vertex_points.push_back(*edge_midpoints[edge]);
							polyhedron->vertex_points.push_back(polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]);
							polyhedron->vertex_points.push_back(*edge_midpoints[polyhedron->clockwise_edges[edge]]);

							polyhedron->vertex_selections.push_back(0);
							polyhedron->vertex_selections.push_back(0);
							polyhedron->vertex_selections.push_back(0);

							{	
								const k3d::uint_t vertex_indices[2] = { edge, polyhedron->clockwise_edges[edge] };
								const k3d::double_t vertex_weights[2] = { 0.5, 0.5 };	
								vertex_attributes.push_back(2, vertex_indices, vertex_weights);
							}

							{	
								vertex_attributes.push_back(polyhedron->clockwise_edges[edge]);
							}

							{	
								const k3d::uint_t vertex_indices[2] = { polyhedron->clockwise_edges[edge], polyhedron->clockwise_edges[polyhedron->clockwise_edges[edge]] };
								const k3d::double_t vertex_weights[2] = { 0.5, 0.5 };
								vertex_attributes.push_back(2, vertex_indices, vertex_weights);
							}

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}

						// Generate a new "center" face ...
						polyhedron->face_shells.push_back(polyhedron->face_shells[face]);
						polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
						polyhedron->face_loop_counts.push_back(1);
						polyhedron->face_selections.push_back(1);
						polyhedron->face_materials.push_back(polyhedron->face_materials[face]);
						face_attributes.push_back(face);

						remove_faces.push_back(false);
					
						polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

						remove_loops.push_back(false);

						const k3d::uint_t new_first_edge = polyhedron->clockwise_edges.size();
						for(k3d::uint_t edge = first_edge; ; )
						{
							polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
							polyhedron->edge_selections.push_back(0);

							const k3d::uint_t edge_indices[2] = { edge, polyhedron->clockwise_edges[edge] };
							const k3d::double_t edge_weights[2] = { 0.5, 0.5 };
							edge_attributes.push_back(2, edge_indices, edge_weights);

							remove_edges.push_back(false);

							polyhedron->vertex_points.push_back(*edge_midpoints[edge]);
							polyhedron->vertex_selections.push_back(0);

							const k3d::uint_t vertex_indices[2] = { edge, polyhedron->clockwise_edges[edge] };
							const k3d::double_t vertex_weights[2] = { 0.5, 0.5 };	
							vertex_attributes.push_back(2, vertex_indices, vertex_weights);

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}
						polyhedron->clockwise_edges.back() = new_first_edge;
		
						remove_faces[face] = true;
					}

					break;
				}
			}

			// Make it happen ...
			k3d::polyhedron::delete_components(Output, *polyhedron, remove_points, remove_edges, remove_loops, remove_faces);
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<subdivide_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xb9d4d815, 0x241c473d, 0xa22d3523, 0x406fa390),
				"SubdivideFaces",
				"Subdivides faces by creating new center / midpoints.",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		CENTER,
		CENTER_MIDPOINTS,
		MIDPOINTS,
	} subdivision_t;

	friend std::ostream& operator << (std::ostream& Stream, const subdivision_t& Value)
	{
		switch(Value)
		{
			case CENTER:
				Stream << "center";
				break;
			case CENTER_MIDPOINTS:
				Stream << "centermidpoints";
				break;
			case MIDPOINTS:
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
			Value = CENTER;
		else if(text == "centermidpoints")
			Value = CENTER_MIDPOINTS;
		else if(text == "midpoints")
			Value = MIDPOINTS;
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
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Center-Midpoints", "centermidpoints", "Joins face center to each edge middle; innovative info: produces quads only"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Midpoints", "midpoints", "Joins contiguous edge midpoints; amusing anecdote: produces one triangle per original vertex, plus a polygon in the middle with as many edges as the original"));
		}

		return values;
	}

private:
	k3d_data(subdivision_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_subdivision_type;
};

/////////////////////////////////////////////////////////////////////////////
// subdivide_faces_factory

k3d::iplugin_factory& subdivide_faces_factory()
{
	return subdivide_faces::get_factory();
}

} // namespace polyhedron

} // namespace module

