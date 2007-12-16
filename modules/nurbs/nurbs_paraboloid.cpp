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
		\author Tim Shead (tshead@k-3d.com)
*/
#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/persistent.h>

namespace libk3dnurbs
{

/////////////////////////////////////////////////////////////////////////////
// nurbs_paraboloid

class nurbs_paraboloid :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	nurbs_paraboloid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmax(init_owner(*this) + init_name("zmax") + init_label(_("Z max")) + init_description(_("Maximum point position")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmin(init_owner(*this) + init_name("zmin") + init_label(_("Z min")) + init_description(_("Origin position")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("Theta max")) + init_description(_("From RenderMan specification")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_zmax.changed_signal().connect(make_reset_mesh_slot());
		m_zmin.changed_signal().connect(make_reset_mesh_slot());
		m_thetamax.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double radius = m_radius.pipeline_value();
		const double zmax = m_zmax.pipeline_value();
		const double zmin = std::max(0.0, std::min(zmax, m_zmin.pipeline_value()));
		const double thetamax = m_thetamax.pipeline_value();

		const double z1 = zmax ? zmin / zmax : 0.0;
		const double z3 = 1.0;
		
		const double r1 = std::sqrt(z1);
		const double r3 = 1.0;
	
		const k3d::point3 p1(r1, z1, 0);
		const k3d::point3 p3(r3, z3, 0);

		const k3d::vector3 t1(1, 2 * std::sqrt(z1), 0);
		const k3d::vector3 t3(1, 2, 0);

		k3d::point3 p2;
		k3d::intersect_lines(p1, t1, p3, t3, p2);

		const double z2 = p2[1];
		const double r2 = p2[0];
		
		k3d::legacy::nupatch* const nupatch = new k3d::legacy::nupatch();
		Mesh.nupatches.push_back(nupatch);
		
		nupatch->material = m_material.pipeline_value();
		nupatch->u_order = 3;
		nupatch->v_order = 3;

		std::vector<double> weights;
		std::vector<k3d::point3> arc_points;
		k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), 0, thetamax, 4, nupatch->u_knots, weights, arc_points);
		
		nupatch->v_knots.insert(nupatch->v_knots.end(), 3, 0);
		nupatch->v_knots.insert(nupatch->v_knots.end(), 3, 1);

		for(unsigned long j = 0; j != arc_points.size(); ++j)
		{
			Mesh.points.push_back(new k3d::legacy::point(r1 * radius * arc_points[j] + z1 * zmax * k3d::point3(0, 0, 1)));
			nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(Mesh.points.back(), weights[j]));
		}
		
		for(unsigned long j = 0; j != arc_points.size(); ++j)
		{
			Mesh.points.push_back(new k3d::legacy::point(r2 * radius * arc_points[j] + z2 * zmax * k3d::point3(0, 0, 1)));
			nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(Mesh.points.back(), weights[j]));
		}

		for(unsigned long j = 0; j != arc_points.size(); ++j)
		{
			Mesh.points.push_back(new k3d::legacy::point(r3 * radius * arc_points[j] + z3 * zmax * k3d::point3(0, 0, 1)));
			nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(Mesh.points.back(), weights[j]));
		}

		assert_warning(is_valid(*nupatch));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_paraboloid, k3d::interface_list<k3d::imesh_source > > factory(
		k3d::uuid(0x571a61d0, 0x272349da, 0xb4460a4f, 0x025378a6),
			"NurbsParaboloid",
			_("Generates a NURBS paraboloid"),
			"NURBS",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmax;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmin;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_paraboloid_factory

k3d::iplugin_factory& nurbs_paraboloid_factory()
{
	return nurbs_paraboloid::get_factory();
}

} // namespace libk3dnurbs

