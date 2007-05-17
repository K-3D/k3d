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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace libk3ddeformation
{

/////////////////////////////////////////////////////////////////////////////
// sphereize_points

class sphereize_points :
	public k3d::dev::mesh_simple_deformation_modifier
{
	typedef k3d::dev::mesh_simple_deformation_modifier base;

public:
	sphereize_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sphere_factor(init_owner(*this) + init_name("sphere_factor") + init_label(_("Sphere factor")) + init_description(_("Sphere factor")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_sphere_factor.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::dev::mesh::points_t& InputPoints, const k3d::dev::mesh::selection_t& PointSelection, k3d::dev::mesh::points_t& OutputPoints)
	{
		const double sphere_factor = m_sphere_factor.value();

		const size_t point_begin = 0;
		const size_t point_end = point_begin + OutputPoints.size();

		double target_radius = 0.0;
		for(size_t point = point_begin; point != point_end; ++point)
			target_radius = std::max(target_radius, k3d::length(k3d::to_vector(InputPoints[point])));
		if(0.0 == target_radius)
			return;

		for(size_t point = point_begin; point != point_end; ++point)
		{
			double radius = k3d::length(k3d::to_vector(InputPoints[point]));
			if(0.0 == radius)
				radius = target_radius;

			OutputPoints[point] = k3d::mix(InputPoints[point], InputPoints[point] * k3d::mix(radius, target_radius, sphere_factor) / radius, PointSelection[point]);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sphereize_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x643a4bdd, 0x191e4297, 0xa9b600fc, 0x699b46f9),
				"SphereizePoints",
				_("Constrains points to the surface of a sphere"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_sphere_factor;
};

/////////////////////////////////////////////////////////////////////////////
// sphereize_points_factory

k3d::iplugin_factory& sphereize_points_factory()
{
	return sphereize_points::get_factory();
}

} // namespace libk3ddeformation

