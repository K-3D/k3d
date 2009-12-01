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
		\brief Modifier object that takes a mesh as input and adds edges to represent SDS creases
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>

#include <iterator>

namespace module
{

namespace subdivision_surface
{

/////////////////////////////////////////////////////////////////////////////
// make_creases

class make_creases :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	make_creases(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		assert_not_implemented();
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		assert_not_implemented();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_creases,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xf50fe5b6, 0xecfa4df3, 0x8474a557, 0xbee303ba),
				"MakeCreases",
				"Subdivides the faces around creases for SDS rendering",
				"Subdivision",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_creases_factory

k3d::iplugin_factory& make_creases_factory()
{
	return make_creases::get_factory();
}

} // namespace subdivision_surface

} // namespace module

