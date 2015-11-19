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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
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
// make_hole

class make_hole :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	make_hole(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_scaling(init_owner(*this) + init_name("scaling") + init_label(_("Scaling")) + init_description(_("Scaling factor by which to scale the hole relative to the original face")) + init_value(0.5) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_add_face(init_owner(*this) + init_name("add_face") + init_label(_("Add Face")) + init_description(_("Add a face, so the new hole is capped")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_scaling.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_add_face.changed_signal().connect(k3d::hint::converter<
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

		const k3d::double_t scaling = m_scaling.pipeline_value();
		const bool add_face = m_add_face.pipeline_value();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			const k3d::mesh::indices_t input_loop_first_edges = polyhedron->loop_first_edges;
			polyhedron->loop_first_edges.clear();

			// For each selected face ...
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				const k3d::uint_t first_loop = polyhedron->face_first_loops[face];
				const k3d::uint_t loop_count = polyhedron->face_loop_counts[face];
				const k3d::uint_t first_edge = input_loop_first_edges[first_loop];

				const k3d::mesh::indices_t::const_iterator face_loops_begin = input_loop_first_edges.begin()+first_loop;
				const k3d::mesh::indices_t::const_iterator face_loops_end = face_loops_begin + loop_count;
				polyhedron->face_first_loops[face] = polyhedron->loop_first_edges.size();
				polyhedron->loop_first_edges.insert(polyhedron->loop_first_edges.end(), face_loops_begin, face_loops_end);

				if(!polyhedron->face_selections[face])
					continue;

				// Deselect the face, making it easier to move the new hole around
				polyhedron->face_selections[face] = 0.;

				// Get ready to copy attributes ...
				k3d::table_copier edge_attributes(polyhedron->edge_attributes);
				k3d::table_copier vertex_attributes(polyhedron->vertex_attributes);
				k3d::table_copier face_attributes(polyhedron->face_attributes);

				// Add a new loop
				polyhedron->face_loop_counts[face]++;
				polyhedron->loop_first_edges.push_back(polyhedron->vertex_points.size());

				// Generate points and edges  for each corner of the hole, and calculate the face centroid position as scaling reference
				k3d::point3 centroid(0, 0, 0);
				const k3d::uint_t hole_begin = points.size();
				const k3d::uint_t first_new_edge = polyhedron->vertex_points.size();
				for(k3d::uint_t edge = first_edge; ; )
				{
					const k3d::uint_t old_point_idx = polyhedron->vertex_points[edge];
					const k3d::uint_t new_point_idx = points.size();
					const k3d::uint_t new_edge = polyhedron->vertex_points.size();
					// Add new edge
					polyhedron->vertex_points.push_back(new_point_idx);
					edge_attributes.push_back(edge);
					vertex_attributes.push_back(edge);
          polyhedron->edge_selections.push_back(1.);
          polyhedron->vertex_selections.push_back(1.);

					// clockwise edges
					polyhedron->clockwise_edges.push_back(new_edge-1);

					points.push_back(points[old_point_idx]);
					point_selection.push_back(1.);
					point_attributes.push_back(old_point_idx);
					centroid += k3d::to_vector(points.back());


					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
					{
						polyhedron->clockwise_edges[first_new_edge] = new_edge;
						break;
					}

				}
				const k3d::uint_t hole_end = points.size();

				// Adjust point positions by scaling around the center
				centroid /= static_cast<k3d::double_t>(hole_end-hole_begin);
				const k3d::vector3 translation_vec = k3d::to_vector(centroid);
				const k3d::matrix4 scaling_mat = k3d::scale3(scaling, scaling, scaling);
				for(k3d::uint_t point_idx = hole_begin; point_idx != hole_end; ++point_idx)
				{
					k3d::point3& p = points[point_idx];
					p = scaling_mat*(p - translation_vec) + translation_vec;
				}

				// Add a new face if requested
				if(add_face)
				{
					const k3d::uint_t new_face_idx = polyhedron->face_shells.size();
					const k3d::uint_t new_loop_idx = polyhedron->loop_first_edges.size();

					polyhedron->face_shells.push_back(polyhedron->face_shells[face]);
					polyhedron->face_first_loops.push_back(new_loop_idx);
					polyhedron->face_loop_counts.push_back(1);
					polyhedron->face_selections.push_back(1.);
					polyhedron->face_materials.push_back(polyhedron->face_materials[face]);
					face_attributes.push_back(face);
					for(k3d::uint_t point_idx = hole_begin; point_idx != hole_end; ++point_idx)
					{
						const k3d::uint_t new_edge_idx = polyhedron->vertex_points.size();

						if(point_idx == hole_begin)
						{
							polyhedron->loop_first_edges.push_back(new_edge_idx);
						}

						polyhedron->vertex_points.push_back(point_idx);
						polyhedron->clockwise_edges.push_back(new_edge_idx+1);

						polyhedron->edge_selections.push_back(1.);
						polyhedron->vertex_selections.push_back(1.);
						edge_attributes.push_back(first_edge + point_idx - hole_begin);
						vertex_attributes.push_back(first_edge + point_idx - hole_begin);
					}
					polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();
				}
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_hole,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9b86ef8b, 0xdc31444a, 0xbf56a318, 0xfe3e11d2),
				"MakeHole",
				"Creates a hole in each selected face, using the face outer loop as basis and scaling it by the specified factor.",
				"Polyhedron",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_scaling;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_add_face;
};

/////////////////////////////////////////////////////////////////////////////
// make_hole_factory

k3d::iplugin_factory& make_hole_factory()
{
	return make_hole::get_factory();
}

} // namespace polyhedron

} // namespace module
