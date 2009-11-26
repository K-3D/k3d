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
// sds_corner

class sds_corner :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	sds_corner(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sharpness(init_owner(*this) + init_name("sharpness") + init_label(_("Sharpness")) + init_description(_("Controls the sharpness of subdivision surface corners")) + init_value(0.0))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_sharpness.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.point_selection)
			return;
		const k3d::mesh::selection_t& point_selection = *Output.point_selection;

		const k3d::double_t sharpness = m_sharpness.pipeline_value();

		const k3d::uint_t point_count = point_selection.size();
		k3d::mesh::doubles_t& corners = Output.point_attributes.create<k3d::mesh::doubles_t>("corner", new k3d::mesh::doubles_t(point_count));

		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + point_count;
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
			corners[point] = point_selection[point] ? sharpness : 0.0;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_corner,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xded03869, 0x283e4531, 0x9de22a9c, 0xe763d7c6),
				"SDSCorner",
				_("Adds variable-sharpness corners to SDS surfaces"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_sharpness;
};

/////////////////////////////////////////////////////////////////////////////
// sds_corner_factory

k3d::iplugin_factory& sds_corner_factory()
{
	return sds_corner::get_factory();
}

} // namespace polyhedron

} // namespace module


