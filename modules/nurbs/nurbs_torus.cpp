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
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/persistent.h>

namespace libk3dnurbs
{

/////////////////////////////////////////////////////////////////////////////
// nurbs_torus

class nurbs_torus :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	nurbs_torus(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_majorradius(init_owner(*this) + init_name("majorradius") + init_label(_("majorradius")) + init_description(_("Major Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_minorradius(init_owner(*this) + init_name("minorradius") + init_label(_("minorradius")) + init_description(_("Minor Radius")) + init_value(2.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_phimin(init_owner(*this) + init_name("phimin") + init_label(_("phimin")) + init_description(_("Phi Minimum")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_phimax(init_owner(*this) + init_name("phimax") + init_label(_("phimax")) + init_description(_("Phi Maximum")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("thetamax")) + init_description(_("Theta Maximum")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("U Segments")) + init_value(4) + init_constraint(constraint::minimum(3L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("v_segments")) + init_description(_("V Segments")) + init_value(4) + init_constraint(constraint::minimum(3L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_majorradius.changed_signal().connect(make_reset_mesh_slot());
		m_minorradius.changed_signal().connect(make_reset_mesh_slot());
		m_phimin.changed_signal().connect(make_reset_mesh_slot());
		m_phimax.changed_signal().connect(make_reset_mesh_slot());
		m_thetamax.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double majorradius = m_majorradius.pipeline_value();
		const double minorradius = m_minorradius.pipeline_value();
		const double phimin = m_phimin.pipeline_value();
		const double phimax = m_phimax.pipeline_value();
		const double thetamax = m_thetamax.pipeline_value();
		const unsigned long u_segments = m_u_segments.pipeline_value();
		const unsigned long v_segments = m_v_segments.pipeline_value();

		k3d::legacy::nupatch* const nupatch = new k3d::legacy::nupatch();
		Mesh.nupatches.push_back(nupatch);
		
		nupatch->material = m_material.pipeline_value();
		nupatch->u_order = 3;
		nupatch->v_order = 3;

		std::vector<double> v_weights;
		std::vector<k3d::point3> v_arc_points;
		k3d::nurbs::circular_arc(k3d::point3(0, 1, 0), k3d::point3(0, 0, 1), phimin, phimax, v_segments, nupatch->v_knots, v_weights, v_arc_points);

		std::vector<double> u_weights;
		std::vector<k3d::point3> u_arc_points;
		k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), 0, thetamax, u_segments, nupatch->u_knots, u_weights, u_arc_points);

		for(unsigned long v = 0; v != v_arc_points.size(); ++v)
		{
			const k3d::point3 offset = minorradius * v_arc_points[v][2] * k3d::point3(0, 0, 1);
			const double radius2 = majorradius + (minorradius * v_arc_points[v][1]);
			const double v_weight = v_weights[v];

			for(unsigned long u = 0; u != u_arc_points.size(); ++u)
			{
				Mesh.points.push_back(new k3d::legacy::point(radius2 * u_arc_points[u] + offset));
				nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(Mesh.points.back(), v_weight * u_weights[u]));
			}
		}

		assert_warning(is_valid(*nupatch));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_torus, k3d::interface_list<k3d::imesh_source > > factory(
		k3d::uuid(0xc6cb880c, 0x4e4d4028, 0x8c77305f, 0x4b8f05a2),
			"NurbsTorus",
			_("Generates a NURBS torus"),
			"NURBS",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_majorradius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_minorradius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_phimin;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_phimax;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_torus_factory

k3d::iplugin_factory& nurbs_torus_factory()
{
	return nurbs_torus::get_factory();
}

} // namespace libk3dnurbs

