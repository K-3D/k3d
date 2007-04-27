// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include <k3dsdk/algebra.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/new_mesh_simple_deformation_modifier.h>

namespace libk3ddeformation
{

/////////////////////////////////////////////////////////////////////////////
// twist_points

class twist_points :
	public k3d::dev::mesh_simple_deformation_modifier
{
	typedef k3d::dev::mesh_simple_deformation_modifier base;

public:
	twist_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_axis(init_owner(*this) + init_name("axis") + init_label(_("Axis")) + init_description(_("Twist around given axis")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values())),
		m_angle(init_owner(*this) + init_name("angle") + init_label(_("Angle")) + init_description(_("Twist angle")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_axis.changed_signal().connect(make_update_mesh_slot());
		m_angle.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::dev::mesh::points_t& InputPoints, const k3d::dev::mesh::selection_t& PointSelection, k3d::dev::mesh::points_t& OutputPoints)
	{
		const k3d::bounding_box3 bounds = k3d::dev::bounds(InputPoints);

		const k3d::axis axis = m_axis.value();
		const double angle = m_angle.value();

		double size = 0.0;
		k3d::point3 angles;
		switch(axis)
		{
			case k3d::X:
				size = std::max(std::abs(bounds.px), std::abs(bounds.nx));
				angles = k3d::point3(angle, 0, 0);
				break;
			case k3d::Y:
				size = std::max(std::abs(bounds.py), std::abs(bounds.ny));
				angles = k3d::point3(0, angle, 0);
				break;
			case k3d::Z:
				size = std::max(std::abs(bounds.pz), std::abs(bounds.nz));
				angles = k3d::point3(0, 0, angle);
				break;
			default:
				assert_not_reached();
		}
		if(0.0 == size)
			return;

		const size_t point_begin = 0;
		const size_t point_end = point_begin + OutputPoints.size();
		for(size_t point = point_begin; point != point_end; ++point)
		{
			const double twist = InputPoints[point][axis] / size;
            OutputPoints[point] = k3d::mix(InputPoints[point], k3d::rotation3D(angles * twist) * InputPoints[point], PointSelection[point]);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<twist_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x76051829, 0x66b54dbf, 0xb67d7719, 0xb2e2596f),
				"TwistPoints",
				_("Twists mesh points along an axis"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_axis;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_angle;
};

/////////////////////////////////////////////////////////////////////////////
// twist_points_factory

k3d::iplugin_factory& twist_points_factory()
{
	return twist_points::get_factory();
}

} // namespace libk3ddeformation

