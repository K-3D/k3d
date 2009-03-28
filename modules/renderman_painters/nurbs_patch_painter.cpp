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
*/

#include "array_helpers.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace renderman
{

namespace painters
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
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs(k3d::nurbs_patch::validate(**primitive));
			if(!nurbs)
				return;

			const k3d::mesh::points_t& points = *Mesh.points;

			const k3d::uint_t patch_begin = 0;
			const k3d::uint_t patch_end = patch_begin + nurbs->patch_first_points.size();
			for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
			{
				k3d::ri::reals u_knots(nurbs->patch_u_knots.begin() + nurbs->patch_u_first_knots[patch], nurbs->patch_u_knots.begin() + nurbs->patch_u_first_knots[patch] + nurbs->patch_u_point_counts[patch] + nurbs->patch_u_orders[patch]);
				k3d::ri::reals v_knots(nurbs->patch_v_knots.begin() + nurbs->patch_v_first_knots[patch], nurbs->patch_v_knots.begin() + nurbs->patch_v_first_knots[patch] + nurbs->patch_v_point_counts[patch] + nurbs->patch_v_orders[patch]);

				array_copier ri_constant_data;
				ri_constant_data.add_arrays(nurbs->constant_data);

				array_copier ri_uniform_data;
				ri_uniform_data.add_arrays(nurbs->uniform_data);

				array_copier ri_varying_data;
				ri_varying_data.add_arrays(nurbs->varying_data);

				array_copier ri_vertex_data;
				ri_vertex_data.add_arrays(Mesh.vertex_data);

				k3d::typed_array<k3d::ri::hpoint>* const ri_points = new k3d::typed_array<k3d::ri::hpoint>();

				const k3d::uint_t point_begin = nurbs->patch_first_points[patch];
				const k3d::uint_t point_end = point_begin + (nurbs->patch_u_point_counts[patch] * nurbs->patch_v_point_counts[patch]);
				for(k3d::uint_t point = point_begin; point != point_end; ++point)
				{
					ri_points->push_back(k3d::ri::hpoint(
						nurbs->patch_point_weights[point] * points[nurbs->patch_points[point]][0],
						nurbs->patch_point_weights[point] * points[nurbs->patch_points[point]][1],
						nurbs->patch_point_weights[point] * points[nurbs->patch_points[point]][2],
						nurbs->patch_point_weights[point]));

					ri_vertex_data.push_back(nurbs->patch_points[point]);
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

				k3d::ri::setup_material(nurbs->patch_materials[patch], RenderState);
				
				if(nurbs->patch_trim_loop_counts[patch])
				{
					k3d::ri::unsigned_integers ri_curve_counts;
					k3d::ri::unsigned_integers ri_trim_orders;
					k3d::ri::reals ri_trim_knots;
					k3d::ri::reals ri_trim_mins;
					k3d::ri::reals ri_trim_maxs;
					k3d::ri::unsigned_integers ri_trim_point_counts;
					k3d::ri::reals ri_trim_u;
					k3d::ri::reals ri_trim_v;
					k3d::ri::reals ri_trim_w;
					
					k3d::uint_t loops_start = nurbs->patch_first_trim_loops[patch];
					k3d::uint_t loops_end = loops_start + nurbs->patch_trim_loop_counts[patch];
					for (k3d::uint_t loop_index = loops_start; loop_index != loops_end; ++loop_index)
					{
						k3d::uint_t curves_start = nurbs->trim_loop_first_curves[loop_index];
						k3d::uint_t curves_end = curves_start + nurbs->trim_loop_curve_counts[loop_index];
						ri_curve_counts.push_back(nurbs->trim_loop_curve_counts[loop_index]);
						for (k3d::uint_t curve = curves_start; curve != curves_end; ++curve)
						{
							ri_trim_orders.push_back(nurbs->curve_orders[curve]);
							ri_trim_knots.insert(ri_trim_knots.end(), nurbs->curve_knots.begin() + nurbs->curve_first_knots[curve], nurbs->curve_knots.begin() + nurbs->curve_first_knots[curve] + nurbs->curve_point_counts[curve] + nurbs->curve_orders[curve]);
							ri_trim_mins.push_back(nurbs->curve_knots[nurbs->curve_orders[curve] - 1]);
							ri_trim_maxs.push_back(nurbs->curve_knots[nurbs->curve_point_counts[curve]]);
							ri_trim_point_counts.push_back(nurbs->curve_point_counts[curve]);
							k3d::uint_t points_start = nurbs->curve_first_points[curve];
							k3d::uint_t points_end = points_start + nurbs->curve_point_counts[curve];
							for (k3d::uint_t point = points_start; point != points_end; ++point)
							{
								k3d::point2 control_point = nurbs->points[nurbs->curve_points[point]];
								double weight = nurbs->curve_point_weights[point];
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
					nurbs->patch_u_point_counts[patch],
					nurbs->patch_u_orders[patch],
					u_knots,
					u_knots[nurbs->patch_u_orders[patch] - 1],
					u_knots[nurbs->patch_u_point_counts[patch]],
					nurbs->patch_v_point_counts[patch],
					nurbs->patch_v_orders[patch],
					v_knots,
					v_knots[nurbs->patch_v_orders[patch] - 1],
					v_knots[nurbs->patch_v_point_counts[patch]],
					ri_parameters
					);
			}
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
			"RenderMan Painter",
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

} // namespace painters

} // namespace renderman

} // namespace module

