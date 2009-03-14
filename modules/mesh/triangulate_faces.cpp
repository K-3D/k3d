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
#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/triangulator.h>

#include <boost/scoped_ptr.hpp>

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
			"Mesh",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	class create_triangles :
		public k3d::triangulator
	{
		typedef k3d::triangulator base;

	public:
		void process(const k3d::mesh& Input, const k3d::mesh_selection& Selection, k3d::mesh& Output)
		{
			input_polyhedron.reset(k3d::polyhedron::validate(Input));
			if(!input_polyhedron)
				return;

			// Allocate new data structures for our output ...
			output_points = &Output.points.create(new k3d::mesh::points_t(*Input.points));
			output_point_selection = &Output.point_selection.create(new k3d::mesh::selection_t(Input.points->size(), 0.0));
			output_polyhedron.reset(k3d::polyhedron::create(Output));

			// Setup copying of attribute arrays ...
			output_polyhedron->constant_data = input_polyhedron->constant_data;

			output_polyhedron->uniform_data = input_polyhedron->uniform_data.clone_types();
			uniform_data_copier.reset(new k3d::attribute_array_copier(input_polyhedron->uniform_data, output_polyhedron->uniform_data));

			output_polyhedron->face_varying_data = input_polyhedron->face_varying_data.clone_types();
			face_varying_data_copier.reset(new k3d::attribute_array_copier(input_polyhedron->face_varying_data, output_polyhedron->face_varying_data));

			Output.vertex_data = Input.vertex_data.clone();
			vertex_data_copier.reset(new k3d::attribute_array_copier(Input.vertex_data, Output.vertex_data));

			// Setup the output mesh ...
			k3d::mesh::selection_t input_face_selection(input_polyhedron->face_selections);
			k3d::mesh_selection::merge(Selection.faces, input_face_selection);

			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + input_polyhedron->face_first_loops.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(input_face_selection[face])
				{
					base::process(
						*Input.points,
						input_polyhedron->face_first_loops,
						input_polyhedron->face_loop_counts,
						input_polyhedron->loop_first_edges,
						input_polyhedron->edge_points,
						input_polyhedron->clockwise_edges,
						face);
				}
				else
				{
					add_existing_face(face);
				}
			}

			output_polyhedron->first_faces.push_back(0);
			output_polyhedron->face_counts.push_back(output_polyhedron->face_first_loops.size());
			output_polyhedron->polyhedron_types.push_back(k3d::mesh::polyhedra_t::POLYGONS);
		}

	private:
		void start_face(const k3d::uint_t Face)
		{
			current_face = Face;
		}

		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], double Weights[4], k3d::uint_t& NewVertex)
		{
			NewVertex = output_points->size();

			output_points->push_back(Coordinates);
			output_point_selection->push_back(0.0);

			vertex_data_copier->push_back(4, Vertices, Weights);

			new_face_varying_data[NewVertex] = new_face_varying_record(Edges, Weights);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			output_polyhedron->face_first_loops.push_back(output_polyhedron->loop_first_edges.size());
			output_polyhedron->face_loop_counts.push_back(1);
			output_polyhedron->face_selections.push_back(1.0);
			output_polyhedron->face_materials.push_back(input_polyhedron->face_materials[current_face]);

			uniform_data_copier->push_back(current_face);

			output_polyhedron->loop_first_edges.push_back(output_polyhedron->edge_points.size());
			output_polyhedron->edge_points.push_back(Vertices[0]);
			output_polyhedron->edge_points.push_back(Vertices[1]);
			output_polyhedron->edge_points.push_back(Vertices[2]);
			output_polyhedron->clockwise_edges.push_back(output_polyhedron->edge_points.size() - 2);
			output_polyhedron->clockwise_edges.push_back(output_polyhedron->edge_points.size() - 1);
			output_polyhedron->clockwise_edges.push_back(output_polyhedron->edge_points.size() - 3);
			output_polyhedron->edge_selections.push_back(0.0);
			output_polyhedron->edge_selections.push_back(0.0);
			output_polyhedron->edge_selections.push_back(0.0);

			for(k3d::uint_t i = 0; i != 3; ++i)
			{
				if(new_face_varying_data.count(Vertices[i]))
					face_varying_data_copier->push_back(4, new_face_varying_data[Vertices[i]].edges, new_face_varying_data[Vertices[i]].weights);
				else
					face_varying_data_copier->push_back(Edges[i]);
			}
		}

		void add_existing_face(const k3d::uint_t Face)
		{
			output_polyhedron->face_first_loops.push_back(output_polyhedron->loop_first_edges.size());
			output_polyhedron->face_loop_counts.push_back(input_polyhedron->face_loop_counts[Face]);
			output_polyhedron->face_selections.push_back(0.0);
			output_polyhedron->face_materials.push_back(input_polyhedron->face_materials[Face]);

			uniform_data_copier->push_back(Face);

			const k3d::uint_t loop_begin = input_polyhedron->face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + input_polyhedron->face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				output_polyhedron->loop_first_edges.push_back(output_polyhedron->edge_points.size());

				const k3d::uint_t first_edge = input_polyhedron->loop_first_edges[loop];
				const k3d::uint_t edge_offset = output_polyhedron->edge_points.size() - first_edge;
				for(k3d::uint_t edge = first_edge; ;)
				{
					output_polyhedron->edge_points.push_back(input_polyhedron->edge_points[edge]);
					output_polyhedron->clockwise_edges.push_back(input_polyhedron->clockwise_edges[edge] + edge_offset);
					output_polyhedron->edge_selections.push_back(0.0);
					face_varying_data_copier->push_back(edge);

					edge = input_polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}

		boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron;

		k3d::mesh::points_t* output_points;
		k3d::mesh::selection_t* output_point_selection;
		boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron;

		boost::shared_ptr<k3d::attribute_array_copier> uniform_data_copier;
		boost::shared_ptr<k3d::attribute_array_copier> face_varying_data_copier;
		boost::shared_ptr<k3d::attribute_array_copier> vertex_data_copier;

		k3d::uint_t current_face;

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

		std::map<k3d::uint_t, new_face_varying_record> new_face_varying_data;
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

