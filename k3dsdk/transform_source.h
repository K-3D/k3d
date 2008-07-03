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
		m_output_matrix(init_owner(*this) + init_name("output_matrix") + init_label(_("Output Matrix")) + init_description("Output Matrix") + init_slot(sigc::mem_fun(*this, &transform_source<base_t>::create_matrix)))
	{
	}

	iproperty& transform_source_output()
	{
		return m_output_matrix;
	}

	sigc::slot<void, ihint*> make_reset_matrix_slot()
	{
		return m_output_matrix.make_reset_slot();
	}

private:
	k3d::matrix4 create_matrix()
	{
		return on_create_matrix();
	}

	virtual k3d::matrix4 on_create_matrix() = 0;

	k3d_data(k3d::matrix4, data::immutable_name, data::change_signal, data::no_undo, data::computed_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_matrix;
};

} // namespace k3d

#endif // !K3DSDK_TRANSFORM_SOURCE_H

