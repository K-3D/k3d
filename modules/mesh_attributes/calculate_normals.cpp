// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/named_array_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/shared_pointer.h>

#include <iterator>

namespace module
{

namespace mesh_attributes
{

/////////////////////////////////////////////////////////////////////////////
// calculate_normals

class calculate_normals :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	calculate_normals(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_uniform(init_owner(*this) + init_name("uniform") + init_label(_("Uniform normals")) + init_description(_("Generate uniform (per-face) normals")) + init_value(false)),
		m_face_varying(init_owner(*this) + init_name("face_varying") + init_label(_("Face-varying normals")) + init_description(_("Generate face-varying (per-edge) normals")) + init_value(true)),
		m_vertex(init_owner(*this) + init_name("vertex") + init_label(_("Vertex normals")) + init_description(_("Generate vertex normals")) + init_value(false)),
		m_uniform_array(init_owner(*this) + init_name("uniform_array") + init_label(_("Uniform array name")) + init_description(_("Uniform output array name")) + init_value(std::string("N"))),
		m_face_varying_array(init_owner(*this) + init_name("face_varying_array") + init_label(_("Face-varying array name")) + init_description(_("Face-varying output array name")) + init_value(std::string("N"))),
		m_vertex_array(init_owner(*this) + init_name("vertex_array") + init_label(_("Vertex array name")) + init_description(_("Vertex output array name")) + init_value(std::string("N")))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());

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

		k3d::replace_selection(m_mesh_selection.pipeline_value(), Output);

		if(!k3d::validate_polyhedra(Output))
			return;

		const k3d::mesh::indices_t& face_first_loops = *Output.polyhedra->face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *Output.polyhedra->face_loop_counts;
		const k3d::mesh::selection_t& face_selection = *Output.polyhedra->face_selection;
		const k3d::mesh::indices_t& loop_first_edges = *Output.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Output.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Output.polyhedra->clockwise_edges;
		const k3d::mesh::points_t& points = *Output.points;

		const size_t face_begin = 0;
		const size_t face_end = face_begin + face_first_loops.size();

		// Compute per-face normals ...
		boost::shared_ptr<k3d::mesh::normals_t> uniform_normal(new k3d::mesh::normals_t(face_first_loops.size()));
		for(size_t face = face_begin; face != face_end; ++face)
			(*uniform_normal)[face] = k3d::normalize(k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]));

		// Compute per-vertex normals as the sum of adjacent face normals ...
		boost::shared_ptr<k3d::mesh::normals_t> vertex_normal(new k3d::mesh::normals_t(points.size()));
//		k3d::mesh::counts_t vertex_normal_count(points.size(), 0);
		for(size_t face = face_begin; face != face_end; ++face)
		{
			const size_t loop_begin = face_first_loops[face];
			const size_t loop_end = loop_begin + face_loop_counts[face];
			for(size_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const size_t first_edge = loop_first_edges[loop];
				for(size_t edge = first_edge; ;)
				{
					(*vertex_normal)[edge_points[edge]] += (*uniform_normal)[face];
//					vertex_normal_count[edge_points[edge]] += 1;

					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}

		// Assign per-vertex normals to each edge (i.e. as facevarying data) ...
		boost::shared_ptr<k3d::mesh::normals_t> face_varying_normal(new k3d::mesh::normals_t(edge_points.size()));
		const size_t loop_begin = 0;
		const size_t loop_end = loop_begin + loop_first_edges.size();
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = loop_first_edges[loop];
			for(size_t edge = first_edge; ;)
			{
				(*face_varying_normal)[edge] = (*vertex_normal)[edge_points[edge]];

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}

		// Store the calculated normals in the output mesh ...
		k3d::mesh::polyhedra_t& polyhedra = *k3d::make_unique(Output.polyhedra);

		if(m_uniform.pipeline_value())
			polyhedra.uniform_data[m_uniform_array.pipeline_value()] = uniform_normal;

		if(m_face_varying.pipeline_value())
			polyhedra.face_varying_data[m_face_varying_array.pipeline_value()] = face_varying_normal;

		if(m_vertex.pipeline_value())
			Output.vertex_data[m_vertex_array.pipeline_value()] = vertex_normal;
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

