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
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/shared_pointer.h>

namespace libk3dblobbies
{

/////////////////////////////////////////////////////////////////////////////
// points_to_blobby_implementation

class points_to_blobby_implementation :
	public k3d::material_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	points_to_blobby_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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

		const k3d::mesh::points_t& points = *Input.points;

		k3d::mesh::blobbies_t& blobbies = *k3d::make_unique(Output.blobbies);
		k3d::mesh::indices_t& first_primitives = *k3d::make_unique(blobbies.first_primitives);
		k3d::mesh::counts_t& primitive_counts = *k3d::make_unique(blobbies.primitive_counts);
		k3d::mesh::indices_t& first_operators = *k3d::make_unique(blobbies.first_operators);
		k3d::mesh::counts_t& operator_counts = *k3d::make_unique(blobbies.operator_counts);
		k3d::mesh::materials_t& materials = *k3d::make_unique(blobbies.materials);
		k3d::mesh::blobbies_t::primitives_t& primitives = *k3d::make_unique(blobbies.primitives);
		k3d::mesh::indices_t& primitive_first_floats = *k3d::make_unique(blobbies.primitive_first_floats);
		k3d::mesh::counts_t& primitive_float_counts = *k3d::make_unique(blobbies.primitive_float_counts);
		k3d::mesh::blobbies_t::operators_t& operators = *k3d::make_unique(blobbies.operators);
		k3d::mesh::indices_t& operator_first_operands = *k3d::make_unique(blobbies.operator_first_operands);
		k3d::mesh::counts_t& operator_operand_counts = *k3d::make_unique(blobbies.operator_operand_counts);
		k3d::mesh::blobbies_t::floats_t& floats = *k3d::make_unique(blobbies.floats);
		k3d::mesh::blobbies_t::operands_t& operands = *k3d::make_unique(blobbies.operands);

		const double radius = m_radius.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		const k3d::matrix4 matrix = k3d::scaling3D(k3d::point3(radius, radius, radius));

		first_primitives.push_back(0);
		primitive_counts.push_back(points.size());
		first_operators.push_back(0);
		operator_counts.push_back(1);
		materials.push_back(material);

		const size_t points_begin = 0;
		const size_t points_end = points_begin + points.size();
		for(size_t point = points_begin; point != points_end; ++point)
		{
			primitives.push_back(k3d::mesh::blobbies_t::ELLIPSOID);
			primitive_first_floats.push_back(floats.size());
			primitive_float_counts.push_back(16);

			k3d::matrix4 matrix = k3d::transpose(k3d::translation3D(points[point]) * k3d::scaling3D(k3d::point3(radius, radius, radius)));
			floats.insert(floats.end(), static_cast<double*>(matrix), static_cast<double*>(matrix) + 16);
		}

		operators.push_back(k3d::mesh::blobbies_t::ADD);
		operator_first_operands.push_back(operands.size());
		operator_operand_counts.push_back(points.size() + 1);
		operands.push_back(points.size());
		for(size_t i = 0; i != points.size(); ++i)
			operands.push_back(i);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<points_to_blobby_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9d5d69d9, 0xfe994aa0, 0x9b7dee22, 0x1823bd2c),
				"PointsToBlobby",
				"Converts input points to an implicit surface composed of blobby ellipsoids",
				"Blobbies",
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
	return points_to_blobby_implementation::get_factory();
}

} // namespace libk3dblobbies

