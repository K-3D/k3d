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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/shared_pointer.h>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// points_to_blobby

class points_to_blobby :
	public k3d::material_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	points_to_blobby(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Points radius")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!Input.points)
			return;

		const k3d::mesh::points_t& input_points = *Input.points;
		const k3d::mesh::attribute_arrays_t& input_vertex_data = Input.vertex_data;

		k3d::mesh::blobbies_t& output_blobbies = *k3d::make_unique(Output.blobbies);
		k3d::mesh::indices_t& output_first_primitives = *k3d::make_unique(output_blobbies.first_primitives);
		k3d::mesh::counts_t& output_primitive_counts = *k3d::make_unique(output_blobbies.primitive_counts);
		k3d::mesh::indices_t& output_first_operators = *k3d::make_unique(output_blobbies.first_operators);
		k3d::mesh::counts_t& output_operator_counts = *k3d::make_unique(output_blobbies.operator_counts);
		k3d::mesh::materials_t& output_materials = *k3d::make_unique(output_blobbies.materials);
		k3d::mesh::blobbies_t::primitives_t& output_primitives = *k3d::make_unique(output_blobbies.primitives);
		k3d::mesh::indices_t& output_primitive_first_floats = *k3d::make_unique(output_blobbies.primitive_first_floats);
		k3d::mesh::counts_t& output_primitive_float_counts = *k3d::make_unique(output_blobbies.primitive_float_counts);
		k3d::mesh::attribute_arrays_t& output_vertex_data = output_blobbies.vertex_data;
		k3d::mesh::blobbies_t::operators_t& output_operators = *k3d::make_unique(output_blobbies.operators);
		k3d::mesh::indices_t& output_operator_first_operands = *k3d::make_unique(output_blobbies.operator_first_operands);
		k3d::mesh::counts_t& output_operator_operand_counts = *k3d::make_unique(output_blobbies.operator_operand_counts);
		k3d::mesh::blobbies_t::floats_t& output_floats = *k3d::make_unique(output_blobbies.floats);
		k3d::mesh::blobbies_t::operands_t& output_operands = *k3d::make_unique(output_blobbies.operands);

		output_vertex_data = input_vertex_data.clone_types();
		k3d::attribute_array_copier output_vertex_data_copier(input_vertex_data, output_vertex_data);

		const double radius = m_radius.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		output_first_primitives.push_back(output_primitives.size());
		output_primitive_counts.push_back(input_points.size());
		output_first_operators.push_back(output_operators.size());
		output_operator_counts.push_back(1);
		output_materials.push_back(material);

		const size_t points_begin = 0;
		const size_t points_end = points_begin + input_points.size();
		for(size_t point = points_begin; point != points_end; ++point)
		{
			output_primitives.push_back(k3d::mesh::blobbies_t::ELLIPSOID);
			output_primitive_first_floats.push_back(output_floats.size());
			output_primitive_float_counts.push_back(16);
			output_vertex_data_copier.push_back(point);

			k3d::matrix4 matrix = k3d::transpose(k3d::translation3D(input_points[point]) * k3d::scaling3D(k3d::point3(radius, radius, radius)));
			output_floats.insert(output_floats.end(), static_cast<double*>(matrix), static_cast<double*>(matrix) + 16);
		}

		output_operators.push_back(k3d::mesh::blobbies_t::ADD);
		output_operator_first_operands.push_back(output_operands.size());
		output_operator_operand_counts.push_back(input_points.size() + 1);
		output_operands.push_back(input_points.size());
		for(size_t i = 0; i != input_points.size(); ++i)
			output_operands.push_back(i);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<points_to_blobby,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9d5d69d9, 0xfe994aa0, 0x9b7dee22, 0x1823bd2c),
				"PointsToBlobby",
				"Converts input points to an implicit surface composed of blobby ellipsoids",
				"Blobby",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
};

/////////////////////////////////////////////////////////////////////////////
// points_to_blobby_factory

k3d::iplugin_factory& points_to_blobby_factory()
{
	return points_to_blobby::get_factory();
}

} // namespace blobby

} // namespace module

