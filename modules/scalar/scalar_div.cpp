// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Anders Dahnielson (anders@dahnielson.com)
*/

#include "scalar_source.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>

namespace module
{

namespace scalar
{

/// An object that take two doubles as input and produce their quotient as output
class scalar_div :
	public scalar_source
{
	typedef scalar_source base;
public:
	scalar_div(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, _("Quotient of inputs")),
		m_input1(init_owner(*this) + init_name("input1") + init_label(_("Input 1")) + init_description(_("First input float")) + init_value(0.0)),
		m_input2(init_owner(*this) + init_name("input2") + init_label(_("Input 2")) + init_description(_("Second input float")) + init_value(1.0))
	{
		m_input1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_value_slot()));
		m_input2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_value_slot()));
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
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input1;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input2;

	void on_update_value(k3d::double_t& Output)
	{
		const k3d::double_t input1 = m_input1.pipeline_value();
		const k3d::double_t input2 = m_input2.pipeline_value();

		Output = input2 != 0 ?  input1 / input2 : input1;
	}
};

k3d::iplugin_factory& scalar_div_factory()
{
	return scalar_div::get_factory();
}

} //namespace scalar

} // namespace module


