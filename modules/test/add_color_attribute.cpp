// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/component.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// add_color_attribute

class add_color_attribute :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	add_color_attribute(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_component(init_owner(*this) + init_name("component") + init_label(_("Component")) + init_description(_("Mesh component to receive a new color attribute.")) + init_value(k3d::string_t("vertex")) + init_values(k3d::component_values())),
		m_name(init_owner(*this) + init_name("name") + init_label(_("Name")) + init_description(_("New attribute name.")) + init_value(k3d::string_t("Cs")))
	{
		m_component.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reset_mesh_slot()));
		m_name.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		const std::vector<k3d::color> colors = boost::assign::list_of(k3d::color(1, 0, 0))(k3d::color(0, 1, 0))(k3d::color(0, 0, 1))(k3d::color(1, 1, 1))(k3d::color(1, 1, 0))(k3d::color(0, 1, 1))(k3d::color(1, 0, 1)); 

		const k3d::string_t component = m_component.pipeline_value();
		const k3d::string_t name = m_name.pipeline_value();

		Output = Input;

		for(k3d::mesh::primitives_t::iterator p = Output.primitives.begin(); p != Output.primitives.end(); ++p)
		{
			k3d::mesh::primitive& primitive = p->writable();
			for(k3d::mesh::named_tables_t::iterator attribute = primitive.attributes.begin(); attribute != primitive.attributes.end(); ++attribute)
			{
				const k3d::string_t& attribute_name = attribute->first;
				if(attribute_name != component)
					continue;

				const k3d::uint_t attribute_count = k3d::component_size(primitive, attribute_name);
				if(0 == attribute_count)
					continue;

				k3d::table& attribute_table = attribute->second;

				k3d::mesh::colors_t& array = attribute_table.create(name, new k3d::mesh::colors_t(attribute_count));
				for(k3d::uint_t i = 0; i != attribute_count; ++i)
					array[i] = colors[i % colors.size()];
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add_color_attribute,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x0caae90d, 0x924154b7, 0x5f977ea9, 0x91eec131),
				"AddColorAttributes",
				_("Adds a color attribute to a mesh component, primarily for testing purposes."),
				"Test",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_component;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_name;
};

/////////////////////////////////////////////////////////////////////////////
// add_color_attribute_factory

k3d::iplugin_factory& add_color_attribute_factory()
{
	return add_color_attribute::get_factory();
}

} // namespace test

} // namespace module

