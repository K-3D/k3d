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

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>

namespace module
{

namespace array
{

/////////////////////////////////////////////////////////////////////////////
// alternate_array_1d

class alternate_array_1d :
	public transform_array_1d
{
	typedef transform_array_1d base;

public:
	alternate_array_1d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_axis1(init_owner(*this) + init_name("axis1") + init_label(_("Axis1")) + init_description(_("Axis1")) + init_value(k3d::X) + init_enumeration(k3d::axis_values()))
	{
		m_axis1.changed_signal().connect(make_node_change_slot());
	}

	const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1)
	{
		const k3d::axis axis1 = m_axis1.pipeline_value();
		k3d::point3 vector1(k3d::X == axis1 ? -1 : 1, k3d::Y == axis1 ? -1 : 1, k3d::Z == axis1 ? -1 : 1);

		const k3d::matrix4 matrix1 = (Index1 % 2) ? k3d::scaling3D(vector1) : k3d::identity3D();

		return matrix1;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory< alternate_array_1d,
				k3d::interface_list<k3d::itransform_array_1d> > factory(
				k3d::uuid(0x172b9170, 0x91d2449b, 0xaa057e85, 0xafb2c01a),
				"AlternateArray1D",
				_("AlternateArray1D"),
				"Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_axis1;
};

k3d::iplugin_factory& alternate_array_1d_factory()
{
	return alternate_array_1d::get_factory();
}

} // namespace array

} // namespace module

