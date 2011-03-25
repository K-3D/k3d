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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// set_position

class set_position :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	set_position(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X")) + init_description(_("X position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y")) + init_description(_("Y position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z(init_owner(*this) + init_name("z") + init_label(_("Z")) + init_description(_("Z position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_use_x(init_owner(*this) + init_name("use_x") + init_label(_("Use X")) + init_description(_("Set position on X components")) + init_value(true)),
		m_use_y(init_owner(*this) + init_name("use_y") + init_label(_("Use Y")) + init_description(_("Set position on Y components")) + init_value(true)),
		m_use_z(init_owner(*this) + init_name("use_z") + init_label(_("Use Z")) + init_description(_("Set position on Z components")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_x.changed_signal().connect(make_update_mesh_slot());
		m_y.changed_signal().connect(make_update_mesh_slot());
		m_z.changed_signal().connect(make_update_mesh_slot());
		m_use_x.changed_signal().connect(make_update_mesh_slot());
		m_use_y.changed_signal().connect(make_update_mesh_slot());
		m_use_z.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::uint_t points_begin = 0;
		const k3d::uint_t points_end = InputPoints.size();
		const k3d::bool_t use_x = m_use_x.pipeline_value();
		const k3d::bool_t use_y = m_use_y.pipeline_value();
		const k3d::bool_t use_z = m_use_z.pipeline_value();
		for(k3d::uint_t point_idx = points_begin; point_idx != points_end; ++point_idx)
		{
			k3d::point3& point = OutputPoints[point_idx];
			point = InputPoints[point_idx];
			if(PointSelection[point_idx])
			{
				if(use_x)
					point[0] = m_x.pipeline_value();
				if(use_y)
					point[1] = m_y.pipeline_value();
				if(use_z)
					point[2] = m_z.pipeline_value();
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<set_position,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xa01212cf, 0x4cb34ab4, 0x879b76dd, 0x8e861efa),
				"SetPosition",
				_("Set points positions"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_use_x;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_use_y;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_use_z;
};

/////////////////////////////////////////////////////////////////////////////
// set_position_factory

k3d::iplugin_factory& set_position_factory()
{
	return set_position::get_factory();
}

} // namespace deformation

} // namespace module


