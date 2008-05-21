// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
*/

#include "array_helpers.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

namespace libk3drendermanpainters
{

/////////////////////////////////////////////////////////////////////////////
// nurbs_patch_painter

class nurbs_patch_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	nurbs_patch_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	~nurbs_patch_painter()
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		if(!k3d::validate_nurbs_patches(Mesh))
			return;

		const k3d::mesh::indices_t& patch_first_points = *Mesh.nurbs_patches->patch_first_points;
		const k3d::mesh::counts_t& patch_u_point_counts = *Mesh.nurbs_patches->patch_u_point_counts;
		const k3d::mesh::counts_t& patch_v_point_counts = *Mesh.nurbs_patches->patch_v_point_counts;
		const k3d::mesh::orders_t& patch_u_orders = *Mesh.nurbs_patches->patch_u_orders;
		const k3d::mesh::orders_t& patch_v_orders = *Mesh.nurbs_patches->patch_v_orders;
		const k3d::mesh::indices_t& patch_u_first_knots = *Mesh.nurbs_patches->patch_u_first_knots;
		const k3d::mesh::indices_t& patch_v_first_knots = *Mesh.nurbs_patches->patch_v_first_knots;
		const k3d::mesh::materials_t& patch_materials = *Mesh.nurbs_patches->patch_materials;
		const k3d::mesh::named_arrays& constant_data = Mesh.nurbs_patches->constant_data;
		const k3d::mesh::named_arrays& uniform_data = Mesh.nurbs_patches->uniform_data;
		const k3d::mesh::indices_t& patch_points = *Mesh.nurbs_patches->patch_points;
		const k3d::mesh::weights_t& patch_point_weights = *Mesh.nurbs_patches->patch_point_weights;
		const k3d::mesh::knots_t& patch_u_knots = *Mesh.nurbs_patches->patch_u_knots;
		const k3d::mesh::knots_t& patch_v_knots = *Mesh.nurbs_patches->patch_v_knots;
		const k3d::mesh::named_arrays& varying_data = Mesh.nurbs_patches->varying_data;
		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::named_arrays& vertex_data = Mesh.vertex_data;

		const size_t patch_begin = 0;
		const size_t patch_end = patch_begin + patch_first_points.size();
		for(size_t patch = patch_begin; patch != patch_end; ++patch)
		{
			k3d::ri::reals u_knots(patch_u_knots.begin() + patch_u_first_knots[patch], patch_u_knots.begin() + patch_u_first_knots[patch] + patch_u_point_counts[patch] + patch_u_orders[patch]);
			k3d::ri::reals v_knots(patch_v_knots.begin() + patch_v_first_knots[patch], patch_v_knots.begin() + patch_v_first_knots[patch] + patch_v_point_counts[patch] + patch_v_orders[patch]);

			array_copier ri_constant_data;
			ri_constant_data.add_arrays(constant_data);

			array_copier ri_uniform_data;
			ri_uniform_data.add_arrays(uniform_data);

			array_copier ri_varying_data;
			ri_varying_data.add_arrays(varying_data);

			array_copier ri_vertex_data;
			ri_vertex_data.add_arrays(vertex_data);

			k3d::typed_array<k3d::ri::hpoint>* const ri_points = new k3d::typed_array<k3d::ri::hpoint>();

			const size_t point_begin = patch_first_points[patch];
			const size_t point_end = point_begin + (patch_u_point_counts[patch] * patch_v_point_counts[patch]);
			for(size_t point = point_begin; point != point_end; ++point)
			{
				ri_points->push_back(k3d::ri::hpoint(
					patch_point_weights[point] * points[patch_points[point]][0],
					patch_point_weights[point] * points[patch_points[point]][1],
					patch_point_weights[point] * points[patch_points[point]][2],
					patch_point_weights[point]));

				ri_vertex_data.push_back(patch_points[point]);
			}

			ri_constant_data.push_back(patch);
			ri_uniform_data.push_back(patch);
			ri_varying_data.insert(4 * patch, 4 * (patch + 1));

			k3d::ri::parameter_list ri_parameters;
			ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
			ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
			ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
			ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);
			ri_parameters.push_back(k3d::ri::parameter(k3d::ri::RI_PW(), k3d::ri::VERTEX, 1, ri_points));

			k3d::ri::setup_material(patch_materials[patch], RenderState);
			
