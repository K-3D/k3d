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
		\brief An object that take two doubles as input and produce their quotient as output
		\author Anders Dahnielson (anders@dahnielson.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dcore
{

class scalar_div :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;
public:
	scalar_div(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input1(init_owner(*this) + init_name("input1") + init_label(_("Input 1")) + init_description(_("First input float")) + init_value(0.0)),
		m_input2(init_owner(*this) + init_name("input2") + init_label(_("Input 2")) + init_description(_("Second input float")) + init_value(1.0)),
		m_output(init_owner(*this) + init_name("output") + init_label(_("Output Value")) + init_description(_("Quotient of inputs")) + init_slot(sigc::mem_fun(*this, &scalar_div::get_value)))
	{
		m_input1.changed_signal().connect(m_output.make_reset_slot());
		m_input2.changed_signal().connect(m_output.make_reset_slot());
	}

	double get_value()
	{
		const double input1 = m_input1.value();
		const double input2 = m_input2.value();

		return_val_if_fail(input2 != 0, input1);

		return input1 / input2;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scalar_div > factory(
			k3d::uuid(0xa797e7a9, 0x237f45a8, 0xa6ee43b3, 0xfd58596f),
			"ScalarDivide",
			_("Divide two scalar inputs and produce their quotient as output"),
			"Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input2;
	k3d_data(double, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;

};

k3d::iplugin_factory& scalar_div_factory()
{
	return scalar_div::get_factory();
}

} //namespace libk3dcore


