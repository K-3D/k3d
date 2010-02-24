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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/material_sink.h>
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
// set_material

class set_material :
	public k3d::material_sink<k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > >
{
	typedef k3d::material_sink<k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > > base;

public:
	set_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_material.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		k3d::imaterial* const material = m_material.pipeline_value();

		// Note: this loop is tricky because we want to avoid deep-copies where possible ...
		for(k3d::mesh::primitives_t::iterator p = Output.primitives.begin(); p != Output.primitives.end(); ++p)
		{
			// Look for primitive structures that contain a material and a selection ...
			for(k3d::mesh::named_tables_t::const_iterator s = (**p).structure.begin(); s != (**p).structure.end(); ++s)
			{
				k3d::string_t materials_name;
				k3d::string_t selection_name;

				for(k3d::mesh::named_arrays_t::const_iterator array = s->second.begin(); array != s->second.end(); ++array)
				{
					if(materials_name.empty())
					{
						if(dynamic_cast<const k3d::mesh::materials_t*>(array->second.get()))
						{
							materials_name = array->first;
							continue;
						}
					}

					if(selection_name.empty())
					{
						if(array->second->get_metadata_value(k3d::metadata::key::role()) == k3d::metadata::value::selection_role())
						{
							if(dynamic_cast<const k3d::mesh::selection_t*>(array->second.get()))
							{
								selection_name = array->first;
								continue;
							}
						}
					}
				}

				// Only now that we've got what we're looking for do we starting making changes ...
				if(materials_name.size() && selection_name.size())
				{
					k3d::mesh::primitive& primitive = p->writable();
					k3d::mesh::table_t& structure = primitive.structure[s->first];

					k3d::mesh::materials_t* const materials = structure.writable<k3d::mesh::materials_t>(materials_name);
					k3d::mesh::selection_t* const selection = structure.writable<k3d::mesh::selection_t>(selection_name);

					const k3d::uint_t begin = 0;
					const k3d::uint_t end = begin + materials->size();
					for(k3d::uint_t i = begin; i != end; ++i)
					{
						if((*selection)[i])
							(*materials)[i] = material;
					}
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<set_material,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x7743a7f2, 0x995345b9, 0x8d6dacc7, 0x3fa37029),
				"SetMaterial",
				_("Sets geometry materials"),
				"Material MeshAttributes",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// set_material_factory

k3d::iplugin_factory& set_material_factory()
{
	return set_material::get_factory();
}

} // namespace mesh_attributes

} // namespace module

