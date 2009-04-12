#ifndef K3DSDK_TRANSFORM_SOURCE_H
#define K3DSDK_TRANSFORM_SOURCE_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include "algebra.h"
#include "data.h"
#include "itransform_source.h"
#include "value_demand_storage.h"

namespace k3d
{

template<typename base_t>
class transform_source :
	public base_t,
	public itransform_source
{
public:
	transform_source(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_output_matrix(init_owner(*this) + init_name("output_matrix") + init_label(_("Output Matrix")) + init_description("Output Matrix") + init_value(identity3()))
	{
		m_output_matrix.set_update_slot(sigc::mem_fun(*this, &transform_source<base_t>::execute));
	}

	iproperty& transform_source_output()
	{
		return m_output_matrix;
	}

	/// Returns a slot that should be connected to input properties to signal that the output has changed
	sigc::slot<void, ihint*> make_update_matrix_slot()
	{
		return m_output_matrix.make_slot();
	}

private:
	k3d_data(k3d::matrix4, data::immutable_name, data::change_signal, data::no_undo, data::value_demand_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_matrix;

	/// Called whenever the output has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, matrix4& Output)
	{
		// We can safely ignore any hints ...
		on_update_matrix(Output);
	}

	/// Override this in derived classes, to return a new transformation matrix.
	virtual void on_update_matrix(matrix4& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_TRANSFORM_SOURCE_H

