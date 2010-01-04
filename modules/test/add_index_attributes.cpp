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
#include <k3dsdk/geometry.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// add_index_attributes

class add_index_attributes :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	add_index_attributes(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		const k3d::uint_t attribute_count = Output.points ? Output.points->size() : 0;
		k3d::mesh::indices_t& array = Output.point_attributes.create("index", new k3d::mesh::indices_t(attribute_count));
		for(k3d::uint_t i = 0; i != attribute_count; ++i)
			array[i] = i;

		for(k3d::mesh::primitives_t::iterator p = Output.primitives.begin(); p != Output.primitives.end(); ++p)
		{
			k3d::mesh::primitive& primitive = p->writable();
			for(k3d::mesh::named_tables_t::iterator attribute = primitive.attributes.begin(); attribute != primitive.attributes.end(); ++attribute)
			{
				const k3d::string_t& attribute_name = attribute->first;

				const k3d::uint_t attribute_count = k3d::component_size(primitive, attribute_name);
				if(0 == attribute_count)
					continue;

				k3d::table& attribute_table = attribute->second;

				k3d::mesh::indices_t& array = attribute_table.create("index", new k3d::mesh::indices_t(attribute_count));
				for(k3d::uint_t i = 0; i != attribute_count; ++i)
					array[i] = i;
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add_index_attributes,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xea30217d, 0xcf4f180d, 0x2321a39a, 0xc25b8741),
				"AddIndexAttributes",
				_("Adds an integer 'index' attribute to every table in a mesh, primarily for testing purposes."),
				"Test",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// add_index_attributes_factory

k3d::iplugin_factory& add_index_attributes_factory()
{
	return add_index_attributes::get_factory();
}

} // namespace test

} // namespace module

