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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/named_array_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/triangulator.h>

namespace module
{

namespace mesh
{

class triangulate_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	triangulate_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}
	
	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		create_triangles().process(Input, m_mesh_selection.pipeline_value(), Output);
	}
	
	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<triangulate_faces, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
			k3d::uuid(0x871ccafd, 0xc944da92, 0xcdf9f2b4, 0xcbc40cd8),
			"TriangulateFaces",
			_("Converts input faces into triangles"),
			"Development",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	class create_triangles :
		public k3d::triangulator
	{
		typedef k3d::triangulator base;

	public:
		create_triangles() :
			m_input(0)
		{
		}

		void process(const k3d::mesh& Input, const k3d::mesh_selection& Selection, k3d::mesh& Output)
		{
			if(!k3d::validate_polyhedra(Input))
				return;

			m_input = &Input;

			// Allocate new data structures for our output ...
			m_polyhedra.reset(new k3d::mesh::polyhedra_t());
			m_first_faces.reset(new k3d::mesh::indices_t());
			m_face_counts.reset(new k3d::mesh::counts_t());
			m_types.reset(new k3d::mesh::polyhedra_t::types_t());
			m_face_first_loops.reset(new k3d::mesh::indices_t());
			m_face_loop_counts.reset(new k3d::mesh::counts_t());
			m_face_selection.reset(new k3d::mesh::selection_t());
			m_face_materials.reset(new k3d::mesh::materials_t());
			m_loop_first_edges.reset(new k3d::mesh::indices_t());
			m_edge_points.reset(new k3d::mesh::indices_t());
			m_clockwise_edges.reset(new k3d::mesh::indices_t());
			m_edge_selection.reset(new k3d::mesh::selection_t());
			m_points.reset(new k3d::mesh::points_t(*m_input->points));
			m_point_selection.reset(new k3d::mesh::selection_t(m_input->point_selection->size(), 0.0));

			// Hook everything together ...
			m_polyhedra->first_faces = m_first_faces;
			m_polyhedra->face_counts = m_face_counts;
			m_polyhedra->types = m_types;
			m_polyhedra->face_first_loops = m_face_first_loops;
			m_polyhedra->face_loop_counts = m_face_loop_counts;
			m_polyhedra->face_selection = m_face_selection;
			m_polyhedra->face_materials = m_face_materials;
			m_polyhedra->loop_first_edges = m_loop_first_edges;
			m_polyhedra->edge_points = m_edge_points;
			m_polyhedra->clockwise_edges = m_clockwise_edges;
			m_polyhedra->edge_selection = m_edge_selection;

			// Setup copying of attribute arrays ...
			m_polyhedra->constant_data = m_input->polyhedra->constant_data;

			m_polyhedra->uniform_data = m_input->polyhedra->uniform_data.clone_types();
			m_uniform_data_copier.reset(new k3d::named_array_copier(m_input->polyhedra->uniform_data, m_polyhedra->uniform_data));

			m_polyhedra->face_varying_data = m_input->polyhedra->face_varying_data.clone_types();
			m_face_varying_data_copier.reset(new k3d::named_array_copier(m_input->polyhedra->face_varying_data, m_polyhedra->face_varying_data));

			Output.vertex_data = m_input->vertex_data.clone();
			m_vertex_data_copier.reset(new k3d::named_array_copier(m_input->vertex_data, Output.vertex_data));

			// Setup the output mesh ...
			Output.polyhedra = m_polyhedra;
			Output.points = m_points;
			Output.point_selection = m_point_selection;

			k3d::mesh::selection_t input_face_selection(*m_input->polyhedra->face_selection);
			k3d::merge_selection(Selection.faces, input_face_selection);

			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + m_input->polyhedra->face_first_loops->size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(input_face_selection[face])
				{
					base::process(
						*m_input->points,
						*m_input->polyhedra->face_first_loops,
						*m_input->polyhedra->face_loop_counts,
						*m_input->polyhedra->loop_first_edges,
						*m_input->polyhedra->edge_points,
						*m_input->polyhedra->clockwise_edges,
						face);
				}
				else
				{
					add_existing_face(face);
				}
			}

			m_first_faces->push_back(0);
			m_face_counts->push_back(m_face_first_loops->size());
			m_types->push_back(k3d::mesh::polyhedra_t::POLYGONS);
		}

	private:
		void start_face(const k3d::uint_t Face)
		{
			m_current_face = Face;
		}

		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], double Weights[4], k3d::uint_t& NewVertex)
		{
			NewVertex = m_points->size();

			m_points->push_back(Coordinates);
			m_point_selection->push_back(0.0);

			m_vertex_data_copier->push_back(4, Vertices, Weights);

			m_new_face_varying_data[NewVertex] = new_face_varying_record(Edges, Weights);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			m_face_first_loops->push_back(m_loop_first_edges->size());
			m_face_loop_counts->push_back(1);
			m_face_selection->push_back(1.0);
			m_face_materials->push_back((*m_input->polyhedra->face_materials)[m_current_face]);

			m_uniform_data_copier->push_back(m_current_face);

			m_loop_first_edges->push_back(m_edge_points->size());
			m_edge_points->push_back(Vertices[0]);
			m_edge_points->push_back(Vertices[1]);
			m_edge_points->push_back(Vertices[2]);
			m_clockwise_edges->push_back(m_edge_points->size() - 2);
			m_clockwise_edges->push_back(m_edge_points->size() - 1);
			m_clockwise_edges->push_back(m_edge_points->size() - 3);
			m_edge_selection->push_back(0.0);
			m_edge_selection->push_back(0.0);
			m_edge_selection->push_back(0.0);

			for(k3d::uint_t i = 0; i != 3; ++i)
			{
				if(m_new_face_varying_data.count(Vertices[i]))
					m_face_varying_data_copier->push_back(4, m_new_face_varying_data[Vertices[i]].edges, m_new_face_varying_data[Vertices[i]].weights);
				else
					m_face_varying_data_copier->push_back(Edges[i]);
			}
		}

		void add_existing_face(const k3d::uint_t Face)
		{
			const k3d::mesh::indices_t& input_face_first_loops = *m_input->polyhedra->face_first_loops;
			const k3d::mesh::indices_t& input_face_loop_counts = *m_input->polyhedra->face_loop_counts;
			const k3d::mesh::materials_t& input_face_materials = *m_input->polyhedra->face_materials;

			const k3d::mesh::indices_t& input_loop_first_edges = *m_input->polyhedra->loop_first_edges;
			
			const k3d::mesh::indices_t& input_edge_points = *m_input->polyhedra->edge_points;
			const k3d::mesh::indices_t& input_clockwise_edges = *m_input->polyhedra->clockwise_edges;

			m_face_first_loops->push_back(m_loop_first_edges->size());
			m_face_loop_counts->push_back(input_face_loop_counts[Face]);
			m_face_selection->push_back(0.0);
			m_face_materials->push_back(input_face_materials[Face]);

			m_uniform_data_copier->push_back(Face);

			const k3d::uint_t loop_begin = input_face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + input_face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				m_loop_first_edges->push_back(m_edge_points->size());

				const k3d::uint_t first_edge = input_loop_first_edges[loop];
				const k3d::uint_t edge_offset = m_edge_points->size() - first_edge;
				for(k3d::uint_t edge = first_edge; ;)
				{
					m_edge_points->push_back(input_edge_points[edge]);
					m_clockwise_edges->push_back(input_clockwise_edges[edge] + edge_offset);
					m_edge_selection->push_back(0.0);
					m_face_varying_data_copier->push_back(edge);

					edge = input_clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}

		const k3d::mesh* m_input;

		boost::shared_ptr<k3d::mesh::polyhedra_t> m_polyhedra;
		boost::shared_ptr<k3d::mesh::indices_t> m_first_faces;
		boost::shared_ptr<k3d::mesh::counts_t> m_face_counts;
		boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> m_types;
		boost::shared_ptr<k3d::mesh::indices_t> m_face_first_loops;
		boost::shared_ptr<k3d::mesh::counts_t> m_face_loop_counts;
		boost::shared_ptr<k3d::mesh::selection_t> m_face_selection;
		boost::shared_ptr<k3d::mesh::materials_t> m_face_materials;
		boost::shared_ptr<k3d::mesh::indices_t> m_loop_first_edges;
		boost::shared_ptr<k3d::mesh::indices_t> m_edge_points;
		boost::shared_ptr<k3d::mesh::indices_t> m_clockwise_edges;
		boost::shared_ptr<k3d::mesh::selection_t> m_edge_selection;
		boost::shared_ptr<k3d::mesh::points_t> m_points;
		boost::shared_ptr<k3d::mesh::selection_t> m_point_selection;

		boost::shared_ptr<k3d::named_array_copier> m_uniform_data_copier;
		boost::shared_ptr<k3d::named_array_copier> m_face_varying_data_copier;
		boost::shared_ptr<k3d::named_array_copier> m_vertex_data_copier;

		k3d::uint_t m_current_face;

		struct new_face_varying_record
		{
			new_face_varying_record()
			{
			}

			new_face_varying_record(k3d::uint_t Edges[4], k3d::double_t Weights[4])
			{
				std::copy(Edges, Edges + 4, edges);
				std::copy(Weights, Weights + 4, weights);
			}

			k3d::uint_t edges[4];
			k3d::double_t weights[4];
		};

		std::map<k3d::uint_t, new_face_varying_record> m_new_face_varying_data;
	};
};

/////////////////////////////////////////////////////////////////////////////
// triangulate_faces_factory

k3d::iplugin_factory& triangulate_faces_factory()
{
	return triangulate_faces::get_factory();
}

} // namespace mesh

} // namespace module

