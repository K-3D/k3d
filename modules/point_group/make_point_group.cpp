// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/point_group.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace point_group
{

/////////////////////////////////////////////////////////////////////////////
// make_point_group

class make_point_group :
	public k3d::material_sink<k3d::mesh_modifier<k3d::node> >
{
	typedef k3d::material_sink<k3d::mesh_modifier<k3d::node> > base;

public:
	make_point_group(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Controls the width of the output points.")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_width.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!Input.points)
			return;

		const k3d::uint_t point_count = Input.points->size();

		Output.points = Input.points;
		Output.point_selection = Input.point_selection;
		Output.vertex_data = Input.vertex_data;

		boost::scoped_ptr<k3d::point_group::primitive> primitive(k3d::point_group::create(Output));
		k3d::typed_array<k3d::double_t>& width = primitive->constant_data.create<k3d::typed_array<k3d::double_t> >("constantwidth");

		primitive->material.push_back(m_material.pipeline_value());
		primitive->points.assign(point_count, 0);
		for(k3d::uint_t i = 0; i != point_count; ++i)
			primitive->points[i] = i;
		width.push_back(m_width.pipeline_value());
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_point_group,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x3e4086fe, 0x0246f9ab, 0xeef01f8a, 0xf5cf5cff),
				"MakePointGroup",
				_("Converts input gprims into a single point group"),
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
};

/////////////////////////////////////////////////////////////////////////////
// make_point_group_factory

k3d::iplugin_factory& make_point_group_factory()
{
	return make_point_group::get_factory();
}

} // namespace point_group

} // namespace module

