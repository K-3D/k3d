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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include "linear_transformation_worker.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// center_points

class center_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	center_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_center_x(init_owner(*this) + init_name("center_x") + init_label(_("Center X")) + init_description(_("Center X components around origin")) + init_value(true)),
		m_center_y(init_owner(*this) + init_name("center_y") + init_label(_("Center Y")) + init_description(_("Center Y components around origin")) + init_value(true)),
		m_center_z(init_owner(*this) + init_name("center_z") + init_label(_("Center Z")) + init_description(_("Center Z components around origin")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_center_x.changed_signal().connect(make_update_mesh_slot());
		m_center_y.changed_signal().connect(make_update_mesh_slot());
		m_center_z.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::bounding_box3 bounds = k3d::bounds(InputPoints);
		if(bounds.empty())
			return;

		const bool center_x = m_center_x.pipeline_value();
		const bool center_y = m_center_y.pipeline_value();
		const bool center_z = m_center_z.pipeline_value();

		const k3d::matrix4 transformation = k3d::translation3D(k3d::vector3(
			center_x ? -0.5 * (bounds.px + bounds.nx) : 0,
			center_y ? -0.5 * (bounds.py + bounds.ny) : 0,
			center_z ? -0.5 * (bounds.pz + bounds.nz) : 0));

		k3d::parallel::parallel_for(
			k3d::parallel::blocked_range<k3d::uint_t>(0, OutputPoints.size(), k3d::parallel::grain_size()),
			linear_transformation_worker(InputPoints, PointSelection, OutputPoints, transformation));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<center_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xc030994f, 0x97ed4165, 0x9ac0b7f0, 0xa777abbc),
				"CenterPoints",
				_("Centers mesh points around the origin"),
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_center_x;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_center_y;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_center_z;
};

/////////////////////////////////////////////////////////////////////////////
// center_points_factory

k3d::iplugin_factory& center_points_factory()
{
	return center_points::get_factory();
}

} // namespace deformation

} // namespace module

