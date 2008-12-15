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
#include <k3dsdk/cone.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace quadrics
{

/////////////////////////////////////////////////////////////////////////////
// cone

class cone :
	public k3d::material_sink<k3d::mesh_source<k3d::node> >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node> > base;

public:
	cone(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_transformation(init_owner(*this) + init_name("transformation") + init_label(_("Transformation")) + init_description(_("Transformation matrix used to position / orient / scale the output cone.")) + init_value(k3d::identity3())),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Defines the cone height along the +Z axis.")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Controls the radius of the output cone.")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_sweep_angle(init_owner(*this) + init_name("sweep_angle") + init_label(_("Sweep Angle")) + init_description(_("Optionally limits the sweep angle of the cone to less-than 360 degrees.")) + init_value(k3d::pi_times_2()) + init_step_increment(k3d::radians(5.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_transformation.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_height.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_sweep_angle.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		boost::scoped_ptr<k3d::cone::primitive> primitive(k3d::cone::create(Output));

		primitive->matrices.push_back(m_transformation.pipeline_value());
		primitive->materials.push_back(m_material.pipeline_value());
		primitive->heights.push_back(m_height.pipeline_value());
		primitive->radii.push_back(m_radius.pipeline_value());
		primitive->sweep_angles.push_back(m_sweep_angle.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cone > factory(
			k3d::uuid(0x61f3952d, 0x294a5474, 0xf8a17585, 0x4cbecead),
			"Cone",
			"Creates a cone primitive",
			"Quadric",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_transformation;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_sweep_angle;
};

k3d::iplugin_factory& cone_factory()
{
	return cone::get_factory();
}

} // namespace quadrics

} // namespace module

