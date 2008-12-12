// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>
#include <iterator>

namespace module
{

namespace mesh_attributes
{

/////////////////////////////////////////////////////////////////////////////
// calculate_normals

class calculate_normals :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	calculate_normals(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_max_angle(init_owner(*this) + init_name("max_angle") + init_label(_("Maximum Angle")) + init_description(_("Normals will not be smoothed across points sharper than this angle (only applies to Face-Varying Normals).")) + init_value(k3d::radians(89.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_uniform(init_owner(*this) + init_name("uniform") + init_label(_("Uniform Normals")) + init_description(_("Generate uniform (per-face) normals.")) + init_value(false)),
		m_face_varying(init_owner(*this) + init_name("face_varying") + init_label(_("Face-Varying Normals")) + init_description(_("Generate face-varying (per-edge) normals.")) + init_value(true)),
		m_vertex(init_owner(*this) + init_name("vertex") + init_label(_("Vertex Normals")) + init_description(_("Generate vertex normals.")) + init_value(false)),
		m_uniform_array(init_owner(*this) + init_name("uniform_array") + init_label(_("Uniform Array Name")) + init_description(_("Uniform output array name.")) + init_value(std::string("N"))),
		m_face_varying_array(init_owner(*this) + init_name("face_varying_array") + init_label(_("Face-Varying Array Name")) + init_description(_("Face-varying output array name.")) + init_value(std::string("N"))),
		m_vertex_array(init_owner(*this) + init_name("vertex_array") + init_label(_("Vertex Array Name")) + init_description(_("Vertex output array name.")) + init_value(std::string("N")))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());

		m_max_angle.changed_signal().connect(make_update_mesh_slot());

		m_uniform.changed_signal().connect(make_update_mesh_slot());
		m_face_varying.changed_signal().connect(make_update_mesh_slot());
		m_vertex.changed_signal().connect(make_update_mesh_slot());
		
		m_uniform_array.changed_signal().connect(make_update_mesh_slot());
		m_face_varying_array.changed_signal().connect(make_update_mesh_slot());
		m_vertex_array.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!k3d::validate_polyhedra(Output))
			return;

		const k3d::mesh::points_t& points = *Output.points;

		k3d::mesh::polyhedra_t& polyhedra = Output.polyhedra.writable();
		const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *polyhedra.face_loop_counts;
		const k3d::mesh::selection_t& face_selection = *polyhedra.face_selection;
		const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;

		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = face_begin + face_first_loops.size();

		// Compute uniform (per-face) normals (used for all subsequent calculations) ...
		k3d::mesh::normals_t uniform_normals(face_first_loops.size());
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
			uniform_normals[face] = k3d::normalize(k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]));

		// Optionally store the uniform normals ...
		if(m_uniform.pipeline_value())
			polyhedra.uniform_data.create(m_uniform_array.pipeline_value(), new k3d::mesh::normals_t(uniform_normals));

		// Optionally compute face-varying (per-edge) normals ...
		if(m_face_varying.pipeline_value())
		{
			const double cos_max_angle = std::cos(std::max(0.0, m_max_angle.pipeline_value()));

			k3d::mesh::normals_t& face_varying_normals = polyhedra.face_varying_data.create(m_face_varying_array.pipeline_value(), new k3d::mesh::normals_t(edge_points.size()));

			k3d::mesh::indices_t point_first_faces;
			k3d::mesh::counts_t point_face_counts;
			k3d::mesh::indices_t point_faces;
			k3d::polyhedron::create_vertex_face_lookup(face_first_loops, face_loop_counts, loop_first_edges, edge_points, clockwise_edges, points, point_first_faces, point_face_counts, point_faces);

			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				const k3d::normal3 face_normal = uniform_normals[face];

				const k3d::uint_t loop_begin = face_first_loops[face];
				const k3d::uint_t loop_end = loop_begin + face_loop_counts[face];
				for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
				{
					const k3d::uint_t first_edge = loop_first_edges[loop];
					for(k3d::uint_t edge = first_edge; ;)
					{
						face_varying_normals[edge] += face_normal;

						if(face_selection[face])
						{
							const k3d::uint_t point_face_begin = point_first_faces[edge_points[edge]];
							const k3d::uint_t point_face_end = point_face_begin + point_face_counts[edge_points[edge]];
							for(k3d::uint_t point_face = point_face_begin; point_face != point_face_end; ++point_face)
							{
								const k3d::uint_t adjacent_face = point_faces[point_face];
								if(adjacent_face == face)
									continue;

								if(!face_selection[adjacent_face])
									continue;

								const k3d::normal3 adjacent_normal = uniform_normals[adjacent_face];

								const double cos_angle = adjacent_normal * face_normal;
								if(cos_angle < cos_max_angle)
									continue;

								face_varying_normals[edge] += adjacent_normal;
							}
						}

						edge = clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
				}
			}
		}

		// Optionally compute per-vertex normals as the sum of adjacent face normals ...
		if(m_vertex.pipeline_value())
		{
			k3d::mesh::normals_t& vertex_normals = Output.vertex_data.create(m_vertex_array.pipeline_value(), new k3d::mesh::normals_t(points.size()));
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				const k3d::uint_t loop_begin = face_first_loops[face];
				const k3d::uint_t loop_end = loop_begin + face_loop_counts[face];
				for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
				{
					const k3d::uint_t first_edge = loop_first_edges[loop];
					for(k3d::uint_t edge = first_edge; ;)
					{
						vertex_normals[edge_points[edge]] += uniform_normals[face];

						edge = clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<calculate_normals,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xa6d565ee, 0x6b4a065d, 0x2430ca88, 0xb0bd88a1),
				"CalculateNormals",
				_("Calculates a variety of polygon normals"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_max_angle;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_uniform;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_face_varying;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_vertex;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_uniform_array;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_face_varying_array;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_vertex_array;
};

/////////////////////////////////////////////////////////////////////////////
// calculate_normals_factory

k3d::iplugin_factory& calculate_normals_factory()
{
	return calculate_normals::get_factory();
}

} // namespace mesh_attributes

} // namespace module

