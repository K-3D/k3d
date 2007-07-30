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
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/transformable.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// position

class position :
	public k3d::transformable<k3d::persistent<k3d::node> >
{
	typedef k3d::transformable<k3d::persistent<k3d::node> > base;

public:
	position(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_space(init_owner(*this) + init_name("space") + init_label(_("Coordinate space")) + init_description(_("Coordinate space")) + init_value(k3d::identity3D())),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X")) + init_description(_("X offset")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y")) + init_description(_("Y offset")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z(init_owner(*this) + init_name("z") + init_label(_("Z")) + init_description(_("Z offset")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_space.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_x.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_y.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_z.changed_signal().connect(m_output_matrix.make_reset_slot());
	}

	k3d::matrix4 matrix()
	{
		k3d::matrix4 result = m_input_matrix.pipeline_value() * m_space.pipeline_value() * translation3D(k3d::vector3(m_x.pipeline_value(), m_y.pipeline_value(), m_z.pipeline_value())) * k3d::inverse(m_space.pipeline_value());
		return result;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<position,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::classes::Position(),
				"Position",
				_("Creates a position transform matrix"),
				"Transform",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_space;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z;
};

/////////////////////////////////////////////////////////////////////////////
// position_factory

k3d::iplugin_factory& position_factory()
{
	return position::get_factory();
}

} // namespace libk3dcore

