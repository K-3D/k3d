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

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// segment

class segment :
	public k3d::material_sink<k3d::mesh_source<k3d::node> >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node> > base;

public:
	segment(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Segment radius")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_x1(init_owner(*this) + init_name("x1") + init_label(_("X1 Position")) + init_description(_("First segment end X position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y1(init_owner(*this) + init_name("y1") + init_label(_("Y1 Position")) + init_description(_("First segment end Y position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z1(init_owner(*this) + init_name("z1") + init_label(_("Z1 Position")) + init_description(_("First segment end Z position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_x2(init_owner(*this) + init_name("x2") + init_label(_("X2 Position")) + init_description(_("Second segment end X position")) + init_value(3.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y2(init_owner(*this) + init_name("y2") + init_label(_("Y2 Position")) + init_description(_("Second segment end Y position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z2(init_owner(*this) + init_name("z2") + init_label(_("Z2 Position")) + init_description(_("Second segment end Z position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Segment color")) + init_value(k3d::color(1, 1, 1)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_x1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_y1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_z1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_x2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_y2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_z2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_color.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::mesh::blobbies_t& blobbies = Output.blobbies.create();
		k3d::mesh::indices_t& first_primitives = blobbies.first_primitives.create();
		k3d::mesh::counts_t& primitive_counts = blobbies.primitive_counts.create();
		k3d::mesh::indices_t& first_operators = blobbies.first_operators.create();
		k3d::mesh::counts_t& operator_counts = blobbies.operator_counts.create();
		k3d::mesh::materials_t& materials = blobbies.materials.create();
		k3d::mesh::blobbies_t::primitives_t& primitives = blobbies.primitives.create();
		k3d::mesh::indices_t& primitive_first_floats = blobbies.primitive_first_floats.create();
		k3d::mesh::counts_t& primitive_float_counts = blobbies.primitive_float_counts.create();
		k3d::mesh::blobbies_t::operators_t& operators = blobbies.operators.create();
		k3d::mesh::indices_t& operator_first_operands = blobbies.operator_first_operands.create();
		k3d::mesh::counts_t& operator_operand_counts = blobbies.operator_operand_counts.create();
		k3d::mesh::blobbies_t::floats_t& floats = blobbies.floats.create();
		k3d::mesh::blobbies_t::operands_t& operands = blobbies.operands.create();

		k3d::imaterial* const material = m_material.pipeline_value();
		const double radius = m_radius.pipeline_value();
		const double x1 = m_x1.pipeline_value();
		const double y1 = m_y1.pipeline_value();
		const double z1 = m_z1.pipeline_value();
		const double x2 = m_x2.pipeline_value();
		const double y2 = m_y2.pipeline_value();
		const double z2 = m_z2.pipeline_value();
		const k3d::color color = m_color.pipeline_value();

		first_primitives.push_back(primitives.size());
		primitive_counts.push_back(1);
		first_operators.push_back(operators.size());
		operator_counts.push_back(0);
		materials.push_back(material);

		primitives.push_back(k3d::mesh::blobbies_t::SEGMENT);
		primitive_first_floats.push_back(floats.size());
		primitive_float_counts.push_back(23);

		floats.push_back(x1);
		floats.push_back(y1);
		floats.push_back(z1);
		floats.push_back(x2);
		floats.push_back(y2);
		floats.push_back(z2);
		floats.push_back(radius);

		k3d::matrix4 matrix = k3d::transpose(k3d::identity3D());
		floats.insert(floats.end(), static_cast<double*>(matrix), static_cast<double*>(matrix) + 16);

		k3d::typed_array<k3d::color>& varying_colors = blobbies.varying_data.create<k3d::typed_array<k3d::color> >("Cs");
		varying_colors.push_back(color);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<segment > factory(
			k3d::uuid(0x975d22ad, 0xe55e41a5, 0x9fdb26d2, 0x529952e0),
			"BlobbySegment",
			"Creates a blobby containing a single segment primitive",
			"Blobby",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z2;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

k3d::iplugin_factory& segment_factory()
{
	return segment::get_factory();
}

} // namespace blobby

} // namespace module

