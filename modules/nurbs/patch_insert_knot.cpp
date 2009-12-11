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
	\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include "nurbs_patches.h"
#include "utility.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{

class patch_insert_knot :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	patch_insert_knot(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_value(init_owner(*this) + init_name(_("u_value")) + init_label(_("u/v value of new knot")) + init_description(_("Insert knot at [0,1]")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.5)),
		m_multiplicity(init_owner(*this) + init_name(_("multiplicity")) + init_label(_("Multiplicity")) + init_description(_("Multiplicity")) + init_constraint(constraint::minimum(1 , constraint::maximum(3))) + init_value(1)),
		m_insert_to_v(init_owner(*this) + init_name(_("insert_to_v")) + init_label(_("Insert knot in v? Otherwise u")) + init_description(_("By default the knot gets inserted into the patch in u direction, by checking this box you choose to insert it to v")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_u_value.changed_signal().connect(make_update_mesh_slot());
		m_multiplicity.changed_signal().connect(make_update_mesh_slot());
		m_insert_to_v.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);
		modify_selected_patches(Input, Output, knot_inserter(m_u_value.pipeline_value(), m_multiplicity.pipeline_value(), !m_insert_to_v.pipeline_value()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<patch_insert_knot, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xc250f185, 0xff4ac282, 0xd9b60c8a, 0x15c58b1c),
		  "NurbsPatchInsertKnot",
		  _("Adds a new knot in either u or v direction to all selected patches"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	struct knot_inserter
	{
		knot_inserter(const k3d::double_t Parameter, const k3d::uint_t Multiplicity, const k3d::bool_t UDirection) : parameter(Parameter), multiplicity(Multiplicity), u_direction(UDirection) {}
		void operator()(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t& Patch)
		{
			insert_knot(OutputMesh, OutputPatches, InputMesh, InputPatches, Patch, parameter, multiplicity, u_direction);
		}
		const k3d::double_t parameter;
		const k3d::uint_t multiplicity;
		const k3d::bool_t u_direction;
	};

	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_value;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_multiplicity;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_insert_to_v;
};

k3d::iplugin_factory& patch_insert_knot_factory()
{
	return patch_insert_knot::get_factory();
}

} //namespace nurbs

} //namespace module

