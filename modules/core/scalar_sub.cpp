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
		\brief An object that take two doubles as input and produce their difference as output
		\author Anders Dahnielson (anders@dahnielson.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dcore
{

class scalar_sub :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;
public:
	scalar_sub(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input1(init_owner(*this) + init_name("input1") + init_label(_("Input 1")) + init_description(_("First input float")) + init_value(0.0)),
		m_input2(init_owner(*this) + init_name("input2") + init_label(_("Input 2")) + init_description(_("Second input float")) + init_value(0.0)),
		m_output(init_owner(*this) + init_name("output") + init_label(_("Output")) + init_description(_("Difference of inputs")) + init_slot(sigc::mem_fun(*this, &scalar_sub::get_value)))
	{
		m_input1.changed_signal().connect(m_output.make_reset_slot());
		m_input2.changed_signal().connect(m_output.make_reset_slot());
	}

	double get_value()
	{
		return m_input1.value() - m_input2.value();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scalar_sub > factory(
			k3d::uuid(0x2b788bf5, 0x09e44dd4, 0x866a2d39, 0x1b782873),
			"ScalarSubtract",
			_("Subtract two scalar inputs and produce their difference as output"),
			"Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input2;
	k3d_data(double, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;

};

k3d::iplugin_factory& scalar_sub_factory()
{
	return scalar_sub::get_factory();
}

} //namespace libk3dcore


