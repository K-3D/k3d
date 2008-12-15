// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include "detail.h"

#include <k3dsdk/document_plugin_factory.h>

namespace module
{

namespace array
{

/////////////////////////////////////////////////////////////////////////////
// rotate_array_3d

class rotate_array_3d :
	public transform_array_3d
{
	typedef transform_array_3d base;

public:
	rotate_array_3d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_angle1(init_owner(*this) + init_name("angle1") + init_label(_("Angle 1")) + init_description(_("Angle 1")) + init_value(k3d::angle_axis(k3d::radians(10.0), k3d::point3(0, 1, 0)))),
		m_angle2(init_owner(*this) + init_name("angle2") + init_label(_("Angle 2")) + init_description(_("Angle 2")) + init_value(k3d::angle_axis(k3d::radians(10.0), k3d::point3(0, 0, 1)))),
		m_angle3(init_owner(*this) + init_name("angle3") + init_label(_("Angle 3")) + init_description(_("Angle 3")) + init_value(k3d::angle_axis(k3d::radians(10.0), k3d::point3(1, 0, 0))))
	{
		m_angle1.changed_signal().connect(make_node_change_slot());
		m_angle2.changed_signal().connect(make_node_change_slot());
		m_angle3.changed_signal().connect(make_node_change_slot());
	}

	const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1, unsigned long Index2, unsigned long Count2, unsigned long Index3, unsigned long Count3)
	{
		const k3d::angle_axis angle1 = m_angle1.pipeline_value();
		const k3d::angle_axis angle2 = m_angle2.pipeline_value();
		const k3d::angle_axis angle3 = m_angle3.pipeline_value();

		return k3d::rotate3(k3d::angle_axis(angle1.angle * Index1, angle1.axis)) *
			k3d::rotate3(k3d::angle_axis(angle2.angle * Index2, angle2.axis)) *
			k3d::rotate3(k3d::angle_axis(angle3.angle * Index3, angle3.axis));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory< rotate_array_3d,
				k3d::interface_list<k3d::itransform_array_3d> > factory(
				k3d::uuid(0xc103b1c3, 0x909b45fa, 0xb59de8c4, 0x0dadfd51),
				"RotateArray3D",
				_("RotateArray3D"),
				"Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::angle_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_angle1;
	k3d_data(k3d::angle_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_angle2;
	k3d_data(k3d::angle_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_angle3;
};

k3d::iplugin_factory& rotate_array_3d_factory()
{
	return rotate_array_3d::get_factory();
}

} // namespace array

} // namespace module

