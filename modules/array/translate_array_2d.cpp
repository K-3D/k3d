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

#include "detail.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace module
{

namespace array
{


/////////////////////////////////////////////////////////////////////////////
// translate_array_2d

class translate_array_2d :
	public transform_array_2d<k3d::persistent<k3d::node> >
{
	typedef transform_array_2d<k3d::persistent<k3d::node> > base;

public:
	translate_array_2d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_offset1(init_owner(*this) + init_name("offset1") + init_label(_("Offset 1")) + init_description(_("Offset 1")) + init_value(k3d::point3(5, 0, 0))),
		m_offset2(init_owner(*this) + init_name("offset2") + init_label(_("Offset 2")) + init_description(_("Offset 2")) + init_value(k3d::point3(0, -5, 0)))
	{
		m_offset1.changed_signal().connect(make_node_change_slot());
		m_offset2.changed_signal().connect(make_node_change_slot());
	}

	const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1, unsigned long Index2, unsigned long Count2)
	{
		return k3d::translation3D((m_offset1.pipeline_value() * Index1) + (m_offset2.pipeline_value() * Index2));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory< translate_array_2d,
				k3d::interface_list<k3d::itransform_array_2d> > factory(
				k3d::uuid(0xa2d1cd67, 0x901e411b, 0x9c624ca6, 0xa02b6a5a),
				"TranslateArray2D",
				_("TranslateArray2D"),
				"Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_offset1;
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_offset2;
};

k3d::iplugin_factory& translate_array_2d_factory()
{
	return translate_array_2d::get_factory();
}

} // namespace array

} // namespace module

