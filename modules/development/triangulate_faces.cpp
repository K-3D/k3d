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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <k3d-i18n-config.h>

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/triangulator.h>

namespace module
{

namespace development
{

class triangulate_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	triangulate_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}
	
	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		create_triangles().process(Input, Output);
	}
	
	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<triangulate_faces, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
			k3d::uuid(0x871ccafd, 0xc944da92, 0xcdf9f2b4, 0xcbc40cd8),
			"NewTriangulateFaces",
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
		void process(const k3d::mesh& Input, k3d::mesh& Output)
		{
			if(!k3d::validate_polyhedra(Input))
				return;

			input = &Input;
			output = &Output;
			writable_points = 0;
			writable_point_selection = 0;
/*
			polyhedra.reset(new k3d::mesh::polyhedra_t());
			first_faces.reset(new k3d::mesh::indices_t());
			face_counts.reset(new k3d::mesh::counts_t());
			types.reset(new k3d::mesh::polyhedra_t::types_t());
			face_first_loops.reset(new k3d::mesh::indices_t());
			face_loop_counts.reset(new k3d::mesh::counts_t());
			face_selection.reset(new k3d::mesh::selection_t());
			face_materials.reset(new k3d::mesh::materials_t());
			loop_first_edges.reset(new k3d::mesh::indices_t());
			edge_points.reset(new k3d::mesh::indices_t());
			clockwise_edges.reset(new k3d::mesh::indices_t());
			edge_selection.reset(new k3d::mesh::selection_t());
*/

			Output.points = Input.points;
			Output.point_selection = Input.point_selection;
			Output.vertex_data = Input.vertex_data;

			base::process(Input);

/*
			first_faces->push_back(0);
			face_counts->push_back(face_first_loops->size());
			types->push_back(k3d::mesh::polyhedra_t::POLYGONS);

			polyhedra->first_faces = first_faces;
			polyhedra->face_counts = face_counts;
			polyhedra->types = types;
			polyhedra->face_first_loops = face_first_loops;
			polyhedra->face_loop_counts = face_loop_counts;
			polyhedra->face_selection = face_selection;
			polyhedra->face_materials = face_materials;
			polyhedra->loop_first_edges = loop_first_edges;
			polyhedra->edge_points = edge_points;
			polyhedra->clockwise_edges = clockwise_edges;
			polyhedra->edge_selection = edge_selection;

			Output.polyhedra = polyhedra;
*/
			input = 0;
			output = 0;
			writable_points = 0;
			writable_point_selection = 0;
		}

	private:
		void on_add_vertex(const k3d::point3& Coordinates, size_t Vertices[4], double Weights[4], size_t& NewVertex)
		{
			if(!writable_points)
				writable_points = k3d::make_unique(output->points);

			if(!writable_point_selection)
				writable_point_selection = k3d::make_unique(output->point_selection);

			NewVertex = writable_points->size();

			writable_points->push_back(Coordinates);
			writable_point_selection->push_back(0.0);
		}

		void on_add_triangle(const size_t Point1, const size_t Point2, const size_t Point3)
		{
		}

		const k3d::mesh* input;
		k3d::mesh* output;

/*
		boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra;
		boost::shared_ptr<k3d::mesh::indices_t> first_faces;
		boost::shared_ptr<k3d::mesh::counts_t> face_counts;
		boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types;
		boost::shared_ptr<k3d::mesh::indices_t> face_first_loops;
		boost::shared_ptr<k3d::mesh::counts_t> face_loop_counts;
		boost::shared_ptr<k3d::mesh::selection_t> face_selection;
		boost::shared_ptr<k3d::mesh::materials_t> face_materials;
		boost::shared_ptr<k3d::mesh::indices_t> loop_first_edges;
		boost::shared_ptr<k3d::mesh::indices_t> edge_points;
		boost::shared_ptr<k3d::mesh::indices_t> clockwise_edges;
		boost::shared_ptr<k3d::mesh::selection_t> edge_selection;
*/

		k3d::mesh::points_t* writable_points;
		k3d::mesh::selection_t* writable_point_selection;
	};
};

/////////////////////////////////////////////////////////////////////////////
// triangulate_faces_factory

k3d::iplugin_factory& triangulate_faces_factory()
{
	return triangulate_faces::get_factory();
}

} // namespace development

} // namespace module

