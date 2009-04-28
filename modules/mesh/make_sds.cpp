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
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// make_sds_implementation

class make_sds_implementation :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;
	typedef k3d::typed_array<std::string> tags_t;

public:
	make_sds_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_interpolateboundary(init_owner(*this) + init_name("interpolateboundary") + init_label(_("Interpolate boundary")) + init_description(_("Render sharp boundaries on the resulting subdivision surface")) + init_value(true))
	{
		m_interpolateboundary.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		const k3d::bool_t interpolateboundary = m_interpolateboundary.pipeline_value();

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(*primitive));
			if(!polyhedron)
				continue;

			std::fill(polyhedron->shell_types.begin(), polyhedron->shell_types.end(), k3d::polyhedron::CATMULL_CLARK);
			
			if(interpolateboundary)
				polyhedron->constant_data.create("interpolateboundary", new k3d::typed_array<k3d::string_t>(polyhedron->shell_types.size(), "interpolateboundary"));
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_sds_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x6550aef5, 0xefd24248, 0x8833eeab, 0xa267191e),
				"MakeSDS",
				_("Converts input polyhedra to SDS surfaces"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_interpolateboundary;
};

/////////////////////////////////////////////////////////////////////////////
// make_sds_factory

k3d::iplugin_factory& make_sds_factory()
{
	return make_sds_implementation::get_factory();
}

} // namespace mesh

} // namespace module


