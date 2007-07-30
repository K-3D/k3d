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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "detail.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3darray
{

/////////////////////////////////////////////////////////////////////////////
// rotate_array_1d

class rotate_array_1d :
	public transform_array_1d<k3d::persistent<k3d::node> >
{
	typedef transform_array_1d<k3d::persistent<k3d::node> > base;

public:
	rotate_array_1d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_angle(init_owner(*this) + init_name("angle") + init_label(_("Angle")) + init_description(_("Angle")) + init_value(k3d::angle_axis(k3d::radians(360.0 / 6.0), k3d::point3(0, 0, 1))))
	{
		m_angle.changed_signal().connect(make_node_change_slot());
	}

	const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1)
	{
		const k3d::angle_axis angle = m_angle.pipeline_value();

		return k3d::rotation3D(k3d::angle_axis(angle.angle * Index1, angle.axis));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory< rotate_array_1d,
				k3d::interface_list<k3d::itransform_array_1d> > factory(
				k3d::uuid(0x321d43cf, 0x45da4300, 0x87fa6c4c, 0xec37e850),
				"RotateArray1D",
				_("RotateArray1D"),
				"Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::angle_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_angle;
};

k3d::iplugin_factory& rotate_array_1d_factory()
{
	return rotate_array_1d::get_factory();
}

} // namespace libk3darray

