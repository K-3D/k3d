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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/vectors.h>

namespace module
{

namespace linear_curve
{

/////////////////////////////////////////////////////////////////////////////
// lissajous_curve

class lissajous_curve :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	lissajous_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_edge_count(init_owner(*this) + init_name("edgecount") + init_label(_("Edge Count")) + init_description(_("Edge count")) + init_value(100) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_mamp(init_owner(*this) + init_name("mamp") + init_label(_("Modulation Amplitude")) + init_description(_("Modulation amplitude")) + init_value(0.05) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_mfreq(init_owner(*this) + init_name("mfreq") + init_label(_("Modulation Frequency")) + init_description(_("Modulation frequency")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_mphase(init_owner(*this) + init_name("mphase") + init_label(_("Modulation Phase")) + init_description(_("Modulation phase")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_xamp(init_owner(*this) + init_name("xamp") + init_label(_("X Amplitude")) + init_description(_("X amplitude")) + init_value(5.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_xfreq(init_owner(*this) + init_name("xfreq") + init_label(_("X Frequency")) + init_description(_("X frequency")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_xphase(init_owner(*this) + init_name("xphase") + init_label(_("X Phase")) + init_description(_("X phase")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_yamp(init_owner(*this) + init_name("yamp") + init_label(_("Y Amplitude")) + init_description(_("Y amplitude")) + init_value(5.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_yfreq(init_owner(*this) + init_name("yfreq") + init_label(_("Y Frequency")) + init_description(_("Y frequency")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_yphase(init_owner(*this) + init_name("yphase") + init_label(_("Y Phase")) + init_description(_("Y phase")) + init_value(k3d::pi_over_2()) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_zamp(init_owner(*this) + init_name("zamp") + init_label(_("Z Amplitude")) + init_description(_("Z amplitude")) + init_value(0.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_zfreq(init_owner(*this) + init_name("zfreq") + init_label(_("Z Frequency")) + init_description(_("Z frequency")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_zphase(init_owner(*this) + init_name("zphase") + init_label(_("Z Phase")) + init_description(_("Z phase")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Curve width")) + init_value(0.1) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_wrap(init_owner(*this) + init_name("closed") + init_label(_("Closed Curve")) + init_description(_("Closed curve (loop)")) + init_value(true))
	{
		m_edge_count.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_wrap.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));

		m_width.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));

		m_mamp.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_mfreq.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_mphase.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_xamp.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_xfreq.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_xphase.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_yamp.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_yfreq.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_yphase.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_zamp.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_zfreq.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_zphase.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		const k3d::uint32_t edge_count = m_edge_count.pipeline_value();
		const k3d::bool_t wrap = m_wrap.pipeline_value();
		const k3d::double_t width = m_width.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		k3d::mesh::points_t& points = Output.points.create();
		k3d::mesh::selection_t& point_selection = Output.point_selection.create();

		points.resize(edge_count);
		point_selection.assign(edge_count, 0.0);

		k3d::mesh::linear_curve_groups_t& linear_curve_groups = Output.linear_curve_groups.create();
		k3d::mesh::indices_t& first_curves = linear_curve_groups.first_curves.create();
		k3d::mesh::counts_t& curve_counts = linear_curve_groups.curve_counts.create();
		k3d::mesh::bools_t& periodic_curves = linear_curve_groups.periodic_curves.create();
		k3d::mesh::materials_t& materials = linear_curve_groups.materials.create();
		k3d::mesh::indices_t& curve_first_points = linear_curve_groups.curve_first_points.create();
		k3d::mesh::counts_t& curve_point_counts = linear_curve_groups.curve_point_counts.create();
		k3d::mesh::selection_t& curve_selection = linear_curve_groups.curve_selection.create();
		k3d::mesh::indices_t& curve_points = linear_curve_groups.curve_points.create();
		k3d::mesh::doubles_t& widths = linear_curve_groups.constant_data.create<k3d::mesh::doubles_t>("width");

		first_curves.push_back(curve_first_points.size());
		curve_counts.push_back(1);
		periodic_curves.push_back(wrap);
		materials.push_back(material);
		widths.push_back(width);

		curve_first_points.push_back(curve_points.size());
		curve_point_counts.push_back(edge_count);
		curve_selection.push_back(0.0);

		for(k3d::uint32_t i = 0; i != edge_count; ++i)
			curve_points.push_back(i);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		const k3d::uint32_t edge_count = m_edge_count.pipeline_value();
		const k3d::double_t mamp = m_mamp.pipeline_value();
		const k3d::double_t mfreq = m_mfreq.pipeline_value();
		const k3d::double_t mphase = m_mphase.pipeline_value();
		const k3d::double_t xamp = m_xamp.pipeline_value();
		const k3d::double_t xfreq = m_xfreq.pipeline_value();
		const k3d::double_t xphase = m_xphase.pipeline_value();
		const k3d::double_t yamp = m_yamp.pipeline_value();
		const k3d::double_t yfreq = m_yfreq.pipeline_value();
		const k3d::double_t yphase = m_yphase.pipeline_value();
		const k3d::double_t zamp = m_zamp.pipeline_value();
		const k3d::double_t zfreq = m_zfreq.pipeline_value();
		const k3d::double_t zphase = m_zphase.pipeline_value();

		k3d::mesh::points_t& points = const_cast<k3d::mesh::points_t&>(*Output.points);

		for(k3d::uint32_t i = 0; i != edge_count; ++i)
		{
			const k3d::double_t percent = static_cast<k3d::double_t>(i) / static_cast<k3d::double_t>(edge_count);
			const k3d::double_t theta = percent * k3d::pi_times_2();

			k3d::point3 point(
				xamp * sin(xfreq * theta + xphase),
				yamp * sin(yfreq * theta + yphase),
				zamp * sin(zfreq * theta + zphase));

			if(mamp)
			{
				point *= 1.0 + (mamp * sin(mfreq * theta + mphase));
			}

			points[i] = point;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<lissajous_curve > factory(
		k3d::uuid(0x7fdcbdc1, 0xd1c6404e, 0xae15a8eb, 0x45ffc793),
			"LissajousCurve",
			_("Generates a lissajous (sine-wave) curve"),
			"Curve",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_edge_count;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mamp;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mfreq;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mphase;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_xamp;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_xfreq;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_xphase;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_yamp;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_yfreq;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_yphase;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zamp;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zfreq;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zphase;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_wrap;
};

k3d::iplugin_factory& lissajous_curve_factory()
{
	return lissajous_curve::get_factory();
}

} // namespace linear_curve3

} // namespace module

