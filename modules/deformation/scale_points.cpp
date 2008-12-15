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
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// scale_points

class scale_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	scale_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_x(init_owner(*this) + init_name("x") + init_label(_("X")) + init_description(_("X scale")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_y(init_owner(*this) + init_name("y") + init_label(_("Y")) + init_description(_("Y scale")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_z(init_owner(*this) + init_name("z") + init_label(_("Z")) + init_description(_("Z scale")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_x.changed_signal().connect(make_update_mesh_slot());
		m_y.changed_signal().connect(make_update_mesh_slot());
		m_z.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::matrix4 transformation = k3d::scale3(m_x.pipeline_value(), m_y.pipeline_value(), m_z.pipeline_value());

		k3d::parallel::parallel_for(
			k3d::parallel::blocked_range<k3d::uint_t>(0, OutputPoints.size(), k3d::parallel::grain_size()),
			linear_transformation_worker(InputPoints, PointSelection, OutputPoints, transformation));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scale_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xd3829136, 0x1f934c4d, 0x89151994, 0xa49d9f65),
				"ScalePoints",
				_("Scales mesh points"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z;
};

/////////////////////////////////////////////////////////////////////////////
// scale_points_factory

k3d::iplugin_factory& scale_points_factory()
{
	return scale_points::get_factory();
}

} // namespace deformation

} // namespace module


