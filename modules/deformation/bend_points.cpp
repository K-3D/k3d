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
		\author Andy Gill (buzz@ucky.com)
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace libk3ddeformation
{

namespace detail
{

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// bend_points

class bend_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	bend_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_angle(init_owner(*this) + init_name("angle") + init_label(_("Angle")) + init_description(_("Bend angle")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_tightness(init_owner(*this) + init_name("tightness") + init_label(_("Tightness")) + init_description(_("Bend tightness")) + init_value(0.6) + init_constraint(constraint::minimum(0.0, constraint::maximum(1.0))) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_position(init_owner(*this) + init_name("position") + init_label(_("Position")) + init_description(_("Bend position")) + init_value(0.5) + init_constraint(constraint::minimum(0.0, constraint::maximum(1.0))) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_along(init_owner(*this) + init_name("along") + init_label(_("Along")) + init_description(_("Axis to bend along")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values())),
		m_around(init_owner(*this) + init_name("around") + init_label(_("Around")) + init_description(_("Axis to bend around")) + init_value(k3d::X) + init_enumeration(k3d::axis_values()))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_angle.changed_signal().connect(make_update_mesh_slot());
		m_tightness.changed_signal().connect(make_update_mesh_slot());
		m_position.changed_signal().connect(make_update_mesh_slot());
		m_along.changed_signal().connect(make_update_mesh_slot());
		m_around.changed_signal().connect(make_update_mesh_slot());
	}

	class bend
	{
	public:
		bend(const k3d::point3& Origin, const k3d::point3& Min, const k3d::point3& Max, const k3d::axis Along, const k3d::axis Around, const double BendAngle, const double Tightness, const double JointPosition) :
			m_Origin(Origin),
			m_Min(Min),
			m_Max(Max),
			m_Along(Along),
			m_Around(Around),
			m_BendAngle(BendAngle),
			m_Tightness(Tightness),
			m_JointPosition(JointPosition),
			m_Size(Max - Min),
			m_Joint(k3d::mix(Min, Max, JointPosition)),
			m_Radius(1.0 - Tightness),
			m_JLower(k3d::mix(Min, Max, JointPosition - m_Radius))
		{
		}

		const k3d::point3 operator()(k3d::point3 coords)
		{
			if(m_Along == m_Around || m_BendAngle == 0)
				return coords;

			double delta = 0;
			double length = 0;
			k3d::vector3 axis;
			k3d::point3 offset;
			k3d::point3 pivot = m_Origin;

			switch(m_Along)
			{
				case k3d::X :
					if(m_Size[0] != 0)
						delta = (coords[0] - m_Min[0]) / m_Size[0];

					length = m_Size[0];

					if(delta > m_JointPosition - m_Radius && delta < m_JointPosition + m_Radius && m_BendAngle != 0)
					{
						coords[0] = m_JLower[0];
						pivot[0] = m_JLower[0];
					}
					else
						m_Origin[0] = m_Joint[0];

					switch(m_Around)
					{
						case k3d::X :
							break;
						case k3d::Y :
							axis = k3d::vector3(0, 1, 0);
							offset = k3d::point3(0, 0, -1);
							break;
						case k3d::Z:
							axis = k3d::vector3(0, 0, 1);
							offset = k3d::point3(0, 1, 0);
							break;
					}

				break;

				case k3d::Y :
					if(m_Size[1])
						delta = (coords[1] - m_Min[1]) / m_Size[1];

					length = m_Size[1];

					if(delta > m_JointPosition - m_Radius && delta < m_JointPosition + m_Radius && m_BendAngle != 0)
					{
						coords[1] = m_JLower[1];
						pivot[1] = m_JLower[1];
					}
					else
						m_Origin[1] = m_Joint[1];

					switch(m_Around)
					{
						case k3d::X :
							axis = k3d::vector3(1, 0, 0);
							offset = k3d::point3(0, 0, 1);
							break;
						case k3d::Y :
							break;
						case k3d::Z :
							axis = k3d::vector3(0, 0, 1);
							offset = k3d::point3(-1, 0, 0);
							break;
					}
				break;

				case k3d::Z:
					if(m_Size[2])
						delta = (coords[2] - m_Min[2]) / m_Size[2];

					length = m_Size[2];

					if(delta > m_JointPosition - m_Radius && delta < m_JointPosition + m_Radius && m_BendAngle != 0)
					{
						coords[2] = m_JLower[2];
						pivot[2] = m_JLower[2];
					}
					else
						m_Origin[2] = m_Joint[2];

					switch(m_Around)
					{
						case k3d::X :
							axis = k3d::vector3(1, 0, 0);
							offset = k3d::point3(0, -1, 0);
							break;
						case k3d::Y :
							axis = k3d::vector3(0, 1, 0);
							offset = k3d::point3(1, 0, 0);
							break;
						case k3d::Z :
							break;
					}
				break;
			}

			if(delta > m_JointPosition - m_Radius && delta < m_JointPosition + m_Radius && m_BendAngle != 0)
			{
				double d = (delta - (m_JointPosition - m_Radius)) / (m_Radius * 2.0);
				double factor = m_Radius * length / tan(m_BendAngle * 0.5);
				k3d::matrix4 rotation = k3d::rotation3D(d * m_BendAngle, axis);
				offset = offset * factor;
				coords -= pivot + offset;
				coords = rotation * coords;
				coords += pivot + offset;
			}
			else if(delta >= m_JointPosition + m_Radius)
			{
				k3d::matrix4 rotation = k3d::rotation3D(m_BendAngle, axis);
				coords -= m_Origin;
				coords = rotation * coords;
				coords += m_Origin;
			}

			return coords;
		}

	private:
		k3d::point3 m_Origin;
		const k3d::point3 m_Min;
		const k3d::point3 m_Max;
		const k3d::axis m_Along;
		const k3d::axis m_Around;
		const double m_BendAngle;
		const double m_Tightness;
		const double m_JointPosition;
		const k3d::point3 m_Size;
		const k3d::point3 m_Joint;
		const double m_Radius;
		const k3d::point3 m_JLower;
	};

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::bounding_box3 bounds = k3d::bounds(InputPoints);

		const double angle = m_angle.value();
		const double tightness = m_tightness.value();
		const double position = m_position.value();
		const k3d::axis along = m_along.value();
		const k3d::axis around = m_around.value();

		bend deformation(k3d::point3(0, 0, 0), k3d::point3(bounds.nx, bounds.ny, bounds.nz), k3d::point3(bounds.px, bounds.py, bounds.pz), along, around, angle, tightness, position);

		const size_t point_begin = 0;
		const size_t point_end = point_begin + OutputPoints.size();
		for(size_t point = point_begin; point != point_end; ++point)
			OutputPoints[point] = k3d::mix(InputPoints[point], deformation(InputPoints[point]), PointSelection[point]);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bend_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xfd599b45, 0x20384cd3, 0xb0af634b, 0x9372888a),
				"BendPoints",
				_("Bends mesh points around an axis"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_angle;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_tightness;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_position;
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_along;
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_around;
};

/////////////////////////////////////////////////////////////////////////////
// bend_points_factory

k3d::iplugin_factory& bend_points_factory()
{
	return bend_points::get_factory();
}

} // namespace libk3ddeformation

