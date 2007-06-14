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
		\brief Implements the RenderMan Blobby primitives and operators
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dblobbies
{

/////////////////////////////////////////////////////////////////////////////
// blobby_ellipsoid

class blobby_ellipsoid :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_ellipsoid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X Position")) + init_description(_("Position on X axis")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y Position")) + init_description(_("Position on Y axis")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z(init_owner(*this) + init_name("z") + init_label(_("Z Position")) + init_description(_("Position on Z axis")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_size_x(init_owner(*this) + init_name("size_x") + init_label(_("X Size")) + init_description(_("Size on X axis")) + init_value(1.0) + init_step_increment(0.1) + init_constraint(constraint::minimum(0.1)) + init_units(typeid(k3d::measurement::distance))),
		m_size_y(init_owner(*this) + init_name("size_y") + init_label(_("Y Size")) + init_description(_("Size on Y axis")) + init_value(1.0) + init_step_increment(0.1) + init_constraint(constraint::minimum(0.1)) + init_units(typeid(k3d::measurement::distance))),
		m_size_z(init_owner(*this) + init_name("size_z") + init_label(_("Z Size")) + init_description(_("Size on Z axis")) + init_value(1.0) + init_step_increment(0.1) + init_constraint(constraint::minimum(0.1)) + init_units(typeid(k3d::measurement::distance))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Ellipsoid color")) + init_value(k3d::color(1, 1, 1)))
	{
		m_x.changed_signal().connect(make_reset_mesh_slot());
		m_y.changed_signal().connect(make_reset_mesh_slot());
		m_z.changed_signal().connect(make_reset_mesh_slot());
		m_size_x.changed_signal().connect(make_reset_mesh_slot());
		m_size_y.changed_signal().connect(make_reset_mesh_slot());
		m_size_z.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::point* position = new k3d::legacy::point(m_x.value(), m_y.value(), m_z.value());
		Mesh.points.push_back(position);

		k3d::matrix4 transformation = k3d::scaling3D(k3d::point3(m_size_x.value(), m_size_y.value(), m_size_z.value()));
		k3d::legacy::blobby::ellipsoid* ellipsoid = new k3d::legacy::blobby::ellipsoid(position, transformation);
		ellipsoid->vertex_data["Cs"] = m_color.value();

		Mesh.blobbies.push_back(new k3d::legacy::blobby(ellipsoid));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_ellipsoid > factory(
			k3d::uuid(0x76ba2f7c, 0xd49945e2, 0xa54d32b0, 0x0f756a94),
			"BlobbyEllipsoid",
			"Creates a RenderMan Blobby Ellipsoid",
			"Blobbies",
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

/////////////////////////////////////////////////////////////////////////////
// blobby_segment

class blobby_segment :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_segment(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		m_x1.changed_signal().connect(make_reset_mesh_slot());
		m_y1.changed_signal().connect(make_reset_mesh_slot());
		m_z1.changed_signal().connect(make_reset_mesh_slot());
		m_x2.changed_signal().connect(make_reset_mesh_slot());
		m_y2.changed_signal().connect(make_reset_mesh_slot());
		m_z2.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::point* start = new k3d::legacy::point(m_x1.value(), m_y1.value(), m_z1.value());
		k3d::legacy::point* end = new k3d::legacy::point(m_x2.value(), m_y2.value(), m_z2.value());
		Mesh.points.push_back(start);
		Mesh.points.push_back(end);

		k3d::matrix4 id = k3d::identity3D();
		k3d::legacy::blobby::segment* segment = new k3d::legacy::blobby::segment(start, end, m_radius.value(), id);
		segment->vertex_data["Cs"] = m_color.value();

		Mesh.blobbies.push_back(new k3d::legacy::blobby(segment));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_segment> factory(
			k3d::uuid(0x975d22ad, 0xe55e41a5, 0x9fdb26d2, 0x529952e0),
			"BlobbySegment",
			"Creates a RenderMan Blobby Segment",
			"Blobbies",
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

/////////////////////////////////////////////////////////////////////////////
// blobby_add_operator

class blobby_add_operator :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_add_operator(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Input mesh 1")) + init_description(_("First addition operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0)),
		m_input_mesh2(init_owner(*this) + init_name("input_mesh2") + init_label(_("Input mesh 2")) + init_description(_("Second addition operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0))
	{
		m_input_mesh1.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh2.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input_mesh1 = m_input_mesh1.value();
		const k3d::legacy::mesh* const input_mesh2 = m_input_mesh2.value();

		// Create output geometry ...
		if(input_mesh1)
			k3d::legacy::deep_copy(*input_mesh1, Mesh);

		if(input_mesh2)
			k3d::legacy::deep_copy(*input_mesh2, Mesh);

		k3d::legacy::blobby::add* addition = new k3d::legacy::blobby::add();
		for(k3d::legacy::mesh::blobbies_t::const_iterator blob = Mesh.blobbies.begin(); blob != Mesh.blobbies.end(); blob++)
			addition->add_operand((*blob)->root);

		Mesh.blobbies.clear();
		Mesh.blobbies.push_back(new k3d::legacy::blobby(addition));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_add_operator> factory(
			k3d::uuid(0x580c8ebb, 0x300d49dc, 0x9c43cbf6, 0x2808872a),
			"BlobbyAddOperator",
			"Blobby Add operator",
			"Blobbies",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh2;
};

/////////////////////////////////////////////////////////////////////////////
// blobby_mult_operator

class blobby_mult_operator :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_mult_operator(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Input mesh 1")) + init_description(_("First multiplication operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0)),
		m_input_mesh2(init_owner(*this) + init_name("input_mesh2") + init_label(_("Input mesh 2")) + init_description(_("Second multiplication operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0))
	{
		m_input_mesh1.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh2.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input_mesh1 = m_input_mesh1.value();
		const k3d::legacy::mesh* const input_mesh2 = m_input_mesh2.value();

		// Create output geometry ...
		if(input_mesh1)
			k3d::legacy::deep_copy(*input_mesh1, Mesh);

		if(input_mesh2)
			k3d::legacy::deep_copy(*input_mesh2, Mesh);

		k3d::legacy::blobby::multiply* multiplication = new k3d::legacy::blobby::multiply();
		for(k3d::legacy::mesh::blobbies_t::const_iterator blob = Mesh.blobbies.begin(); blob != Mesh.blobbies.end(); blob++)
			multiplication->add_operand((*blob)->root);

		Mesh.blobbies.clear();
		Mesh.blobbies.push_back(new k3d::legacy::blobby(multiplication));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_mult_operator> factory(
			k3d::uuid(0x51777220, 0xbf7346fc, 0xbf66e8bf, 0x349853b9),
			"BlobbyMultOperator",
			"Blobby Mult operator",
			"Blobbies",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh2;
};

/////////////////////////////////////////////////////////////////////////////
// blobby_max_operator

class blobby_max_operator :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_max_operator(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Input mesh 1")) + init_description(_("First maximum operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0)),
		m_input_mesh2(init_owner(*this) + init_name("input_mesh2") + init_label(_("Input mesh 2")) + init_description(_("Second maximum operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0))
	{
		m_input_mesh1.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh2.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input_mesh1 = m_input_mesh1.value();
		const k3d::legacy::mesh* const input_mesh2 = m_input_mesh2.value();

		// Create output geometry ...
		if(input_mesh1)
			k3d::legacy::deep_copy(*input_mesh1, Mesh);

		if(input_mesh2)
			k3d::legacy::deep_copy(*input_mesh2, Mesh);

		k3d::legacy::blobby::max* maximum = new k3d::legacy::blobby::max();
		for(k3d::legacy::mesh::blobbies_t::const_iterator blob = Mesh.blobbies.begin(); blob != Mesh.blobbies.end(); blob++)
			maximum->add_operand((*blob)->root);

		Mesh.blobbies.clear();
		Mesh.blobbies.push_back(new k3d::legacy::blobby(maximum));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_max_operator> factory(
			k3d::uuid(0x4a8e2958, 0x58d546aa, 0xad7bfb37, 0x5c67450b),
			"BlobbyMaxOperator",
			"Blobby Max operator",
			"Blobbies",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh2;
};

/////////////////////////////////////////////////////////////////////////////
// blobby_min_operator

class blobby_min_operator :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_min_operator(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Input mesh 1")) + init_description(_("First minimum operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0)),
		m_input_mesh2(init_owner(*this) + init_name("input_mesh2") + init_label(_("Input mesh 2")) + init_description(_("Second minimum operand (should be a blobby)")) + init_value<k3d::legacy::mesh*>(0))
	{
		m_input_mesh1.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh2.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input_mesh1 = m_input_mesh1.value();
		const k3d::legacy::mesh* const input_mesh2 = m_input_mesh2.value();

		// Create output geometry ...
		if(input_mesh1)
			k3d::legacy::deep_copy(*input_mesh1, Mesh);

		if(input_mesh2)
			k3d::legacy::deep_copy(*input_mesh2, Mesh);

		k3d::legacy::blobby::min* minimum = new k3d::legacy::blobby::min();
		for(k3d::legacy::mesh::blobbies_t::const_iterator blob = Mesh.blobbies.begin(); blob != Mesh.blobbies.end(); blob++)
			minimum->add_operand((*blob)->root);

		Mesh.blobbies.clear();
		Mesh.blobbies.push_back(new k3d::legacy::blobby(minimum));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_min_operator> factory(
			k3d::uuid(0xc8e5eabc, 0x86f44610, 0x8e2a647d, 0x847de77a),
			"BlobbyMinOperator",
			"Blobby Min operator",
			"Blobbies",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh2;
};

/////////////////////////////////////////////////////////////////////////////
// blobby_sub_operator

class blobby_sub_operator :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_sub_operator(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Input mesh 1")) + init_description(_("Blobby subtrahend")) + init_value<k3d::legacy::mesh*>(0)),
		m_input_mesh2(init_owner(*this) + init_name("input_mesh2") + init_label(_("Input mesh 2")) + init_description(_("Blobby minuend")) + init_value<k3d::legacy::mesh*>(0)),
		m_type(init_owner(*this) + init_name("subtraction_type") + init_label(_("Subtraction type")) + init_description(_("Swap subtrahend and minuend")) + init_enumeration(subtract_values()) + init_value(SUBTRACTION))
	{
		m_input_mesh1.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh2.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input_mesh1 = m_input_mesh1.value();
		const k3d::legacy::mesh* const input_mesh2 = m_input_mesh2.value();

		// Create output geometry ...
		if(input_mesh1)
			k3d::legacy::deep_copy(*input_mesh1, Mesh);

		if(input_mesh2)
			k3d::legacy::deep_copy(*input_mesh2, Mesh);

		if(Mesh.blobbies.size() >= 2)
			{
				k3d::legacy::mesh::blobbies_t::const_iterator blobby = Mesh.blobbies.begin();
				k3d::legacy::blobby::opcode* opcode1 = (*blobby++)->root;
				k3d::legacy::blobby::opcode* opcode2 = (*blobby++)->root;
				k3d::legacy::blobby::subtract* subtraction = new k3d::legacy::blobby::subtract(opcode1, opcode2);

				Mesh.blobbies.clear();
				Mesh.blobbies.push_back(new k3d::legacy::blobby(subtraction));
			}
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_sub_operator> factory(
			k3d::uuid(0x351e67c1, 0x12034c4c, 0x974516db, 0xbcb767d9),
			"BlobbySubOperator",
			"Blobby Sub operator",
			"Blobbies",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		SUBTRACTION,
		REVERSE_SUBTRACTION
	} subtract_t;

	static const k3d::ienumeration_property::enumeration_values_t& subtract_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
			{
				values.push_back(k3d::ienumeration_property::enumeration_value_t("Subtraction", "subtraction", "Subtract blobbies"));
				values.push_back(k3d::ienumeration_property::enumeration_value_t("Reverse Subtraction", "reverse_subtraction", "Subtract blobbies"));
			}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const subtract_t& Value)
	{
		switch(Value)
			{
				case SUBTRACTION:
					Stream << "subtraction";
					break;
				case REVERSE_SUBTRACTION:
					Stream << "reverse_subtraction";
					break;
			}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, subtract_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "subtraction")
			Value = SUBTRACTION;
		else if(text == "reverse_subtraction")
			Value = REVERSE_SUBTRACTION;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh2;
	k3d_data(subtract_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
};

/////////////////////////////////////////////////////////////////////////////
// blobby_div_operator

class blobby_div_operator :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	blobby_div_operator(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Input mesh 1")) + init_description(_("Blobby dividend")) + init_value<k3d::legacy::mesh*>(0)),
		m_input_mesh2(init_owner(*this) + init_name("input_mesh2") + init_label(_("Input mesh 2")) + init_description(_("Blobby divisor")) + init_value<k3d::legacy::mesh*>(0)),
		m_type(init_owner(*this) + init_name("division_type") + init_label(_("Division type")) + init_description(_("Swap dividend and divisor")) + init_enumeration(divide_values()) + init_value(DIVISION))
	{
		m_input_mesh1.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh2.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input_mesh1 = m_input_mesh1.value();
		const k3d::legacy::mesh* const input_mesh2 = m_input_mesh2.value();

		// Create output geometry ...
		if(input_mesh1)
			k3d::legacy::deep_copy(*input_mesh1, Mesh);

		if(input_mesh2)
			k3d::legacy::deep_copy(*input_mesh2, Mesh);

		if(Mesh.blobbies.size() >= 2)
			{
				k3d::legacy::mesh::blobbies_t::const_iterator blobby = Mesh.blobbies.begin();
				k3d::legacy::blobby::opcode* opcode1 = (*blobby++)->root;
				k3d::legacy::blobby::opcode* opcode2 = (*blobby++)->root;
				k3d::legacy::blobby::divide* division = new k3d::legacy::blobby::divide(opcode1, opcode2);

				Mesh.blobbies.clear();
				Mesh.blobbies.push_back(new k3d::legacy::blobby(division));
			}
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_div_operator> factory(
			k3d::uuid(0xf650ce52, 0x2fc64539, 0xa184ef26, 0x03bbe6d7),
			"BlobbyDivOperator",
			"Blobby Div operator",
			"Blobbies",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		DIVISION,
		REVERSE_DIVISION
	} divide_t;

	static const k3d::ienumeration_property::enumeration_values_t& divide_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
			{
				values.push_back(k3d::ienumeration_property::enumeration_value_t("Division", "division", "Divide blobbies"));
				values.push_back(k3d::ienumeration_property::enumeration_value_t("Reverse Division", "reverse_division", "Divide blobbies"));
			}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const divide_t& Value)
	{
		switch(Value)
			{
				case DIVISION:
					Stream << "division";
					break;
				case REVERSE_DIVISION:
					Stream << "reverse_division";
					break;
			}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, divide_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "division")
			Value = DIVISION;
		else if(text == "reverse_division")
			Value = REVERSE_DIVISION;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh2;
	k3d_data(divide_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
};

/////////////////////////////////////////////////////////////////////////////
// blobby factories

k3d::iplugin_factory& blobby_ellipsoid_factory()
{
	return blobby_ellipsoid::get_factory();
}

k3d::iplugin_factory& blobby_segment_factory()
{
	return blobby_segment::get_factory();
}

k3d::iplugin_factory& blobby_add_operator_factory()
{
	return blobby_add_operator::get_factory();
}

k3d::iplugin_factory& blobby_mult_operator_factory()
{
	return blobby_mult_operator::get_factory();
}

k3d::iplugin_factory& blobby_max_operator_factory()
{
	return blobby_max_operator::get_factory();
}

k3d::iplugin_factory& blobby_min_operator_factory()
{
	return blobby_min_operator::get_factory();
}

k3d::iplugin_factory& blobby_sub_operator_factory()
{
	return blobby_sub_operator::get_factory();
}

k3d::iplugin_factory& blobby_div_operator_factory()
{
	return blobby_div_operator::get_factory();
}

} // namespace libk3dblobbies


