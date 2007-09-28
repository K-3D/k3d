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
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
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

			m_points.reset(new k3d::mesh::points_t(*Input.points));
			m_point_selection.reset(new k3d::mesh::selection_t(*Input.point_selection));

			base::process(Input);

			m_first_faces->push_back(0);
			m_face_counts->push_back(m_face_first_loops->size());
			m_types->push_back(k3d::mesh::polyhedra_t::POLYGONS);

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

			Output.polyhedra = m_polyhedra;
			Output.points = m_points;
			Output.point_selection = m_point_selection;
		}

	private:
		void on_add_vertex(const k3d::point3& Coordinates, size_t Vertices[4], double Weights[4], size_t& NewVertex)
		{
			NewVertex = m_points->size();

			m_points->push_back(Coordinates);
			m_point_selection->push_back(0.0);
		}

		void on_add_triangle(const size_t Point1, const size_t Point2, const size_t Point3)
		{
			m_face_first_loops->push_back(m_loop_first_edges->size());
			m_face_loop_counts->push_back(1);
			m_face_selection->push_back(0.0);
			m_face_materials->push_back(0);
			m_loop_first_edges->push_back(m_edge_points->size());
			m_edge_points->push_back(Point1);
			m_edge_points->push_back(Point2);
			m_edge_points->push_back(Point3);
			m_clockwise_edges->push_back(m_edge_points->size() - 2);
			m_clockwise_edges->push_back(m_edge_points->size() - 1);
			m_clockwise_edges->push_back(m_edge_points->size() - 3);
			m_edge_selection->push_back(0.0);
			m_edge_selection->push_back(0.0);
			m_edge_selection->push_back(0.0);
		}

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

