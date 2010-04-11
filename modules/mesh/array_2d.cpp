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
#include <k3dsdk/hints.h>
#include <k3dsdk/itransform_array_2d.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// array_2d_implementation

class array_2d_implementation :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	array_2d_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_layout(init_owner(*this) + init_name("layout") + init_label(_("Layout")) + init_description(_("Layout")) + init_value<k3d::itransform_array_2d*>(0)),
		m_count1(init_owner(*this) + init_name("count1") + init_label(_("Count 1")) + init_description(_("Number of mesh copies")) + init_value(5) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(0))),
		m_count2(init_owner(*this) + init_name("count2") + init_label(_("Count 2")) + init_description(_("Number of mesh copies")) + init_value(5) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(0)))
	{
		m_layout.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reset_mesh_slot()));
		m_count1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reset_mesh_slot()));
		m_count2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reset_mesh_slot()));
	}


	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::itransform_array_2d* const layout = m_layout.pipeline_value();
		if(!layout)
			return;

		const k3d::int32_t count1 = m_count1.pipeline_value();
		const k3d::int32_t count2 = m_count2.pipeline_value();
		for(k3d::int32_t i = 0; i != count1; ++i)
		{
			for(k3d::int32_t j = 0; j != count2; ++j)
			{
				// Merge input geometry into our output ...
				k3d::uint_t point_begin = 0;
				k3d::uint_t point_end = 0;
				k3d::mesh::append(Input, Output, &point_begin, &point_end);

				// Transform the corresponding output points ...
				if(Output.points)
				{
					k3d::mesh::points_t& output_points = Output.points.writable();

					const k3d::matrix4 matrix = layout->get_element(i, count1, j, count2);
					for(k3d::uint_t point = point_begin; point != point_end; ++point)
						output_points[point] = matrix * output_points[point];
				}
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<array_2d_implementation,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x4818d995, 0xf8664006, 0xba02ded4, 0x8cb9defb),
				"MeshArray2D",
				_("Makes copies of a mesh along two dimensions"),
				"Mesh Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::itransform_array_2d*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_layout;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_count1;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_count2;
};

/////////////////////////////////////////////////////////////////////////////
// array_2d_factory

k3d::iplugin_factory& array_2d_factory()
{
	return array_2d_implementation::get_factory();
}

} // namespace mesh

} // namespace module


