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
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// sds_crease

class sds_crease :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	sds_crease(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sharpness(init_owner(*this) + init_name("sharpness") + init_label(_("Sharpness")) + init_description(_("Controls the sharpness of subdivision surface creases")) + init_value(0.0))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_sharpness.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		const k3d::double_t sharpness = m_sharpness.pipeline_value();

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			const k3d::uint_t edge_count = polyhedron->edge_selections.size();
			k3d::mesh::doubles_t* creases_ptr = polyhedron->edge_attributes.writable<k3d::mesh::doubles_t>("crease");
			k3d::mesh::doubles_t& creases = creases_ptr ? *creases_ptr : polyhedron->edge_attributes.create<k3d::mesh::doubles_t>("crease", new k3d::mesh::doubles_t(edge_count));
			if(!creases_ptr)
				creases.assign(creases.size(), 0);

			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + edge_count;
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(polyhedron->edge_selections[edge])
					creases[edge] = sharpness;
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_crease,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x78b79c12, 0x4c174732, 0xa67d6317, 0x6dbec1fb),
				"SDSCrease",
				_("Adds variable-sharpness creases to SDS surfaces"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_sharpness;
};

/////////////////////////////////////////////////////////////////////////////
// sds_crease_factory

k3d::iplugin_factory& sds_crease_factory()
{
	return sds_crease::get_factory();
}

} // namespace polyhedron

} // namespace module

