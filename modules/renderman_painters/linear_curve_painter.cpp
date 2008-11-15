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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/linear_curve.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
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
// linear_curve_painter

class linear_curve_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	linear_curve_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::linear_curve::const_primitive> linear_curve(k3d::linear_curve::validate(**primitive));
			if(!linear_curve)
				continue;
			
			const k3d::mesh::points_t& points = *Mesh.points;
			const k3d::mesh::attribute_arrays_t& vertex_data = Mesh.vertex_data;

			const k3d::uint_t group_begin = 0;
			const k3d::uint_t group_end = group_begin + linear_curve->first_curves.size();
			for(k3d::uint_t group = group_begin; group != group_end; ++group)
			{
				k3d::ri::unsigned_integers ri_point_counts;

				array_copier ri_constant_data;
				ri_constant_data.add_arrays(linear_curve->constant_data);

				array_copier ri_uniform_data;
				ri_uniform_data.add_arrays(linear_curve->uniform_data);
				
				array_copier ri_varying_data;
				ri_varying_data.add_arrays(linear_curve->varying_data);

				array_copier ri_vertex_data;
				ri_vertex_data.add_arrays(vertex_data);
				ri_vertex_data.add_array(k3d::ri::RI_P(), points);

				const k3d::uint_t curves_begin = linear_curve->first_curves[group];
				const k3d::uint_t curves_end = curves_begin + linear_curve->curve_counts[group];
				for(k3d::uint_t curve = curves_begin; curve != curves_end; ++curve)
				{
					const k3d::uint_t curve_points_begin = linear_curve->curve_first_points[curve];
					const k3d::uint_t curve_points_end = curve_points_begin + linear_curve->curve_point_counts[curve];
					for(k3d::uint_t curve_point = curve_points_begin; curve_point != curve_points_end; ++curve_point)
						ri_vertex_data.push_back(linear_curve->curve_points[curve_point]);

					ri_point_counts.push_back(linear_curve->curve_point_counts[curve]);
					ri_varying_data.insert(curve_points_begin, curve_points_end);
				}

				ri_constant_data.push_back(group);
				ri_uniform_data.insert(curves_begin, curves_end);

				k3d::ri::parameter_list ri_parameters;
				ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
				ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
				ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
				ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

				k3d::ri::setup_material(linear_curve->materials[group], RenderState);
				RenderState.stream.RiCurvesV("linear", ri_point_counts, linear_curve->periodic_curves[group] ? "periodic" : "nonperiodic", ri_parameters);
			}
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<linear_curve_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0xf8b19206, 0xa0ae4bd1, 0xb3548a15, 0x1209469e),
			"RenderManLinearCurvePainter",
			_("Renders linear curves"),
			"RenderMan Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// linear_curve_painter_factory

k3d::iplugin_factory& linear_curve_painter_factory()
{
	return linear_curve_painter::get_factory();
}

} // namespace painters

} // namespace renderman

} // namespace module

