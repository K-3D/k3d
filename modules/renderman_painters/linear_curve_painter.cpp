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
		if(!k3d::validate_linear_curve_groups(Mesh))
			return;

		const k3d::mesh::indices_t& first_curves = *Mesh.linear_curve_groups->first_curves;
		const k3d::mesh::counts_t& curve_counts = *Mesh.linear_curve_groups->curve_counts;
		const k3d::mesh::bools_t& periodic_curves = *Mesh.linear_curve_groups->periodic_curves;
		const k3d::mesh::materials_t& materials = *Mesh.linear_curve_groups->materials;
		const k3d::mesh::named_arrays& constant_data = Mesh.linear_curve_groups->constant_data;
		const k3d::mesh::indices_t& curve_first_points = *Mesh.linear_curve_groups->curve_first_points;
		const k3d::mesh::counts_t& curve_point_counts = *Mesh.linear_curve_groups->curve_point_counts;
		const k3d::mesh::named_arrays& uniform_data = Mesh.linear_curve_groups->uniform_data;
		const k3d::mesh::named_arrays& varying_data = Mesh.linear_curve_groups->varying_data;
		const k3d::mesh::indices_t& curve_points = *Mesh.linear_curve_groups->curve_points;
		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::named_arrays& vertex_data = Mesh.vertex_data;

		const size_t group_begin = 0;
		const size_t group_end = group_begin + first_curves.size();
		for(size_t group = group_begin; group != group_end; ++group)
		{
			k3d::ri::unsigned_integers ri_point_counts;

			array_copier ri_constant_data;
			ri_constant_data.add_arrays(constant_data);

			array_copier ri_uniform_data;
			ri_uniform_data.add_arrays(uniform_data);
			
			array_copier ri_varying_data;
			ri_varying_data.add_arrays(varying_data);

			array_copier ri_vertex_data;
			ri_vertex_data.add_arrays(vertex_data);
			ri_vertex_data.add_array(k3d::ri::RI_P(), points);

			const size_t curves_begin = first_curves[group];
			const size_t curves_end = curves_begin + curve_counts[group];
			for(size_t curve = curves_begin; curve != curves_end; ++curve)
			{
				const size_t curve_points_begin = curve_first_points[curve];
				const size_t curve_points_end = curve_points_begin + curve_point_counts[curve];
				for(size_t curve_point = curve_points_begin; curve_point != curve_points_end; ++curve_point)
					ri_vertex_data.push_back(curve_points[curve_point]);

				ri_point_counts.push_back(curve_point_counts[curve]);
				ri_varying_data.insert(curve_points_begin, curve_points_end);
			}

			ri_constant_data.push_back(group);
			ri_uniform_data.insert(curves_begin, curves_end);

			k3d::ri::parameter_list ri_parameters;
			ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
			ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
			ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
			ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

			k3d::ri::setup_material(materials[group], RenderState);
			RenderState.stream.RiCurvesV("linear", ri_point_counts, periodic_curves[group] ? "periodic" : "nonperiodic", ri_parameters);
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
			"RenderMan Painters",
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

} // namespace libk3drendermanpainters

