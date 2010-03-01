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
	\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include "nurbs_curves.h"
#include "nurbs_patches.h"
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

class create_cap :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	create_cap(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the curve which was used to create the cap")) + init_value(false)),
		m_u_offset(init_owner(*this) + init_name(_("u_offset")) + init_label(_("U Offset")) + init_description(_("Shift the capped curve U range by this amount, useful to rotate the UV-system on the cap")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.0)),
		m_coons(init_owner(*this) + init_name(_("coons")) + init_label(_("Coons")) + init_description(_("Use a Coons surface to fill the curve if true, use a trimmed bilinear patch otherwise")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
		m_u_offset.changed_signal().connect(make_update_mesh_slot());
		m_coons.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		if(!Output.points.get())
			return;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);
		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(Output));

		k3d::mesh selected_curves_mesh;
		selected_curves_mesh.points.create();
		selected_curves_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> selected_curves(k3d::nurbs_curve::create(selected_curves_mesh));
		selected_curves->material.push_back(0);
		visit_selected_curves(Output, selected_curve_extractor(selected_curves_mesh, *selected_curves));

		// Eliminate duplicate points
		replace_duplicate_points(selected_curves_mesh);

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(selected_curves_mesh, unused_points);
		k3d::mesh::delete_points(selected_curves_mesh, unused_points);

		// Merge all curves that share endpoints into a single curve
		k3d::mesh merged_mesh;
		merged_mesh.points.create();
		merged_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> merged_curves(k3d::nurbs_curve::create(merged_mesh));
		merge_connected_curves(merged_mesh, *merged_curves, selected_curves_mesh, 0);

		// Eliminate duplicate points again, closing any loops
		replace_duplicate_points(merged_mesh);
		k3d::mesh::lookup_unused_points(merged_mesh, unused_points);
		k3d::mesh::delete_points(merged_mesh, unused_points);

		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_merged_curves(k3d::nurbs_curve::validate(merged_mesh, *merged_mesh.primitives.front()));

		if(m_coons.pipeline_value())
		{
			// Split all loops into 4 curves, and apply the Coons patch algorithm to create the caps
			for(k3d::uint_t curve = 0; curve != merged_curves->curve_first_points.size(); ++curve)
			{
				if(!is_closed(*const_merged_curves, curve))
					continue;
				// Split the curve in half
				k3d::mesh halved_mesh;
				halved_mesh.points.create();
				halved_mesh.point_selection.create();
				boost::scoped_ptr<k3d::nurbs_curve::primitive> halved_curves(k3d::nurbs_curve::create(halved_mesh));
				// This opens the curve
				split_curve(halved_mesh, *halved_curves, merged_mesh, *const_merged_curves, curve, m_u_offset.pipeline_value());
				boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_halved_curves(k3d::nurbs_curve::validate(halved_mesh, *halved_mesh.primitives.front()));
				// Now we split it in two
				split_curve(halved_mesh, *halved_curves, halved_mesh, *const_halved_curves, 0, 0.5);
				return_if_fail(halved_curves->curve_first_points.size() == 3);
				// Split both new curves
				k3d::mesh quartered_mesh;
				quartered_mesh.points.create();
				quartered_mesh.point_selection.create();
				boost::scoped_ptr<k3d::nurbs_curve::primitive> quartered_curves(k3d::nurbs_curve::create(quartered_mesh));
				split_curve(quartered_mesh, *quartered_curves, halved_mesh, *const_halved_curves, 1, 0.5);
				split_curve(quartered_mesh, *quartered_curves, halved_mesh, *const_halved_curves, 2, 0.5);

				// Make sure the endpoints of the split curves are shared
				replace_duplicate_points(quartered_mesh);
				k3d::mesh::bools_t unused_points;
				k3d::mesh::lookup_unused_points(quartered_mesh, unused_points);
				k3d::mesh::delete_points(quartered_mesh, unused_points);

				// Apply coons
				boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_quartered_curves(k3d::nurbs_curve::validate(quartered_mesh, *quartered_mesh.primitives.front()));
				coons_patch(Output, *output_patches, quartered_mesh, *const_quartered_curves);
			}
		}
		else // no coons requested, use a trimmed bilinear patch instead
		{
			for(k3d::uint_t curve = 0; curve != merged_curves->curve_first_points.size(); ++curve)
			{
				if(!is_closed(*const_merged_curves, curve))
					continue;
				k3d::point3 origin;
				k3d::normal3 normal;
				k3d::vector3 u_axis;
				k3d::vector3 v_axis;
				k3d::mesh::points_2d_t parameter_points;
				k3d::mesh::points_t points;
				k3d::mesh::weights_t weights;
				curve_arrays merged_arrays(merged_mesh, *const_merged_curves, curve, true);
				merged_arrays.points3(points);
				merged_arrays.weights(weights);
				// Get the plane the curve lies in
				extract_plane(origin, normal, u_axis, v_axis, parameter_points, points, m_u_offset.pipeline_value());
				// Get the bounding box for the curve points in this plane
				k3d::point2 bbmin, bbmax;
				bbox2(bbmin, bbmax, parameter_points);
				const k3d::double_t u_offset = -bbmin[0];
				const k3d::double_t v_offset = -bbmin[1];
				const k3d::double_t u_scale = u_axis.length() / (bbmax[0] - bbmin[0]);
				const k3d::double_t v_scale = v_axis.length() / (bbmax[1] - bbmin[1]);
				patch_point_data bilinear_point_data;
				bilinear_point_data.points.push_back(origin + bbmin[0] * u_axis + bbmin[1] * v_axis);
				bilinear_point_data.points.push_back(origin + bbmax[0] * u_axis + bbmin[1] * v_axis);
				bilinear_point_data.points.push_back(origin + bbmin[0] * u_axis + bbmax[1] * v_axis);
				bilinear_point_data.points.push_back(origin + bbmax[0] * u_axis + bbmax[1] * v_axis);
				bilinear_point_data.weights.resize(4, 1.0);
				k3d::table patch_attributes = merged_curves->curve_attributes.clone(curve, curve+1);
				k3d::table parameter_attributes = merged_curves->parameter_attributes.clone(2*curve, 2*curve+2);
				k3d::table_copier parameter_copier(merged_curves->parameter_attributes, parameter_attributes);
				parameter_copier.push_back(2*curve);
				parameter_copier.push_back(2*curve+1);
				create_bilinear_patch(Output, *output_patches, bilinear_point_data, patch_attributes, parameter_attributes);
				output_patches->patch_materials.push_back(merged_curves->material.back());
				add_trim_curve(*output_patches, output_patches->patch_first_points.size()-1, parameter_points, weights, merged_arrays.knots, merged_curves->curve_orders[curve], u_offset, v_offset, u_scale, v_scale);
			}
		}

		if(m_delete_original.pipeline_value())
		{
			delete_selected_curves(Output);
		}
		delete_empty_primitives(Output);
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<create_cap, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x3f8d9448, 0x5d442835, 0x7d4d0f87, 0x4c0e0593),
		  "NurbsCreateCap",
		  _("Creates a NURBS surface filling out the selected closed curves"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_offset;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_coons;
};

//Create connect_curve factory
k3d::iplugin_factory& create_cap_factory()
{
	return create_cap::get_factory();
}

}//namespace nurbs
} //namespace module

