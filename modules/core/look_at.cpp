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
#include <k3d-i18n-config.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// look_at

class look_at :
	public k3d::persistent<k3d::node>,
	public k3d::itransform_source,
	public k3d::itransform_sink
{
	typedef k3d::persistent<k3d::node> base;

public:
	look_at(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input(init_owner(*this) + init_name("input_matrix") + init_label(_("Input matrix")) + init_description(_("Input matrix")) + init_value(k3d::identity3D())),
		m_target(init_owner(*this) + init_name("target_matrix") + init_label(_("Target matrix")) + init_description(_("Target matrix")) + init_value(k3d::identity3D())),
		m_output(init_owner(*this) + init_name("output_matrix") + init_label(_("Output matrix")) + init_description(_("Read only")) + init_slot(sigc::mem_fun(*this, &look_at::output_value)))
	{
		m_input.changed_signal().connect(m_output.make_reset_slot());
		m_target.changed_signal().connect(m_output.make_reset_slot());
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
		const k3d::matrix4 input_matrix = m_input.value();
		const k3d::matrix4 target_matrix = m_target.value();

		const k3d::point3 from = input_matrix * k3d::point3(0, 0, 0);
		const k3d::point3 to = target_matrix * k3d::point3(0, 0, 0);
		const k3d::vector3 spherical = k3d::spherical(k3d::to_vector(to - from));

		return input_matrix * rotation3D(k3d::quaternion(k3d::euler_angles(0, -spherical[2], spherical[1], k3d::euler_angles::ZXYstatic)));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<look_at,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::uuid(0x4e2a30f5, 0x6d7d47ad, 0x943ccd36, 0x4b305b55),
				"LookAt",
				_("Orients a transformation matrix to look at another"),
				"Transform",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_target;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;
};

/////////////////////////////////////////////////////////////////////////////
// look_at_factory

k3d::iplugin_factory& look_at_factory()
{
	return look_at::get_factory();
}

} // namespace libk3dcore

