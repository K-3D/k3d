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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include "isimulation.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imatrix_source.h>
#include <k3dsdk/imatrix_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/value_demand_storage.h>

namespace module
{

namespace ode
{

/////////////////////////////////////////////////////////////////////////////
// node

class node :
	public k3d::node,
	public k3d::imatrix_source,
	public k3d::imatrix_sink
{
	typedef k3d::node base;

public:
	node(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_simulation(init_owner(*this) + init_name("simulation") + init_label(_("Simulation")) + init_description(_("ODE simulation node.")) + init_value<ode::isimulation*>(0)),
		m_input(init_owner(*this) + init_name("input_matrix") + init_label(_("Input matrix")) + init_description(_("Input matrix")) + init_value(k3d::identity3())),
		m_mass(init_owner(*this) + init_name("mass") + init_label(_("Mass")) + init_description(_("Mass.")) + init_value(1.0)),
		m_linear_velocity(init_owner(*this) + init_name("linear_velocity") + init_label(_("Linear Velocity")) + init_description(_("Defines the initial linear velocity (if any).")) + init_value(k3d::vector3(0, 0, 0))),
		m_output(init_owner(*this) + init_name("output_matrix") + init_label(_("Output Matrix")) + init_description(_("Output matrix (read only)")) + init_value(k3d::identity3())),
		m_body_id(0)
	{
		m_simulation.changed_signal().connect(sigc::mem_fun(*this, &node::new_simulation));

		m_input.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(sigc::mem_fun(*this, &node::state_changed)));
		m_mass.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(sigc::mem_fun(*this, &node::state_changed)));
		m_linear_velocity.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(sigc::mem_fun(*this, &node::state_changed)));

		m_output.set_update_slot(sigc::mem_fun(*this, &node::execute));
	}

	k3d::iproperty& matrix_source_output()
	{
		return m_output;
	}

	k3d::iproperty& matrix_sink_input()
	{
		return m_input;
	}

	void new_simulation(k3d::ihint*)
	{
		if(m_body_id)
		{
			dBodyDestroy(m_body_id);
			m_body_id = 0;
		}

		m_simulation_changed_connection.disconnect();
		m_initial_state_connection.disconnect();
		m_update_state_connection.disconnect();

		if(ode::isimulation* const simulation = m_simulation.pipeline_value())
		{
			m_body_id = dBodyCreate(simulation->world());
			dGeomID geometry_id = dCreateBox(simulation->space(), 5, 5, 5);
			dGeomSetBody(geometry_id, m_body_id);

			m_simulation_changed_connection = simulation->connect_simulation_changed_signal(sigc::mem_fun(*this, &node::simulation_changed));
			m_initial_state_connection = simulation->connect_initial_state_signal(sigc::mem_fun(*this, &node::initial_state));
			m_update_state_connection = simulation->connect_update_state_signal(sigc::mem_fun(*this, &node::update_state));

			simulation->simulation_changed();
		}
	}

	void state_changed(k3d::ihint*)
	{
		if(ode::isimulation* const simulation = m_simulation.pipeline_value())
			simulation->simulation_changed();
	}

	void simulation_changed()
	{
		m_output.update(0);
	}

	void initial_state()
	{
		if(!m_body_id)
			return;

		const k3d::matrix4 resting = m_input.pipeline_value();
		const k3d::vector3 linear_velocity = m_linear_velocity.pipeline_value();

		dReal p[] = { resting[0][3], resting[1][3], resting[2][3] };
		dReal r[] = { resting[0][0], resting[1][0], resting[2][0], 0, resting[0][1], resting[1][1], resting[2][1], 0, resting[0][2], resting[1][2], resting[2][2] };

		dBodySetPosition(m_body_id, p[0], p[1], p[2]);
		dBodySetRotation(m_body_id, r);
		dBodySetLinearVel(m_body_id, linear_velocity[0], linear_velocity[1], linear_velocity[2]);
		dBodySetAngularVel(m_body_id, 0, 0, 0);
	}

	void update_state()
	{
		if(!m_body_id)
			return;
		
		dMass mass;
		dMassSetBoxTotal(&mass, m_mass.pipeline_value(), 5, 5, 5);
		dBodySetMass(m_body_id, &mass);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<node,
			k3d::interface_list<k3d::imatrix_source,
			k3d::interface_list<k3d::imatrix_sink > > > factory(
				k3d::uuid(0x8d0deddb, 0x10409128, 0x04d1b2b1, 0x62c4cda1),
				"ODEPhysicalNode",
				_("Rigid-body dynamics object."),
				"Simulation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(ode::isimulation*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_simulation;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_mass;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_linear_velocity;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_output;

	sigc::connection m_simulation_changed_connection;
	sigc::connection m_initial_state_connection;
	sigc::connection m_update_state_connection;

	dBodyID m_body_id;

	/// Called whenever the output matrix has been modified and needs to be updated.
	void execute(const std::vector<k3d::ihint*>& Hints, k3d::matrix4& Matrix)
	{
		if(m_body_id)
		{
			if(ode::isimulation* const simulation = m_simulation.pipeline_value())
			{
				simulation->update();

				const dReal* const p = dBodyGetPosition(m_body_id);
				const dReal* const r = dBodyGetRotation(m_body_id);

				Matrix = k3d::matrix4(
						k3d::vector4(r[0], r[4], r[8], p[0]),
						k3d::vector4(r[1], r[5], r[9], p[1]),
						k3d::vector4(r[2], r[6], r[10], p[2]),
						k3d::vector4(0, 0, 0, 1));
				return;
			}
		}

		Matrix = m_input.pipeline_value();
	}
};

/////////////////////////////////////////////////////////////////////////////
// node_factory

k3d::iplugin_factory& node_factory()
{
	return node::get_factory();
}

} // namespace ode

} // namespace module

