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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/vectors.h>

namespace module
{

namespace linear_curve
{

/////////////////////////////////////////////////////////////////////////////
// lissajous_curve

class lissajous_curve :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	lissajous_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_edge_count(init_owner(*this) + init_name("edgecount") + init_label(_("Edge Count")) + init_description(_("Edge count")) + init_value(100) + init_constraint(constraint::minimum(3L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
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
		m_material.changed_signal().connect(make_reset_mesh_slot());

		m_edge_count.changed_signal().connect(make_reset_mesh_slot());
		m_mamp.changed_signal().connect(make_reset_mesh_slot());
		m_mfreq.changed_signal().connect(make_reset_mesh_slot());
		m_mphase.changed_signal().connect(make_reset_mesh_slot());
		m_xamp.changed_signal().connect(make_reset_mesh_slot());
		m_xfreq.changed_signal().connect(make_reset_mesh_slot());
		m_xphase.changed_signal().connect(make_reset_mesh_slot());
		m_yamp.changed_signal().connect(make_reset_mesh_slot());
		m_yfreq.changed_signal().connect(make_reset_mesh_slot());
		m_yphase.changed_signal().connect(make_reset_mesh_slot());
		m_zamp.changed_signal().connect(make_reset_mesh_slot());
		m_zfreq.changed_signal().connect(make_reset_mesh_slot());
		m_zphase.changed_signal().connect(make_reset_mesh_slot());
		m_width.changed_signal().connect(make_reset_mesh_slot());
		m_wrap.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const unsigned long edges = m_edge_count.pipeline_value();
		const double mamp = m_mamp.pipeline_value();
		const double mfreq = m_mfreq.pipeline_value();
		const double mphase = m_mphase.pipeline_value();
		const double xamp = m_xamp.pipeline_value();
		const double xfreq = m_xfreq.pipeline_value();
		const double xphase = m_xphase.pipeline_value();
		const double yamp = m_yamp.pipeline_value();
		const double yfreq = m_yfreq.pipeline_value();
		const double yphase = m_yphase.pipeline_value();
		const double zamp = m_zamp.pipeline_value();
		const double zfreq = m_zfreq.pipeline_value();
		const double zphase = m_zphase.pipeline_value();
		const double width = m_width.pipeline_value();
		const double wrap = m_wrap.pipeline_value();

		k3d::legacy::linear_curve_group* const group = new k3d::legacy::linear_curve_group();
		group->material = m_material.pipeline_value();
		group->wrap = wrap;
		group->constant_data["width"] = width;

		k3d::legacy::linear_curve* const curve = new k3d::legacy::linear_curve();
		group->curves.push_back(curve);

		Mesh.linear_curve_groups.push_back(group);

		// Create the curve ...
		for(unsigned long n = 0; n != edges; n++)
		{
			const double percent = static_cast<double>(n) / static_cast<double>(edges);
			const double theta = percent * k3d::pi_times_2();

			k3d::point3 coords(
				xamp * sin(xfreq * theta + xphase),
				yamp * sin(yfreq * theta + yphase),
				zamp * sin(zfreq * theta + zphase));

			if(mamp)
				coords *= 1.0 + (mamp * sin(mfreq * theta + mphase));

			k3d::legacy::point* const p = new k3d::legacy::point(coords);

			Mesh.points.push_back(p);
			curve->control_points.push_back(p);
		}
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<lissajous_curve > factory(
		k3d::uuid(0x7fdcbdc1, 0xd1c6404e, 0xae15a8eb, 0x45ffc793),
			"LissajousCurve",
			_("Generates a lissajous (sine-wave) curve"),
			"Curves",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_edge_count;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mamp;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mfreq;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mphase;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_xamp;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_xfreq;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_xphase;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_yamp;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_yfreq;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_yphase;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zamp;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zfreq;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zphase;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_wrap;
};

k3d::iplugin_factory& lissajous_curve_factory()
{
	return lissajous_curve::get_factory();
}

} // namespace linear_curve3

} // namespace module

