#ifndef K3DSDK_TRANSFORMABLE_H
#define K3DSDK_TRANSFORMABLE_H

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

#include "algebra.h"
#include "data.h"
#include "hints.h"
#include "itransform_sink.h"
#include "itransform_source.h"
#include "value_demand_storage.h"

namespace k3d
{

/// Provides a boilerplate implementation of an object that can act as a transform source and a transform sink
template<typename base_t>
class transformable :
	public base_t,
	public itransform_source,
	public itransform_sink
{
public:
	transformable(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_input_matrix(init_owner(*this) + init_name("input_matrix") + init_label(_("Input Matrix")) + init_description("Input matrix") + init_value(identity3())),
		m_output_matrix(init_owner(*this) + init_name("output_matrix") + init_label(_("Output Matrix")) + init_description(_("Output Matrix")) + init_value(identity3()))
	{
		m_input_matrix.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_update_matrix_slot()));

		m_output_matrix.set_update_slot(sigc::mem_fun(*this, &transformable<base_t>::execute));
	}

	iproperty& transform_source_output()
	{
		return m_output_matrix;
	}

	iproperty& transform_sink_input()
	{
		return m_input_matrix;
	}

	/// Returns a slot that should be connected to input properties to signal that the output has changed
	sigc::slot<void, ihint*> make_update_matrix_slot()
	{
		return m_output_matrix.make_slot();
	}

	/// Convenience function that returns this object's transformation matrix
	const matrix4 matrix()
	{
		return m_output_matrix.pipeline_value();
	}

protected:
	k3d_data(matrix4, data::immutable_name, data::change_signal, data::no_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_input_matrix;

private:
	k3d_data(matrix4, data::immutable_name, data::change_signal, data::no_undo, data::value_demand_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_matrix;

	/// Called whenever the output has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, matrix4& Output)
	{
		// We can safely ignore any hints ...
		on_update_matrix(m_input_matrix.pipeline_value(), Output);
	}

	/// Override this in derived classes, to return a new / modified transformation matrix.
	virtual void on_update_matrix(const matrix4& Input, matrix4& Output)
	{
		Output = Input;
	}
};

} // namespace k3d

#endif // !K3DSDK_TRANSFORMABLE_H

