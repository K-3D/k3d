// K-3D
// Copyright (c) 2004-2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/shared_pointer.h>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// ellipsoid

class ellipsoid :
	public k3d::material_sink<k3d::mesh_source<k3d::node> >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node> > base;

public:
	ellipsoid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X Position")) + init_description(_("Position on X axis")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y Position")) + init_description(_("Position on Y axis")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z(init_owner(*this) + init_name("z") + init_label(_("Z Position")) + init_description(_("Position on Z axis")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_size_x(init_owner(*this) + init_name("size_x") + init_label(_("X Size")) + init_description(_("Size on X axis")) + init_value(1.0) + init_step_increment(0.1) + init_constraint(constraint::minimum<double>(0.1)) + init_units(typeid(k3d::measurement::distance))),
		m_size_y(init_owner(*this) + init_name("size_y") + init_label(_("Y Size")) + init_description(_("Size on Y axis")) + init_value(1.0) + init_step_increment(0.1) + init_constraint(constraint::minimum<double>(0.1)) + init_units(typeid(k3d::measurement::distance))),
		m_size_z(init_owner(*this) + init_name("size_z") + init_label(_("Z Size")) + init_description(_("Size on Z axis")) + init_value(1.0) + init_step_increment(0.1) + init_constraint(constraint::minimum<double>(0.1)) + init_units(typeid(k3d::measurement::distance))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Ellipsoid color")) + init_value(k3d::color(1, 1, 1)))
	{
		m_material.changed_signal().connect(make_topology_changed_slot());
		m_x.changed_signal().connect(make_topology_changed_slot());
		m_y.changed_signal().connect(make_topology_changed_slot());
		m_z.changed_signal().connect(make_topology_changed_slot());
		m_size_x.changed_signal().connect(make_topology_changed_slot());
		m_size_y.changed_signal().connect(make_topology_changed_slot());
		m_size_z.changed_signal().connect(make_topology_changed_slot());
		m_color.changed_signal().connect(make_topology_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Output)
	{
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

		k3d::imaterial* const material = m_material.pipeline_value();
		const double x = m_x.pipeline_value();
		const double y = m_y.pipeline_value();
		const double z = m_z.pipeline_value();
		const double size_x = m_size_x.pipeline_value();
		const double size_y = m_size_y.pipeline_value();
		const double size_z = m_size_z.pipeline_value();
		const k3d::color color = m_color.pipeline_value();

		first_primitives.push_back(primitives.size());
		primitive_counts.push_back(1);
		first_operators.push_back(operators.size());
		operator_counts.push_back(0);
		materials.push_back(material);

		primitives.push_back(k3d::mesh::blobbies_t::ELLIPSOID);
		primitive_first_floats.push_back(floats.size());
		primitive_float_counts.push_back(16);

		k3d::matrix4 matrix = k3d::transpose(k3d::translation3D(k3d::point3(x, y, z)) * k3d::scaling3D(k3d::point3(size_x, size_y, size_z)));
		floats.insert(floats.end(), static_cast<double*>(matrix), static_cast<double*>(matrix) + 16);

		boost::shared_ptr<k3d::typed_array<k3d::color> > varying_colors(new k3d::typed_array<k3d::color>());
		varying_colors->push_back(color);
		blobbies.varying_data["Cs"] = varying_colors;
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<ellipsoid > factory(
			k3d::uuid(0x76ba2f7c, 0xd49945e2, 0xa54d32b0, 0x0f756a94),
			"BlobbyEllipsoid",
			"Creates a RenderMan Blobby Ellipsoid",
			"Blobby",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_size_x;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_size_y;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_size_z;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

k3d::iplugin_factory& ellipsoid_factory()
{
	return ellipsoid::get_factory();
}

} // namespace blobby

} // namespace module

