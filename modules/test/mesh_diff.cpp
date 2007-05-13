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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/new_mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace libk3dtest
{

/////////////////////////////////////////////////////////////////////////////
// mesh_diff

class mesh_diff :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;

public:
	mesh_diff(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_equal(init_owner(*this) + init_name("equal") + init_label(_("Equal")) + init_description(_("True iff all input meshes are completely equivalent")) + init_slot(sigc::mem_fun(*this, &mesh_diff::get_equal))),
		m_user_property_changed_signal(*this)
	{
		m_user_property_changed_signal.connect(m_equal.make_reset_slot());
	}

	bool get_equal()
	{
		const k3d::dev::mesh* first_mesh = 0;
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::dev::mesh*))
			{
				if(first_mesh)
				{
					if(const k3d::dev::mesh* const mesh = boost::any_cast<k3d::dev::mesh*>(k3d::get_value(document().dag(), property)))
					{
						if(*first_mesh != *mesh)
							return false;
					}
				}
				else
				{
					first_mesh = boost::any_cast<k3d::dev::mesh*>(k3d::get_value(document().dag(), property));
				}
			}
		}

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_diff >
			factory(
				k3d::uuid(0x5aa98498, 0xa980407d, 0xb4289694, 0x2e996b30),
				"MeshDiff",
				_("Compares meshes for equivalence"),
				"Test",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(bool, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::computed_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_equal;

	k3d::user_property_changed_signal m_user_property_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// mesh_diff_factory

k3d::iplugin_factory& mesh_diff_factory()
{
	return mesh_diff::get_factory();
}

} // namespace libk3dtest

