// K-3D
// Copyright (c) 2007, Timothy M. Shead
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
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>

namespace libk3dprimitives
{

/////////////////////////////////////////////////////////////////////////////
// poly_icosahedron

class poly_icosahedron :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	poly_icosahedron(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
//		m_tessellation_level(init_owner(*this) + init_name("tessellation_level") + init_label(_("Tessellation level")) + init_description(_("Tessellation Level")) + init_value(3) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Sphere radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
//		m_tessellation_level.changed_signal().connect(make_topology_changed_slot());
		m_material.changed_signal().connect(make_topology_changed_slot());
		m_radius.changed_signal().connect(make_topology_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		k3d::imaterial* const material = m_material.pipeline_value();
		const double radius = m_radius.pipeline_value();

		// Create points ...
		const size_t point_count = 12;
		boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t(point_count));
		boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t(point_count, 0.0));

		for(size_t i = 0; i != point_count; ++i)
			(*points)[i] = radius * k3d::point3(vdata[i][0], vdata[i][1], vdata[i][2]);

		Mesh.points = points;
		Mesh.point_selection = point_selection;

		// Create triangles ...
		const size_t triangle_count = 20;
		boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra(new k3d::mesh::polyhedra_t());
		boost::shared_ptr<k3d::mesh::indices_t> first_faces(new k3d::mesh::indices_t());
		boost::shared_ptr<k3d::mesh::counts_t> face_counts(new k3d::mesh::counts_t());
		boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types(new k3d::mesh::polyhedra_t::types_t());
		boost::shared_ptr<k3d::mesh::indices_t> face_first_loops(new k3d::mesh::indices_t());
		boost::shared_ptr<k3d::mesh::counts_t> face_loop_counts(new k3d::mesh::counts_t());
		boost::shared_ptr<k3d::mesh::selection_t> face_selection(new k3d::mesh::selection_t());
		boost::shared_ptr<k3d::mesh::materials_t> face_materials(new k3d::mesh::materials_t());
		boost::shared_ptr<k3d::mesh::indices_t> loop_first_edges(new k3d::mesh::indices_t());
		boost::shared_ptr<k3d::mesh::indices_t> edge_points(new k3d::mesh::indices_t());
		boost::shared_ptr<k3d::mesh::indices_t> clockwise_edges(new k3d::mesh::indices_t());
		boost::shared_ptr<k3d::mesh::selection_t> edge_selection(new k3d::mesh::selection_t());

		first_faces->push_back(face_first_loops->size());
		face_counts->push_back(triangle_count);
		types->push_back(k3d::mesh::polyhedra_t::POLYGONS);

		face_first_loops->reserve(triangle_count);
		face_loop_counts->reserve(triangle_count);
		face_selection->reserve(triangle_count);
		face_materials->reserve(triangle_count);
		loop_first_edges->reserve(triangle_count);
		edge_points->reserve(3 * triangle_count);
		clockwise_edges->reserve(3 * triangle_count);
		edge_selection->reserve(3 * triangle_count);
		
		for(size_t i = 0; i != triangle_count; ++i)
		{
			face_first_loops->push_back(loop_first_edges->size());
			face_loop_counts->push_back(1);
			face_selection->push_back(0.0);
			face_materials->push_back(material);
			loop_first_edges->push_back(edge_points->size());
			
			for(size_t j = 0; j != 3; ++j)
			{
				edge_points->push_back(tindices[i][j]);
				clockwise_edges->push_back(edge_points->size());
				edge_selection->push_back(0.0);
			}

			if(loop_first_edges->size() && clockwise_edges->size())
				clockwise_edges->back() = loop_first_edges->back();
		}

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

		Mesh.polyhedra = polyhedra;
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_icosahedron, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x100dff7b, 0x94410b60, 0x4989f4a9, 0x75f6c1bf),
			"PolyIcosahedron",
			_("Generates a polygonal sphere by recursive subdivision of an isocahedron"),
			"Polygons",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
//	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_tessellation_level;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;

	static double vdata[12][3];
	static size_t tindices[20][3];
};

#define X .525731112119133606
#define Z .850650808352039932

double poly_icosahedron::vdata[12][3] = {
	{-X, 0, Z},
	{X, 0, Z},
	{-X, 0, -Z},
	{X, 0, -Z},
	{0, Z, X},
	{0, Z, -X},
	{0, -Z, X},
	{0, -Z, -X},
	{Z, X, 0},
	{-Z, X, 0},
	{Z, -X, 0},
	{-Z, -X, 0}};

size_t poly_icosahedron::tindices[20][3] = {
	{1, 4, 0},
	{4, 9, 0},
	{4, 5, 9},
	{8, 5, 4},
	{1, 8, 4},
	{1, 10, 8},
	{10, 3, 8},
	{8, 3, 5},
	{3, 2, 5},
	{3, 7, 2},
	{3, 10, 7},
	{10, 6, 7},
	{6, 11, 7},
	{6, 0, 11},
	{6, 1, 0},
	{10, 1, 6},
	{11, 0, 9},
	{2, 11, 9},
	{5, 2, 9},
	{11, 2, 7}};

/////////////////////////////////////////////////////////////////////////////
// poly_icosahedron_factory

k3d::iplugin_factory& poly_icosahedron_factory()
{
	return poly_icosahedron::get_factory();
}

} // namespace libk3dprimitives

