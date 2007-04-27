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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// view_matrix

class view_matrix :
	public k3d::persistent<k3d::node>,
	public k3d::itransform_source
{
	typedef k3d::persistent<k3d::node> base;

public:
	view_matrix(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_position(init_owner(*this) + init_name("position") + init_label(_("Position")) + init_description(_("Position")) + init_value(k3d::identity3D())),
		m_look(init_owner(*this) + init_name("look") + init_label(_("Look Position")) + init_description(_("Look Position")) + init_value(k3d::translation3D(k3d::vector3(0, 0, 1)))),
		m_up(init_owner(*this) + init_name("up") + init_label(_("Up Position")) + init_description(_("Up Position")) + init_value(k3d::translation3D(k3d::vector3(0, 1, 0)))),
		m_output(init_owner(*this) + init_name("output_matrix") + init_label(_("Output matrix")) + init_description(_("Read only")) + init_slot(sigc::mem_fun(*this, &view_matrix::output_value)))
	{
		m_position.changed_signal().connect(m_output.make_reset_slot());
		m_look.changed_signal().connect(m_output.make_reset_slot());
		m_up.changed_signal().connect(m_output.make_reset_slot());
	}

	k3d::iproperty& transform_source_output()
	{
		return m_output;
	}

	k3d::matrix4 output_value()
	{
		const k3d::point3 look_position = m_look.value() * k3d::point3();
		const k3d::point3 up_position = m_up.value() * k3d::point3();
		const k3d::point3 position = m_position.value() * k3d::point3();

		return k3d::view_matrix(
			k3d::to_vector(look_position - position),
			k3d::to_vector(up_position - position),
			position);
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

	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_position;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_look;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_up;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;
};

/////////////////////////////////////////////////////////////////////////////
// view_matrix_factory

k3d::iplugin_factory& view_matrix_factory()
{
	return view_matrix::get_factory();
}

} // namespace libk3dcore

