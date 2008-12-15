// K-3D
// Copyright (c) 2008, Timothy M. Shead 
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
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/torus.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace quadrics
{

/////////////////////////////////////////////////////////////////////////////
// torus

class torus :
	public k3d::material_sink<k3d::mesh_source<k3d::node> >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node> > base;

public:
	torus(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_transformation(init_owner(*this) + init_name("transformation") + init_label(_("Transformation")) + init_description(_("Transformation matrix used to position / orient / scale the output torus.")) + init_value(k3d::identity3())),
		m_major_radius(init_owner(*this) + init_name("major_radius") + init_label(_("Major Radius")) + init_description(_("Controls the major radius of the output torus.")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_minor_radius(init_owner(*this) + init_name("minor_radius") + init_label(_("Minor Radius")) + init_description(_("Controls the minor radius of the output torus.")) + init_value(2.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_phi_min(init_owner(*this) + init_name("phi_min") + init_label(_("Phi Min")) + init_description(_("Phi Min.")) + init_value(0.0) + init_step_increment(k3d::radians(5.0)) + init_units(typeid(k3d::measurement::angle))),
		m_phi_max(init_owner(*this) + init_name("phi_max") + init_label(_("Phi Max")) + init_description(_("Phi Max.")) + init_value(k3d::pi_times_2()) + init_step_increment(k3d::radians(5.0)) + init_units(typeid(k3d::measurement::angle))),
		m_sweep_angle(init_owner(*this) + init_name("sweep_angle") + init_label(_("Sweep Angle")) + init_description(_("Optionally limits the sweep angle of the torus to less-than 360 degrees.")) + init_value(k3d::pi_times_2()) + init_step_increment(k3d::radians(5.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_transformation.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_major_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_minor_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_phi_min.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_phi_max.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_sweep_angle.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		boost::scoped_ptr<k3d::torus::primitive> primitive(k3d::torus::create(Output));

		primitive->matrices.push_back(m_transformation.pipeline_value());
		primitive->materials.push_back(m_material.pipeline_value());
		primitive->major_radii.push_back(m_major_radius.pipeline_value());
		primitive->minor_radii.push_back(m_minor_radius.pipeline_value());
		primitive->phi_min.push_back(m_phi_min.pipeline_value());
		primitive->phi_max.push_back(m_phi_max.pipeline_value());
		primitive->sweep_angles.push_back(m_sweep_angle.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<torus > factory(
			k3d::uuid(0xecc68314, 0x004e8d14, 0xd1571fb6, 0x247612b3),
			"Torus",
			"Creates a torus primitive",
			"Quadric",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_transformation;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_major_radius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_minor_radius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_phi_min;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_phi_max;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_sweep_angle;
};

k3d::iplugin_factory& torus_factory()
{
	return torus::get_factory();
}

} // namespace quadrics

} // namespace module

