#ifndef K3DSDK_TRANSFORMABLE_H
#define K3DSDK_TRANSFORMABLE_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "algebra.h"
#include "data.h"
#include "itransform_sink.h"
#include "itransform_source.h"
#include "vectors.h"

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
		m_input_matrix(init_owner(*this) + init_name("input_matrix") + init_label(_("Input Matrix")) + init_description("Input matrix") + init_value(identity3D())),
		m_output_matrix(init_owner(*this) + init_name("output_matrix") + init_label(_("Output Matrix")) + init_description(_("Output Matrix")) + init_slot(sigc::mem_fun(*this, &transformable<base_t>::matrix)))
	{
		m_input_matrix.changed_signal().connect(m_output_matrix.make_reset_slot());
	}

	iproperty& transform_source_output()
	{
		return m_output_matrix;
	}

	iproperty& transform_sink_input()
	{
		return m_input_matrix;
	}

protected:
	virtual matrix4 matrix()
	{
		return m_input_matrix.pipeline_value();
	}

	k3d_data(matrix4, data::immutable_name, data::change_signal, data::no_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_input_matrix;
	k3d_data(matrix4, data::immutable_name, data::change_signal, data::no_undo, data::computed_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_matrix;
};

} // namespace k3d

#endif // K3DSDK_TRANSFORMABLE_H

