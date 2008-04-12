// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

namespace module
{

namespace array
{

/////////////////////////////////////////////////////////////////////////////
// translate_array_1d

class translate_array_1d :
	public transform_array_1d
{
	typedef transform_array_1d base;

public:
	translate_array_1d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_offset(init_owner(*this) + init_name("offset") + init_label(_("Offset")) + init_description(_("Offset")) + init_value(k3d::point3(5, 0, 0)))
	{
		m_offset.changed_signal().connect(make_node_change_slot());
	}

	const k3d::matrix4 on_get_element(unsigned long Index1, unsigned long Count1)
	{
		return k3d::translation3D((m_offset.pipeline_value() * Index1));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory< translate_array_1d,
				k3d::interface_list<k3d::itransform_array_1d> > factory(
				k3d::uuid(0x172b9170, 0x91d2449b, 0xaa057e85, 0xafb2c019),
				"TranslateArray1D",
				_("TranslateArray1D"),
				"Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_offset;
};

k3d::iplugin_factory& translate_array_1d_factory()
{
	return translate_array_1d::get_factory();
}

} // namespace array

} // namespace module

