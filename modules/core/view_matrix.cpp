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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace core
{

/////////////////////////////////////////////////////////////////////////////
// view_matrix

class view_matrix :
	public k3d::transformable<k3d::node >
{
	typedef k3d::transformable<k3d::node > base;

public:
	view_matrix(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_position(init_owner(*this) + init_name("position") + init_label(_("Position")) + init_description(_("Position")) + init_value(k3d::identity3())),
		m_look(init_owner(*this) + init_name("look") + init_label(_("Look Position")) + init_description(_("Look Position")) + init_value(k3d::translate3(k3d::vector3(0, 0, 1)))),
		m_up(init_owner(*this) + init_name("up") + init_label(_("Up Position")) + init_description(_("Up Position")) + init_value(k3d::translate3(k3d::vector3(0, 1, 0))))
	{
		m_position.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_matrix_slot()));
		m_look.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_matrix_slot()));
		m_up.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_matrix_slot()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<view_matrix,
			k3d::interface_list<k3d::itransform_source> > factory(
				k3d::uuid(0x8c5f4eef, 0xa858499f, 0x83774152, 0x147b7fe2),
				"ViewMatrix",
				_("Generates a transformation matrix from a position, a look vector, and an up vector"),
				"Transform",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_position;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_look;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_up;

	void on_update_matrix(const k3d::matrix4& Input, k3d::matrix4& Output)
	{
		const k3d::point3 look_position = m_look.pipeline_value() * k3d::point3();
		const k3d::point3 up_position = m_up.pipeline_value() * k3d::point3();
		const k3d::point3 position = m_position.pipeline_value() * k3d::point3();

		Output = Input * k3d::view_matrix(look_position - position, up_position - position, position);
	}
};

/////////////////////////////////////////////////////////////////////////////
// view_matrix_factory

k3d::iplugin_factory& view_matrix_factory()
{
	return view_matrix::get_factory();
}

} // namespace core

} // namespace module

