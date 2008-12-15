// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include <k3dsdk/algebra.h>
#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/blobby.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// points_to_blobby

class points_to_blobby :
	public k3d::material_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	points_to_blobby(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Points radius")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!Input.points)
			return;

		const k3d::mesh::points_t& input_points = *Input.points;
		const k3d::mesh::attribute_arrays_t& input_vertex_data = Input.vertex_data;

		boost::scoped_ptr<k3d::blobby::primitive> blobby(k3d::blobby::create(Output));

		blobby->vertex_data = input_vertex_data.clone_types();
		k3d::attribute_array_copier vertex_data_copier(input_vertex_data, blobby->vertex_data);

		const k3d::double_t radius = m_radius.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		blobby->first_primitives.push_back(blobby->primitives.size());
		blobby->primitive_counts.push_back(input_points.size());
		blobby->first_operators.push_back(blobby->operators.size());
		blobby->operator_counts.push_back(1);
		blobby->materials.push_back(material);

		const k3d::uint_t points_begin = 0;
		const k3d::uint_t points_end = points_begin + input_points.size();
		for(k3d::uint_t point = points_begin; point != points_end; ++point)
		{
			blobby->primitives.push_back(k3d::blobby::ELLIPSOID);
			blobby->primitive_first_floats.push_back(blobby->floats.size());
			blobby->primitive_float_counts.push_back(16);
			vertex_data_copier.push_back(point);

			k3d::matrix4 matrix = k3d::transpose(k3d::translate3(k3d::to_vector(input_points[point])) * k3d::scale3(radius, radius, radius));
			blobby->floats.insert(blobby->floats.end(), static_cast<double*>(matrix), static_cast<double*>(matrix) + 16);
		}

		blobby->operators.push_back(k3d::blobby::ADD);
		blobby->operator_first_operands.push_back(blobby->operands.size());
		blobby->operator_operand_counts.push_back(input_points.size() + 1);
		blobby->operands.push_back(input_points.size());
		for(k3d::uint_t i = 0; i != input_points.size(); ++i)
			blobby->operands.push_back(i);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<points_to_blobby,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9d5d69d9, 0xfe994aa0, 0x9b7dee22, 0x1823bd2c),
				"PointsToBlobby",
				"Converts input points to an implicit surface composed of blobby ellipsoids",
				"Blobby",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
};

/////////////////////////////////////////////////////////////////////////////
// points_to_blobby_factory

k3d::iplugin_factory& points_to_blobby_factory()
{
	return points_to_blobby::get_factory();
}

} // namespace blobby

} // namespace module

