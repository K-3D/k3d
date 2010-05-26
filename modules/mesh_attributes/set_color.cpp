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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/component.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace mesh_attributes
{

/////////////////////////////////////////////////////////////////////////////
// set_color

class set_color :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	set_color(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_name(init_owner(*this) + init_name("name") + init_label(_("Name")) + init_description(_("Name for the color attribute array")) + init_value(k3d::string_t("Cs"))),
		m_default_color(init_owner(*this) + init_name("default_color") + init_label(_("Default Color")) + init_description(_("Color used for unselected faces in case no previous array existed")) + init_value(k3d::color(0, 1, 1))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color to apply to selected faces")) + init_value(k3d::color(0, 0, 1)))

	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_name.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_default_color.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_color.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		const k3d::string_t array_name = m_name.pipeline_value();
		const k3d::color default_color = m_default_color.pipeline_value();
		const k3d::color color = m_color.pipeline_value();

		// Note: this loop is tricky because we want to avoid deep-copies where possible ...
		for(k3d::uint_t p = 0; p != Output.primitives.size(); ++p)
		{
			for(k3d::mesh::named_tables_t::const_iterator s = Output.primitives[p]->structure.begin(); s != Output.primitives[p]->structure.end(); ++s)
			{
				// Look for the selection array in a structure
				for(k3d::mesh::named_arrays_t::const_iterator array = s->second.begin(); array != s->second.end(); ++array)
				{
					if(array->second.get() && array->second->get_metadata_value(k3d::metadata::key::role()) == k3d::metadata::value::selection_role())
					{
						const k3d::mesh::selection_t* selections = dynamic_cast<const k3d::mesh::selection_t*>(array->second.get());
						if(selections)
						{
							const k3d::string_t& structure_name = s->first;
							// Now look for an existing color array, or create it if it doesn't exist
							k3d::mesh::primitive& primitive = Output.primitives[p].writable();
							for(k3d::mesh::named_tables_t::iterator attribute = primitive.attributes.begin(); attribute != primitive.attributes.end(); ++attribute)
							{
								const k3d::string_t& attribute_name = attribute->first;
								if(attribute_name != structure_name)
									continue;

								const k3d::uint_t attribute_count = k3d::component_size(primitive, attribute_name);
								if(0 == attribute_count)
									continue;

								return_if_fail(attribute_count == selections->size());

								k3d::table& attribute_table = attribute->second;
								k3d::mesh::colors_t& array = attribute_table.lookup<k3d::mesh::colors_t>(array_name) ? *(attribute_table.writable<k3d::mesh::colors_t>(array_name)) : attribute_table.create(array_name, new k3d::mesh::colors_t(attribute_count, default_color));

								for(k3d::uint_t i = 0; i != attribute_count; ++i)
									if(selections->at(i))
										array[i] = color * selections->at(i);
							}
						}
					}
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<set_color,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9d2cc7f6, 0x27462cca, 0x77e8dca7, 0x25608d51),
				"SetColor",
				_("Sets geometry color"),
				"Material MeshAttributes",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_name;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_default_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

/////////////////////////////////////////////////////////////////////////////
// set_color_factory

k3d::iplugin_factory& set_color_factory()
{
	return set_color::get_factory();
}

} // namespace mesh_attributes

} // namespace module

