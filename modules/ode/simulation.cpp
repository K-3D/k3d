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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/time_sink.h>
#include <k3dsdk/vector3.h>

namespace module
{

namespace ode
{

/////////////////////////////////////////////////////////////////////////////
// simulation

class simulation :
	public k3d::time_sink<k3d::node>,
	public isimulation
{
	typedef k3d::time_sink<k3d::node> base;

public:
	simulation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_world_id(dWorldCreate()),
		m_space_id(dSimpleSpaceCreate(0)),
		m_plane_id(dCreatePlane(m_space_id, 0, 0, 1, 0)),
		m_contact_joint_group_id(dJointGroupCreate(0)),
		m_last_time(0.0),
		m_modified(false),
		m_gravity(init_owner(*this) + init_name("gravity") + init_label(_("Gravity")) + init_description(_("Defines the gravity direction and magnitude (if any).")) + init_value(k3d::vector3(0, 0, -9.81)))
	{
		m_time.changed_signal().connect(sigc::mem_fun(*this, &simulation::time_changed));
	}

	~simulation()
	{
		dJointGroupDestroy(m_contact_joint_group_id);
		dGeomDestroy(m_plane_id);
		dSpaceDestroy(m_space_id);
		dWorldDestroy(m_world_id);
	}

	const dWorldID world()
	{
		return m_world_id;
	}

	const dSpaceID space()
	{
		return m_space_id;
	}

	sigc::connection connect_simulation_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_simulation_changed_signal.connect(Slot);
	}

	sigc::connection connect_initial_state_signal(const sigc::slot<void>& Slot)
	{
		return m_initial_state_signal.connect(Slot);
	}

	sigc::connection connect_update_state_signal(const sigc::slot<void>& Slot)
	{
		return m_update_state_signal.connect(Slot);
	}

	void simulation_changed()
	{
		m_modified = true;
		m_simulation_changed_signal.emit();
	}

	void update()
	{
		if(!m_modified)
			return;

		m_modified = false;

		const k3d::double_t current_time = m_time.pipeline_value();
		const k3d::double_t delta_time = current_time - m_last_time;
		m_last_time = current_time;

		if(delta_time > 0.0)
		{
			// Setup the world state ...
			const k3d::vector3 gravity = m_gravity.pipeline_value();
			dWorldSetGravity(m_world_id, gravity[0], gravity[1], gravity[2]);

			// Set body states ...
			m_update_state_signal.emit();

			// Detect collisions ...
			dSpaceCollide(m_space_id, this, raw_collision_callback);

			// Step the simulation ...
			dWorldStep(m_world_id, delta_time);

			// Reset collisions ...
			dJointGroupEmpty(m_contact_joint_group_id);
		}
		else
		{
			// Set initial body states ...
			m_initial_state_signal.emit();
		}
	}

	void time_changed(k3d::ihint*)
	{
		m_modified = true;
		m_simulation_changed_signal.emit();
	}

	static void raw_collision_callback(void* Data, dGeomID Geometry1, dGeomID Geometry2)
	{
		reinterpret_cast<simulation*>(Data)->collision_callback(Geometry1, Geometry2);
	}
	
	void collision_callback(dGeomID Geometry1, dGeomID Geometry2)
	{
		const int max_contact_count = 64;
		dContact contacts[max_contact_count];

		dContact* const contact_begin = contacts;
		dContact* const contact_end = contact_begin + dCollide(Geometry1, Geometry2, max_contact_count, &contacts[0].geom, sizeof(dContact));
		for(dContact* contact = contact_begin; contact != contact_end; ++contact)
		{
//			contact->surface.mode = dContactBounce | dContactSoftCFM;
			contact->surface.mode = 0;
//			contact->surface.mu = dInfinity;
			contact->surface.mu = 0;
			contact->surface.mu2 = 0;
			contact->surface.bounce = 0.0;
			contact->surface.bounce_vel = 0.0;
			contact->surface.soft_cfm = 0.00;

			const dJointID joint_id = dJointCreateContact(m_world_id, m_contact_joint_group_id, contact);
			dJointAttach(joint_id, dGeomGetBody(Geometry1), dGeomGetBody(Geometry2));
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<simulation,
			k3d::interface_list<k3d::itime_sink> > factory(
				k3d::uuid(0x0df159e6, 0x4d49063c, 0x391217b5, 0x8079599e),
				"ODESimulation",
				_("Rigid-body dynamics simulation."),
				"ODE",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	dWorldID m_world_id;
	dSpaceID m_space_id;
	dGeomID m_plane_id;
	dJointGroupID m_contact_joint_group_id;

	k3d::bool_t m_modified;
	sigc::signal<void> m_simulation_changed_signal;
	sigc::signal<void> m_initial_state_signal;
	sigc::signal<void> m_update_state_signal;

	k3d::double_t m_last_time;

	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gravity;
};

/////////////////////////////////////////////////////////////////////////////
// simulation_factory

k3d::iplugin_factory& simulation_factory()
{
	return simulation::get_factory();
}

} // namespace ode

} // namespace module

