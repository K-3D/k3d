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

#include "nurbs_curves.h"
#include "utility.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace nurbs
{

class approximate_test :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	approximate_test(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_order(init_owner(*this) + init_name("order") + init_label(_("Order")) + init_description(_("Order of the curve (2-linear - 4-cubic)")) + init_value(3) + init_constraint(constraint::minimum(2)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_point_count(init_owner(*this) + init_name("point_count") + init_label(_("Point Count")) + init_description(_("Number of control points")) + init_value(4) + init_constraint(constraint::minimum(2)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_point_spacing(init_owner(*this) + init_name("point_spacing") + init_label(_("Point Spacing")) + init_description(_("Space between points")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_order.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_point_count.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_point_spacing.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();
		Output.points.create();
		Output.point_selection.create();

		const k3d::uint_t order = 3;

		k3d::mesh::knots_t knots;
		k3d::nurbs_curve::add_open_uniform_knots(order, order+1, knots);

		k3d::mesh::points_t orig_points;
		orig_points.push_back(k3d::point3(0,0,0));
		orig_points.push_back(k3d::point3(2.0/3.0,0,-1.0));
		orig_points.push_back(k3d::point3(4.0/3.0,0,1.0));
		orig_points.push_back(k3d::point3(2,0,0));
		k3d::mesh::weights_t orig_weights(orig_points.size(), 1);
		orig_weights[2] = 5.0;

		k3d::mesh::knots_t sample_params;
		points4_t samples;
		for(k3d::double_t u = 0; u < knots.back(); u += 0.1)
		{
			sample_params.push_back(u);
			samples.push_back(evaluate_position(orig_points, orig_weights, knots, u));
		}
		sample_params.push_back(knots.back());
		samples.push_back(evaluate_position(orig_points, orig_weights, knots, knots.back()));

		k3d::mesh::points_t points;
		k3d::mesh::weights_t weights;
		approximate(points, weights, sample_params, samples, order, knots);
		boost::scoped_ptr<k3d::nurbs_curve::primitive> output_curves(k3d::nurbs_curve::create(Output));
		k3d::nurbs_curve::add_curve(Output, *output_curves, order, points, weights, knots);
		output_curves->material.push_back(0);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<approximate_test, k3d::interface_list<k3d::imesh_source > > factory(
		  k3d::uuid(0x1959c1d8, 0x444f7fdc, 0xc242cba2, 0x277b1ee7),
		  "NurbsApproximateTest",
		  _("Temporary test plugin for the NURBS curve approximation algorithm"),
		  "NURBS",
		  k3d::iplugin_factory::DEPRECATED);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_order;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_point_count;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_point_spacing;
};

/////////////////////////////////////////////////////////////////////////////
// approximate_test_factory

k3d::iplugin_factory& approximate_test_factory()
{
	return approximate_test::get_factory();
}

} // namespace nurbs

} // namespace module

