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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/user_property_changed_signal.h>
#include <k3dsdk/value_demand_storage.h>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// mesh_diff

class mesh_diff :
	public k3d::node
{
	typedef k3d::node base;

public:
	mesh_diff(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_threshold(init_owner(*this) + init_name("threshold") + init_label(_("Threshold")) + init_description(_("Sets the maximum allowable difference between floating-point numbers")) + init_value(0) + init_constraint(constraint::minimum<k3d::int32_t>(0))),
		m_equal(init_owner(*this) + init_name("equal") + init_label(_("Equal")) + init_description(_("True iff all input meshes are completely equivalent")) + init_value(true)),
		m_user_property_changed_signal(*this)
	{
		m_threshold.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_equal.make_slot()));
		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_equal.make_slot()));

		m_equal.set_update_slot(sigc::mem_fun(*this, &mesh_diff::execute));
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

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_threshold;
	k3d_data(k3d::bool_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_equal;
	k3d::user_property_changed_signal m_user_property_changed_signal;

	void execute(const std::vector<k3d::ihint*>& Hints, k3d::bool_t& Output)
	{
		Output = true;
		const k3d::int32_t threshold = m_threshold.pipeline_value();

		const k3d::mesh* first_mesh = 0;
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::mesh*))
			{
				if(first_mesh)
				{
					if(const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property)))
					{
						if(!k3d::almost_equal<k3d::mesh>(threshold)(*first_mesh, *mesh))
						{
							Output = false;
							return;
						}
					}
				}
				else
				{
					first_mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property));
				}
			}
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// mesh_diff_factory

k3d::iplugin_factory& mesh_diff_factory()
{
	return mesh_diff::get_factory();
}

} // namespace test

} // namespace module

