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
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "detail.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// add

class add :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	add(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_user_property_changed_signal(*this)
	{
		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}
	
	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		// Get the set of input meshes ...
		detail::mesh_collection meshes;

		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::mesh*))
			{
				if(const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property)))
					meshes.push_back(mesh);
			}
		}

		// Merge 'em ...
		detail::merge(meshes, m_material.pipeline_value(), k3d::blobby::ADD, true, Output);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imulti_mesh_sink> > > factory(
			k3d::uuid(0xf075d161, 0x2b42365f, 0x5eeb458b, 0x67c2a928),
			"BlobbyAdd",
			_("Combines blobbies using the Addition operator."),
			"Blobby",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d::user_property_changed_signal m_user_property_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// add_factory

k3d::iplugin_factory& add_factory()
{
	return add::get_factory();
}

} // namespace blobby

} // namespace module

