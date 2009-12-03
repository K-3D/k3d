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

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{

class approximate_test :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
		typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	approximate_test(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
				m_samples_per_span(init_owner(*this) + init_name("samples") + init_label(_("Samples")) + init_description(_("The number of samples per span")) + init_value(20) + init_constraint(constraint::minimum(1)) + init_step_increment(10) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_samples_per_span.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(k3d::geometry::selection::create(1), Output);
		if(!Output.points)
			return;
		k3d::mesh mesh;
		k3d::mesh::points_t& pts = mesh.points.create();
		k3d::mesh::selection_t& point_sel = mesh.point_selection.create();
		visit_selected_curves(Output, curve_approximator(mesh, m_samples_per_span.pipeline_value()));

		point_sel.resize(pts.size());
		Output = mesh;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<approximate_test, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x1959c1d8, 0x444f7fdc, 0xc242cba2, 0x277b1ee7),
		  "NurbsApproximateTest",
		  _("Temporary test plugin for the NURBS curve approximation algorithm"),
		  "NURBS",
		  k3d::iplugin_factory::DEPRECATED);

		return factory;
	}

private:
	struct curve_approximator
	{
		curve_approximator(k3d::mesh& Mesh, const k3d::int32_t Samples) : mesh(Mesh), samples_per_knot(Samples) {}
		k3d::mesh& mesh;
		const k3d::int32_t samples_per_knot;
		void operator()(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t& Curve)
		{
			k3d::mesh::points_t orig_points;
			k3d::mesh::weights_t orig_weights;
			k3d::mesh::knots_t orig_knots;
			const k3d::uint_t order = Curves.curve_orders[Curve];
			extract_curve_arrays(orig_points, orig_knots, orig_weights, Mesh, Curves, Curve, false);
			k3d::mesh::knots_t sample_params;
			points4_t samples;
			sample_params.push_back(orig_knots.front());
			samples.push_back(evaluate_position(orig_points, orig_weights, orig_knots, orig_knots.front()));
			for(k3d::uint_t i = 1; i != orig_knots.size(); ++i)
			{
				if(orig_knots[i] == orig_knots[i-1])
					continue;
				const k3d::double_t interval = (orig_knots[i] - orig_knots[i-1])/static_cast<k3d::double_t>(samples_per_knot);
				for(k3d::uint_t j = 0; j != samples_per_knot; ++j)
				{
					const k3d::double_t u = orig_knots[i-1] + j*interval;
					sample_params.push_back(u);
					samples.push_back(evaluate_position(orig_points, orig_weights, orig_knots, u));
				}
			}
			sample_params.push_back(orig_knots.back());
			samples.push_back(evaluate_position(orig_points, orig_weights, orig_knots, orig_knots.back()));
			return_if_fail(sample_params.size() == samples.size());
			k3d::mesh::points_t points;
			k3d::mesh::weights_t weights;
			approximate(points, weights, sample_params, samples, order, orig_knots);
			k3d::log() << "found weights: " << weights << std::endl;
			boost::scoped_ptr<k3d::nurbs_curve::primitive> output_curves(k3d::nurbs_curve::create(mesh));
			k3d::nurbs_curve::add_curve(mesh, *output_curves, order, points, weights, orig_knots);
			output_curves->material.push_back(0);
//			k3d::mesh::points_t& tmp = mesh.points.writable();
//			for(k3d::uint_t i = 0; i != samples.size(); ++i)
//			{
//				const k3d::point4& p = samples[i];
//				const k3d::double_t w = p[3];
//				tmp.push_back(k3d::point3(p[0]/w, p[1]/w, p[2]/w));
//			}
		}
	};
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_samples_per_span;
};

/////////////////////////////////////////////////////////////////////////////
// approximate_test_factory

k3d::iplugin_factory& approximate_test_factory()
{
	return approximate_test::get_factory();
}

} // namespace nurbs

} // namespace module

