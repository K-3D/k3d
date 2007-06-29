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
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// orientation

class orientation :
	public k3d::persistent<k3d::node>,
	public k3d::itransform_source,
	public k3d::itransform_sink
{
	typedef k3d::persistent<k3d::node> base;

public:
	orientation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input(init_owner(*this) + init_name("input_matrix") + init_label(_("Input matrix")) + init_description(_("Input matrix")) + init_value(k3d::identity3D())),
		m_space(init_owner(*this) + init_name("space") + init_label(_("Coordinate space")) + init_description(_("Coordinate space")) + init_value(k3d::identity3D())),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X")) + init_description(_("X rotation angle")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y")) + init_description(_("Y rotation angle")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_z(init_owner(*this) + init_name("z") + init_label(_("Z")) + init_description(_("Z rotation angle")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_output(init_owner(*this) + init_name("output_matrix") + init_label(_("Output matrix")) + init_description(_("Read only")) + init_slot(sigc::mem_fun(*this, &orientation::output_value)))
	{
		m_input.changed_signal().connect(m_output.make_reset_slot());
		m_space.changed_signal().connect(m_output.make_reset_slot());
		m_x.changed_signal().connect(m_output.make_reset_slot());
		m_y.changed_signal().connect(m_output.make_reset_slot());
		m_z.changed_signal().connect(m_output.make_reset_slot());
	}

	k3d::iproperty& transform_source_output()
	{
		return m_output;
	}

	k3d::iproperty& transform_sink_input()
	{
		return m_input;
	}

	k3d::matrix4 output_value()
	{
		return m_input.value() * m_space.value() * rotation3D(k3d::point3(m_x.value(), m_y.value(), m_z.value())) * k3d::inverse(m_space.value());
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<orientation,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::classes::Orientation(),
				"Orientation",
				_("Creates an orientation transform matrix"),
				"Transform",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_space;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;
};

/////////////////////////////////////////////////////////////////////////////
// orientation_factory

k3d::iplugin_factory& orientation_factory()
{
	return orientation::get_factory();
}

} // namespace libk3dcore

