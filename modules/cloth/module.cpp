
// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
	\author Carlos Andres Dominguez Caballero (carlosadc at gmail dot com)
*/


#include <k3dsdk/module.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_deformation_modifier.h>

#include "d_Vec3f.h"
#include "cloth_solver.h"

namespace module
{

namespace cloth
{

class cloth_plugin :
	public k3d::mesh_deformation_modifier
{
	typedef k3d::mesh_deformation_modifier base;
public:
	cloth_plugin(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_time(init_owner(*this) + init_name("time") + init_label(_("Time")) + init_description(_("Controls the current time displayed in the viewports.")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::time))),
		m_damping(init_owner(*this) + init_name("damping") + init_label(_("Damping")) + init_description(_("Damping of cloth")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_gravity(init_owner(*this) + init_name("gravity") + init_label(_("Gravity")) + init_description(_("Gravity to affect the system")) + init_value(-9.81) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_stiffness(init_owner(*this) + init_name("stiffness") + init_label(_("Stiffness")) + init_description(_("Stiffness of cloth (k constant for spring structure)")) + init_value(20) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_time.changed_signal().connect(make_update_mesh_slot());
		k3d::log() << debug << "ClothModifier plug-in started" << std::endl;
		first_time = true;
	}

	~cloth_plugin()
	{
		k3d::log() << debug << "ClothModifier plug-in terminated" << std::endl;
	}

	void on_deform_mesh(const k3d::mesh& InputMesh, const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const double damping = m_damping.pipeline_value();
		const double gravity = m_gravity.pipeline_value();
		const double stiffness = m_stiffness.pipeline_value();
		//const k3d::mesh::indices_t& face_first_loops = *InputMesh.polyhedra->face_first_loops;
		//const k3d::mesh::indices_t& loop_first_edges = *InputMesh.polyhedra->loop_first_edges;
		//const k3d::mesh::indices_t& edge_points = *InputMesh.polyhedra->edge_points;

		if(first_time)
		{
			solver = new cloth_solver(36);
			solver->generate_particles(InputPoints);
			first_time = false;
		}		
		else
		{	
			solver->set_constants(gravity, damping, stiffness);

			for(int i=0; i<10; i++)
				solver->rk_step(0.003, PointSelection);
		}
		
		solver->update_vertices(OutputPoints);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cloth_plugin,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xd6a72aa4, 0x9e426c45, 0x2429eaab, 0x634a2ff8),
				"ClothModifier",
				_("Cloth Simulation Modifier(currently for grids only)"),
				"ClothSimulation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_time;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_damping;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_gravity;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_stiffness;
	cloth_solver *solver;
	bool first_time;
};

} // namespace cloth

} // namespace module

K3D_MODULE_START(Registry)
        Registry.register_factory(module::cloth::cloth_plugin::get_factory());
K3D_MODULE_END