			if (Mesh.nurbs_patches->patch_trim_curve_loop_counts && Mesh.nurbs_patches->trim_points)
			{
				const k3d::mesh::counts_t& patch_trim_curve_loop_counts = *Mesh.nurbs_patches->patch_trim_curve_loop_counts;
				const k3d::mesh::indices_t& patch_first_trim_curve_loops = *Mesh.nurbs_patches->patch_first_trim_curve_loops;
				const k3d::mesh::points_2d_t& trim_points = *Mesh.nurbs_patches->trim_points;
				const k3d::mesh::indices_t& first_trim_curves = *Mesh.nurbs_patches->first_trim_curves;
				const k3d::mesh::counts_t& trim_curve_counts = *Mesh.nurbs_patches->trim_curve_counts;
				const k3d::mesh::indices_t& trim_curve_first_points = *Mesh.nurbs_patches->trim_curve_first_points;
				const k3d::mesh::counts_t& trim_curve_point_counts = *Mesh.nurbs_patches->trim_curve_point_counts;
				const k3d::mesh::orders_t& trim_curve_orders = *Mesh.nurbs_patches->trim_curve_orders;
				const k3d::mesh::indices_t& trim_curve_first_knots = *Mesh.nurbs_patches->trim_curve_first_knots;
				const k3d::mesh::selection_t& trim_curve_selection = *Mesh.nurbs_patches->trim_curve_selection;
				const k3d::mesh::indices_t& trim_curve_points = *Mesh.nurbs_patches->trim_curve_points;
				const k3d::mesh::weights_t& trim_curve_point_weights = *Mesh.nurbs_patches->trim_curve_point_weights;
				const k3d::mesh::knots_t& trim_curve_knots = *Mesh.nurbs_patches->trim_curve_knots;
				
				k3d::ri::unsigned_integers ri_curve_counts;
				k3d::ri::unsigned_integers ri_trim_orders;
				k3d::ri::reals ri_trim_knots;
				k3d::ri::reals ri_trim_mins;
				k3d::ri::reals ri_trim_maxs;
				k3d::ri::unsigned_integers ri_trim_point_counts;
				k3d::ri::reals ri_trim_u;
				k3d::ri::reals ri_trim_v;
				k3d::ri::reals ri_trim_w;
				
				k3d::uint_t loops_start = patch_first_trim_curve_loops[patch];
				k3d::uint_t loops_end = loops_start + patch_trim_curve_loop_counts[patch];
				for (k3d::uint_t loop_index = loops_start; loop_index != loops_end; ++loop_index)
				{
					k3d::uint_t curves_start = first_trim_curves[loop_index];
					k3d::uint_t curves_end = curves_start + trim_curve_counts[loop_index];
					ri_curve_counts.push_back(trim_curve_counts[loop_index]);
					for (k3d::uint_t curve = curves_start; curve != curves_end; ++curve)
					{
						ri_trim_orders.push_back(trim_curve_orders[curve]);
						ri_trim_knots.insert(ri_trim_knots.end(), trim_curve_knots.begin() + trim_curve_first_knots[curve], trim_curve_knots.begin() + trim_curve_first_knots[curve] + trim_curve_point_counts[curve] + trim_curve_orders[curve]);
						ri_trim_mins.push_back(trim_curve_knots[trim_curve_orders[curve] - 1]);
						ri_trim_maxs.push_back(trim_curve_knots[trim_curve_point_counts[curve]]);
						ri_trim_point_counts.push_back(trim_curve_point_counts[curve]);
						k3d::uint_t points_start = trim_curve_first_points[curve];
						k3d::uint_t points_end = points_start + trim_curve_point_counts[curve];
						for (k3d::uint_t point = points_start; point != points_end; ++point)
						{
							k3d::point2 control_point = trim_points[trim_curve_points[point]];
							double weight = trim_curve_point_weights[point];
							ri_trim_u.push_back(static_cast<k3d::ri::real>(control_point[0] * weight));
							ri_trim_v.push_back(static_cast<k3d::ri::real>(control_point[1] * weight));
							ri_trim_w.push_back(static_cast<k3d::ri::real>(weight));
						}
					}
				}
				RenderState.stream.RiTrimCurve(
					ri_curve_counts,
					ri_trim_orders,
					ri_trim_knots,
					ri_trim_mins,
					ri_trim_maxs,
					ri_trim_point_counts,
					ri_trim_u,
					ri_trim_v,
					ri_trim_w
				);
			}
			
			RenderState.stream.RiNuPatchV(
				patch_u_point_counts[patch],
				patch_u_orders[patch],
				u_knots,
				u_knots[patch_u_orders[patch] - 1],
				u_knots[patch_u_point_counts[patch]],
				patch_v_point_counts[patch],
				patch_v_orders[patch],
				v_knots,
				v_knots[patch_v_orders[patch] - 1],
				v_knots[patch_v_point_counts[patch]],
				ri_parameters
				);
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_patch_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x120d87f0, 0xfe05419f, 0xae2d14ce, 0xea9c32b4),
			"RenderManNURBSPatchPainter",
			_("Renders NURBS patches"),
			"RenderMan Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_patch_painter_factory

k3d::iplugin_factory& nurbs_patch_painter_factory()
{
	return nurbs_patch_painter::get_factory();
}

} // namespace libk3drendermanpainters

