// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\author Bart Janssens (bart.janssens@polytechnic.be)
*/
#include <k3dsdk/document_plugin_factory.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>

#include <iterator>

namespace libk3dprimitives
{

/////////////////////////////////////////////////////////////////////////////
// poly_cube_implementation

class poly_cube_implementation :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	poly_cube_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number (X axis)")) + init_value(1) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number (Y axis)")) + init_value(1) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_slices(init_owner(*this) + init_name("slices") + init_label(_("Slices")) + init_description(_("Slice number (Z axis)")) + init_value(1) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Cube width")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Cube height")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_depth(init_owner(*this) + init_name("depth") + init_label(_("Depth")) + init_description(_("Cube depth")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_rows.changed_signal().connect(make_reset_mesh_slot());
		m_columns.changed_signal().connect(make_reset_mesh_slot());
		m_slices.changed_signal().connect(make_reset_mesh_slot());
		m_width.changed_signal().connect(make_reset_mesh_slot());
		m_height.changed_signal().connect(make_reset_mesh_slot());
		m_depth.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		const unsigned long rows = m_rows.pipeline_value();
		const unsigned long cols = m_columns.pipeline_value();
		const unsigned long slices = m_slices.pipeline_value();
		const double width = m_width.pipeline_value();
		const double height = m_height.pipeline_value();
		const double depth = m_depth.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		// First plane
		unsigned long point_rows = rows + 1;
		unsigned long point_columns = cols + 1;

		// Create points ...
		boost::multi_array<k3d::legacy::point*, 2> points(boost::extents[point_rows][point_columns]);
		for(unsigned long column = 0; column <= cols; ++column)
			for(unsigned long row = 0; row <= rows; ++row)
			{
				double x = width * (static_cast<double>(column) / static_cast<double>(cols) - 0.5);
				double y = height * (static_cast<double>(row) / static_cast<double>(rows) - 0.5);
				double z = depth / 2;

				points[row][column] = new k3d::legacy::point(x, y, z);
				Mesh.points.push_back(points[row][column]);
			}

		// Create edges ...
		boost::multi_array<k3d::legacy::split_edge*, 3> edges(boost::extents[rows][cols][4]);
		for(unsigned long row = 0; row != rows; ++row)
			for(unsigned long column = 0; column != cols; ++column)
			{
				edges[row][column][0] = new k3d::legacy::split_edge(points[row][column]);
				edges[row][column][1] = new k3d::legacy::split_edge(points[row][(column+1) % point_columns]);
				edges[row][column][2] = new k3d::legacy::split_edge(points[(row+1) % point_rows][(column+1) % point_columns]);
				edges[row][column][3] = new k3d::legacy::split_edge(points[(row+1) % point_rows][column]);

				// Add face
				k3d::legacy::face* const new_face = new k3d::legacy::face(edges[row][column][0], material);
				polyhedron.faces.push_back(new_face);

				for(unsigned long i = 0; i < 4; ++i)
					edges[row][column][i]->face_clockwise = edges[row][column][(i+1)%4];
			}

		// Join edges ...
		const unsigned long edge_rows = rows - 1;
		const unsigned long edge_columns = cols - 1;
		for(unsigned long row = 0; row != edge_rows; ++row)
			for(unsigned long column = 0; column != cols; ++column)
				k3d::legacy::join_edges(*edges[row][column][2], *edges[(row+1) % rows][column][0]);

		for(unsigned long column = 0; column != edge_columns; ++column)
			for(unsigned long row = 0; row != rows; ++row)
				k3d::legacy::join_edges(*edges[row][column][1], *edges[row][(column+1) % cols][3]);

		std::vector<k3d::legacy::split_edge*> boundary_edges;
		// Top row boundary edges
		for(unsigned long column = 0; column != cols; ++column)
			boundary_edges.push_back(edges[0][column][0]);

		// Right row
		for(unsigned long row = 0; row != rows; ++row)
			boundary_edges.push_back(edges[row][cols-1][1]);

		// Bottom row
		for(unsigned long column = 0; column != cols; ++column)
			boundary_edges.push_back(edges[rows-1][cols-column-1][2]);

		// Left row
		for(unsigned long row = 0; row != rows; ++row)
			boundary_edges.push_back(edges[rows-row-1][0][3]);

		for(unsigned long slice = 1; slice <= slices; ++slice)
		{
			std::vector<k3d::legacy::split_edge*> up_edges;
			std::vector<k3d::legacy::split_edge*> down_edges;
			std::vector<k3d::legacy::split_edge*> companion_edges;
			std::vector<k3d::legacy::split_edge*> new_boundary_edges;

			for(unsigned long i = 0; i < boundary_edges.size(); ++i)
			{
				k3d::point3 offset(0, 0, -depth/slices);
				k3d::legacy::point* new_point = new k3d::legacy::point(boundary_edges[i]->vertex->position + offset);
				Mesh.points.push_back(new_point);

				// New boundary edge:
				k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(new_point);
				new_boundary_edges.push_back(new_edge);

				// "down" edge:
				k3d::legacy::split_edge* up_edge = new k3d::legacy::split_edge(boundary_edges[i]->vertex);
				up_edges.push_back(up_edge);

				// "up" edge:
				k3d::legacy::split_edge* down_edge = new k3d::legacy::split_edge(new_point);
				down_edges.push_back(down_edge);

				// Join up- and down edges:
				k3d::legacy::join_edges(*up_edge, *down_edge);

				// New companion edge:
				k3d::legacy::split_edge* companion_edge = new k3d::legacy::split_edge(boundary_edges[i]->vertex);
				companion_edges.push_back(companion_edge);

				// Add face:
				k3d::legacy::face* const new_face = new k3d::legacy::face(companion_edge, material);
				polyhedron.faces.push_back(new_face);
			}

			// Connect first face
			companion_edges[0]->face_clockwise = up_edges[boundary_edges.size()-1];
			up_edges[0]->face_clockwise = new_boundary_edges[0];
			new_boundary_edges[boundary_edges.size()-1]->face_clockwise = down_edges[0];
			down_edges[0]->face_clockwise = companion_edges[0];
			k3d::legacy::join_edges(*companion_edges[0], *boundary_edges[boundary_edges.size()-1]);

			for(unsigned long i = 1; i < boundary_edges.size(); ++i)
			{
				// Set the clockwise edges
				companion_edges[i]->face_clockwise = up_edges[i-1];
				up_edges[i]->face_clockwise = new_boundary_edges[i];
				new_boundary_edges[i-1]->face_clockwise = down_edges[i];
				down_edges[i]->face_clockwise = companion_edges[i];

				// Join with the original edge
				k3d::legacy::join_edges(*companion_edges[i], *boundary_edges[i-1]);
			}

			boundary_edges = new_boundary_edges;
		}

		// Last plane
		// Create points ...
		boost::multi_array<k3d::legacy::point*, 2> last_points(boost::extents[point_rows][point_columns]);
		for(unsigned long column = 1; column <= cols-1; ++column)
			for(unsigned long row = 1; row <= rows-1; ++row)
			{
				double x = width * (static_cast<double>(column) / static_cast<double>(cols) - 0.5);
				double y = height * (static_cast<double>(row) / static_cast<double>(rows) - 0.5);
				double z = - depth / 2;

				last_points[row][column] = new k3d::legacy::point(x, y, z);
				Mesh.points.push_back(last_points[row][column]);
			}

		// Top row boundary points
		//last_points[0][0] = boundary_edges[2*cols+2*rows-1]->vertex;
		for(unsigned long column = 0; column != cols; ++column)
			last_points[0][column] = boundary_edges[column]->vertex;

		// Right row
		for(unsigned long row = 0; row != rows; ++row)
			last_points[row][cols] = boundary_edges[cols+row]->vertex;

		// Bottom row
		for(unsigned long column = 0; column != cols; ++column)
			last_points[rows][cols-column] = boundary_edges[rows+cols+column]->vertex;

		// Left row
		for(unsigned long row = 0; row != rows; ++row)
			last_points[rows-row][0] = boundary_edges[2*cols+rows+row]->vertex;

		// Create edges ...
		boost::multi_array<k3d::legacy::split_edge*, 3> last_edges(boost::extents[rows][cols][4]);
		for(unsigned long row = 0; row != rows; ++row)
			for(unsigned long column = 0; column != cols; ++column)
			{
				last_edges[row][column][3] = new k3d::legacy::split_edge(last_points[row][column]);
				last_edges[row][column][0] = new k3d::legacy::split_edge(last_points[row][(column+1) % point_columns]);
				last_edges[row][column][1] = new k3d::legacy::split_edge(last_points[(row+1) % point_rows][(column+1) % point_columns]);
				last_edges[row][column][2] = new k3d::legacy::split_edge(last_points[(row+1) % point_rows][column]);

				for(unsigned long i = 0; i < 4; ++i)
					last_edges[row][column][(i+1)%4]->face_clockwise = last_edges[row][column][i];

				// Add face
				k3d::legacy::face* const new_face = new k3d::legacy::face(last_edges[row][column][0], material);
				polyhedron.faces.push_back(new_face);
			}

		// Join edges ...
		for(unsigned long row = 0; row != edge_rows; ++row)
			for(unsigned long column = 0; column != cols; ++column)
				k3d::legacy::join_edges(*last_edges[row][column][2], *last_edges[(row+1) % rows][column][0]);

		for(unsigned long column = 0; column != edge_columns; ++column)
			for(unsigned long row = 0; row != rows; ++row)
				k3d::legacy::join_edges(*last_edges[row][column][1], *last_edges[row][(column+1) % cols][3]);

		// Join the last boundary edges, obtained from the extrusion process, to the back plane
		// Top row boundary edges
		for(unsigned long column = 0; column != cols; ++column)
			k3d::legacy::join_edges(*last_edges[0][column][0], *boundary_edges[column]);

		// Right row
		for(unsigned long row = 0; row != rows; ++row)
			k3d::legacy::join_edges(*last_edges[row][cols-1][1], *boundary_edges[cols+row]);

		// Bottom row
		for(unsigned long column = 0; column != cols; ++column)
			k3d::legacy::join_edges(*last_edges[rows-1][cols-column-1][2], *boundary_edges[rows+cols+column]);

		// Left row
		for(unsigned long row = 0; row != rows; ++row)
			k3d::legacy::join_edges(*last_edges[rows-row-1][0][3], *boundary_edges[2*cols+rows+row]);

		assert_warning(is_valid(polyhedron));
		assert_warning(is_solid(polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_cube_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x7ec966d1, 0xd78a445c, 0x8806b8a3, 0x5593f241),
			"PolyCube",
			_("Generates a polygonal cube"),
			"Polygons",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_columns;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_rows;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_slices;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_depth;
};

/////////////////////////////////////////////////////////////////////////////
// poly_cube_factory

k3d::iplugin_factory& poly_cube_factory()
{
	return poly_cube_implementation::get_factory();
}

} // namespace libk3dprimitives


