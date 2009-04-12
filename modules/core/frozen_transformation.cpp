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
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/value_demand_storage.h>

namespace module
{

namespace core
{

/////////////////////////////////////////////////////////////////////////////
// frozen_transformation

class frozen_transformation :
	public k3d::node,
	public k3d::itransform_source,
	public k3d::itransform_sink
{
	typedef k3d::node base;

public:
	frozen_transformation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input(init_owner(*this) + init_name("input_matrix") + init_label(_("Input matrix")) + init_description(_("Input matrix")) + init_value(k3d::identity3())),
		m_matrix(init_owner(*this) + init_name("matrix") + init_label(_("Matrix")) + init_description(_("Transformation matrix")) + init_value(k3d::identity3())),
		m_output(init_owner(*this) + init_name("output_matrix") + init_label(_("Output Matrix")) + init_description(_("Output matrix (read only)")) + init_value(k3d::identity3()))
	{
		m_input.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_output.make_slot()));
		m_matrix.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_output.make_slot()));

		m_output.set_update_slot(sigc::mem_fun(*this, &frozen_transformation::execute));
	}

	k3d::iproperty& transform_source_output()
	{
		return m_output;
	}

	k3d::iproperty& transform_sink_input()
	{
		return m_input;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<frozen_transformation,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::classes::FrozenTransformation(),
				"FrozenTransformation",
				_("Stores an arbitrary transformation matrix"),
				"Transform",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_matrix;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_output;

	/// Called whenever the output matrix has been modified and needs to be updated.
	void execute(const std::vector<k3d::ihint*>& Hints, k3d::matrix4& Matrix)
	{
		Matrix = m_input.pipeline_value() * m_matrix.pipeline_value();
	}

};

/////////////////////////////////////////////////////////////////////////////
// frozen_transformation_factory

k3d::iplugin_factory& frozen_transformation_factory()
{
	return frozen_transformation::get_factory();
}

} // namespace core

} // namespace module

