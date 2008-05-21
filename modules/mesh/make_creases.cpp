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
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/subdivision_surface/sds_crease.h>

#include <iterator>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// make_creases_implementation

class make_creases_implementation :
	public k3d::legacy::mesh_modifier<k3d::node >
{
	typedef k3d::legacy::mesh_modifier<k3d::node > base;

public:
	make_creases_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
			k3d::sds::crease(Mesh, **polyhedron);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_creases_implementation,
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
	return make_creases_implementation::get_factory();
}

} // namespace mesh

} // namespace module

