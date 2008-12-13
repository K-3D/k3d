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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "obj_parser.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/mesh_reader.h>
#include <k3dsdk/node.h>

namespace module
{

namespace obj
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader

class mesh_reader :
	public k3d::mesh_reader<k3d::node >
{
	typedef k3d::mesh_reader<k3d::node > base;

public:
	mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_load_mesh(const k3d::filesystem::path& Path, k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::log() << info << "Loading .obj file: " << Path.native_console_string() << std::endl;
		k3d::filesystem::ifstream file(Path);
		if(!file)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << Path.native_console_string() << "]" << std::endl;
			return;
		}

		my_parser parser(Output);
		parser.parse(file);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader,
                k3d::interface_list<k3d::imesh_source,
                k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0x650f96b9, 0xe1c145d5, 0x9eda410d, 0x13f4e98f),
			"OBJMeshReader",
			_("Mesh reader that loads external Wavefront (.obj) files into the document by reference"),
			"MeshReader");

		return factory;
	}

private:
	/// Implementation of obj_parser that instantiates just points and faces to start ...
	class my_parser :
		public obj_parser
	{
	public:
		my_parser(k3d::mesh& Mesh) :
			mesh(Mesh),
			points(0),
			point_selection(0),
			point_weights(0),

			polyhedra(0),
			first_faces(0),
			face_counts(0),
			types(0),
			face_first_loops(0),
			face_loop_counts(0),
			face_selection(0),
			face_materials(0),
			loop_first_edges(0),
			edge_points(0),
			clockwise_edges(0),
			edge_selection(0),

			nurbs_patches(0),
			nurbs_patch_first_points(0),
			nurbs_patch_u_point_counts(0),
			nurbs_patch_v_point_counts(0),
			nurbs_patch_u_orders(0),
			nurbs_patch_v_orders(0),
			nurbs_patch_u_first_knots(0),
			nurbs_patch_v_first_knots(0),
			nurbs_patch_selection(0),
			nurbs_patch_materials(0),
			nurbs_patch_points(0),
			nurbs_patch_point_weights(0),
			nurbs_patch_u_knots(0),
			nurbs_patch_v_knots(0),

			u_order(0),
			v_order(0),
			s0(0),
			s1(0),
			t0(0),
			t1(0)
		{
		}

		~my_parser()
		{
			delete point_weights;
		}

	private:
		k3d::mesh& mesh;
		k3d::mesh::points_t* points;
		k3d::mesh::selection_t* point_selection;
		k3d::mesh::weights_t* point_weights; // Note: *not* part of the mesh!
	
		k3d::mesh::polyhedra_t* polyhedra;	
		k3d::mesh::indices_t* first_faces;
		k3d::mesh::counts_t* face_counts;
		k3d::mesh::polyhedra_t::types_t* types;
		k3d::mesh::indices_t* face_first_loops;
		k3d::mesh::counts_t* face_loop_counts;
		k3d::mesh::selection_t* face_selection;
		k3d::mesh::materials_t* face_materials;
		k3d::mesh::indices_t* loop_first_edges;
		k3d::mesh::indices_t* edge_points;
		k3d::mesh::indices_t* clockwise_edges;
		k3d::mesh::selection_t* edge_selection;

		k3d::mesh::nurbs_patches_t* nurbs_patches;
		k3d::mesh::indices_t* nurbs_patch_first_points;
		k3d::mesh::counts_t* nurbs_patch_u_point_counts;
		k3d::mesh::counts_t* nurbs_patch_v_point_counts;
		k3d::mesh::orders_t* nurbs_patch_u_orders;
		k3d::mesh::orders_t* nurbs_patch_v_orders;
		k3d::mesh::indices_t* nurbs_patch_u_first_knots;
		k3d::mesh::indices_t* nurbs_patch_v_first_knots;
		k3d::mesh::selection_t* nurbs_patch_selection;
		k3d::mesh::materials_t* nurbs_patch_materials;
		k3d::mesh::indices_t* nurbs_patch_points;
		k3d::mesh::weights_t* nurbs_patch_point_weights;
		k3d::mesh::knots_t* nurbs_patch_u_knots;
		k3d::mesh::knots_t* nurbs_patch_v_knots;

		k3d::string_t curve_surface_type;
		k3d::uint_t u_order;
		k3d::uint_t v_order;
		k3d::double_t s0;
		k3d::double_t s1;
		k3d::double_t t0;
		k3d::double_t t1;
		k3d::mesh::indices_t vertex_coordinates;
		k3d::mesh::indices_t texture_coordinates;
		k3d::mesh::indices_t normal_coordinates;
		k3d::mesh::knots_t u_knots;
		k3d::mesh::knots_t v_knots;

		void on_vertex_coordinates(const k3d::point4& Vertex)
		{
			if(!points)
				points = &mesh.points.create();

			if(!point_selection)
				point_selection = &mesh.point_selection.create();

			if(!point_weights)
				point_weights = new k3d::mesh::weights_t(); // Note: *not* part of the mesh!

			points->push_back(k3d::point3(Vertex[0], Vertex[1], Vertex[2]));
			point_selection->push_back(0.0);
			point_weights->push_back(Vertex[3]);
		}

		void on_face(const k3d::mesh::indices_t& Points, const k3d::mesh::indices_t& TexturePoints, const k3d::mesh::indices_t& Normals)
		{
			if(!polyhedra)
				polyhedra = &mesh.polyhedra.create();

			if(!first_faces)
			{
				first_faces = &polyhedra->first_faces.create();
				first_faces->push_back(0);
			}

			if(!face_counts)
			{
				face_counts = &polyhedra->face_counts.create();
				face_counts->push_back(0);
			}

			if(!types)
			{
				types = &polyhedra->types.create();
				types->push_back(k3d::mesh::polyhedra_t::POLYGONS);
			}

			if(!face_first_loops)
				face_first_loops = &polyhedra->face_first_loops.create();

			if(!face_loop_counts)
				face_loop_counts = &polyhedra->face_loop_counts.create();

			if(!face_selection)
				face_selection = &polyhedra->face_selection.create();

			if(!face_materials)
				face_materials = &polyhedra->face_materials.create();

			if(!loop_first_edges)
				loop_first_edges = &polyhedra->loop_first_edges.create();

			if(!edge_points)
				edge_points = &polyhedra->edge_points.create();

			if(!clockwise_edges)
				clockwise_edges = &polyhedra->clockwise_edges.create();

			if(!edge_selection)
				edge_selection = &polyhedra->edge_selection.create();

			face_first_loops->push_back(loop_first_edges->size());
			face_loop_counts->push_back(1);
			face_selection->push_back(0.0);
			face_materials->push_back(static_cast<k3d::imaterial*>(0));
			loop_first_edges->push_back(edge_points->size());

			const k3d::uint_t point_begin = 0;
			const k3d::uint_t point_end = point_begin + Points.size();
			const k3d::uint_t first_edge = edge_points->size();
			for(k3d::uint_t point = point_begin; point != point_end; ++point)
			{
				edge_points->push_back(Points[point]);
				clockwise_edges->push_back(edge_points->size());
				edge_selection->push_back(0.0);
			}
			clockwise_edges->back() = first_edge;

			face_counts->back() = face_counts->back() + 1;
		}

		void on_curve_surface_type(const k3d::string_t& Type)
		{
			curve_surface_type = Type;
		}

		void on_degree(const size_t& UDegree, const size_t& VDegree)
		{
			u_order = UDegree + 1;
			v_order = VDegree + 1;
		}

		void on_surface(const k3d::double_t& S0, const k3d::double_t& S1, const k3d::double_t& T0, const k3d::double_t& T1, const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates)
		{
			s0 = S0;
			s1 = S1;
			t0 = T0;
			t1 = T1;
			vertex_coordinates = VertexCoordinates;
			texture_coordinates = TextureCoordinates;
			normal_coordinates = NormalCoordinates;
		}

		void on_parameter(const k3d::string_t& Direction, const k3d::mesh::knots_t& Knots)
		{
			if(Direction == "u")
			{
				u_knots = Knots;
				return;
			}
			else if(Direction == "v")
			{
				v_knots = Knots;
				return;
			}

			throw std::runtime_error("unknown knot direction: " + Direction);
		}

		void on_curve_surface_end()
		{
			if(curve_surface_type != "bspline")
				throw std::runtime_error("unknown surface type [" + curve_surface_type + "] will be ignored");
			
			const k3d::uint_t expected_vertex_count = (u_knots.size() - u_order) * (v_knots.size() - v_order);
			if(expected_vertex_count != vertex_coordinates.size())
				throw std::runtime_error("invalid vertex count for bspline surface");

			if(!nurbs_patches)
				nurbs_patches = &mesh.nurbs_patches.create();

			if(!nurbs_patch_first_points)
				nurbs_patch_first_points = &nurbs_patches->patch_first_points.create();

			if(!nurbs_patch_u_point_counts)
				nurbs_patch_u_point_counts = &nurbs_patches->patch_u_point_counts.create();

			if(!nurbs_patch_v_point_counts)
				nurbs_patch_v_point_counts = &nurbs_patches->patch_v_point_counts.create();

			if(!nurbs_patch_u_orders)
				nurbs_patch_u_orders = &nurbs_patches->patch_u_orders.create();

			if(!nurbs_patch_v_orders)
				nurbs_patch_v_orders = &nurbs_patches->patch_v_orders.create();

			if(!nurbs_patch_u_first_knots)
				nurbs_patch_u_first_knots = &nurbs_patches->patch_u_first_knots.create();

			if(!nurbs_patch_v_first_knots)
				nurbs_patch_v_first_knots = &nurbs_patches->patch_v_first_knots.create();

			if(!nurbs_patch_selection)
				nurbs_patch_selection = &nurbs_patches->patch_selection.create();

			if(!nurbs_patch_materials)
				nurbs_patch_materials = &nurbs_patches->patch_materials.create();

			if(!nurbs_patch_points)
				nurbs_patch_points = &nurbs_patches->patch_points.create();

			if(!nurbs_patch_point_weights)
				nurbs_patch_point_weights = &nurbs_patches->patch_point_weights.create();

			if(!nurbs_patch_u_knots)
				nurbs_patch_u_knots = &nurbs_patches->patch_u_knots.create();

			if(!nurbs_patch_v_knots)
				nurbs_patch_v_knots = &nurbs_patches->patch_v_knots.create();

			nurbs_patch_first_points->push_back(nurbs_patch_points->size());
			nurbs_patch_u_point_counts->push_back(u_knots.size() - u_order);
			nurbs_patch_v_point_counts->push_back(v_knots.size() - v_order);
			nurbs_patch_u_orders->push_back(u_order);
			nurbs_patch_v_orders->push_back(v_order);
			nurbs_patch_u_first_knots->push_back(nurbs_patch_u_knots->size());
			nurbs_patch_v_first_knots->push_back(nurbs_patch_v_knots->size());
			nurbs_patch_selection->push_back(0);
			nurbs_patch_materials->push_back(static_cast<k3d::imaterial*>(0));
			nurbs_patch_points->insert(nurbs_patch_points->end(), vertex_coordinates.begin(), vertex_coordinates.end());

			for(k3d::uint_t i = 0; i != vertex_coordinates.size(); ++i)
				nurbs_patch_point_weights->push_back((*point_weights)[vertex_coordinates[i]]);

			nurbs_patch_u_knots->insert(nurbs_patch_u_knots->end(), u_knots.begin(), u_knots.end());
			nurbs_patch_v_knots->insert(nurbs_patch_v_knots->end(), v_knots.begin(), v_knots.end());

			curve_surface_type.clear();
			u_order = 0;
			v_order = 0;
			s0 = 0;
			s1 = 0;
			t0 = 0;
			t1 = 0;
			vertex_coordinates.clear();
			texture_coordinates.clear();
			normal_coordinates.clear();
			u_knots.clear();
			v_knots.clear();
		}
	};
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader::get_factory();
}

} // namespace io

} // namespace obj

} // namespace module

