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
#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// shear_points

class shear_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	shear_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_direction(init_owner(*this) + init_name("direction") + init_label(_("Direction")) + init_description(_("Shear direction")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values())),
		m_axis(init_owner(*this) + init_name("axis") + init_label(_("Axis")) + init_description(_("Shear axis")) + init_value(k3d::X) + init_enumeration(k3d::axis_values())),
		m_shear_factor(init_owner(*this) + init_name("shear_factor") + init_label(_("Shear factor")) + init_description(_("Shear factor")) + init_value(0.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_direction.changed_signal().connect(make_update_mesh_slot());
		m_axis.changed_signal().connect(make_update_mesh_slot());
		m_shear_factor.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::axis direction = m_direction.pipeline_value();
		const k3d::axis axis = m_axis.pipeline_value();
		const double shear_factor = m_shear_factor.pipeline_value();

		const double xy = k3d::X == direction && k3d::Y == axis ? shear_factor : 0;
		const double xz = k3d::X == direction && k3d::Z == axis ? shear_factor : 0;
		const double yx = k3d::Y == direction && k3d::X == axis ? shear_factor : 0;
		const double yz = k3d::Y == direction && k3d::Z == axis ? shear_factor : 0;
		const double zx = k3d::Z == direction && k3d::X == axis ? shear_factor : 0;
		const double zy = k3d::Z == direction && k3d::Y == axis ? shear_factor : 0;

		const k3d::matrix4 transformation = k3d::shear3(xy, xz, yx, yz, zx, zy);

		k3d::parallel::parallel_for(
			k3d::parallel::blocked_range<k3d::uint_t>(0, OutputPoints.size(), k3d::parallel::grain_size()),
			linear_transformation_worker(InputPoints, PointSelection, OutputPoints, transformation));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<shear_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x0def4291, 0x51ce479b, 0xadf9aec8, 0xed522ccc),
				"ShearPoints",
				_("Shears mesh points along an axis"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_direction;
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_axis;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_shear_factor;
};

/////////////////////////////////////////////////////////////////////////////
// shear_points_factory

k3d::iplugin_factory& shear_points_factory()
{
	return shear_points::get_factory();
}

} // namespace deformation

} // namespace module


