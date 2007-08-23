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
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

namespace libk3drendermanpainters
{

/////////////////////////////////////////////////////////////////////////////
// point_group_painter

class point_group_painter :
	public k3d::persistent<k3d::node>,
	public k3d::ri::imesh_painter
{
	typedef k3d::persistent<k3d::node> base;

public:
	point_group_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		if(!k3d::validate_point_groups(Mesh))
			return;

		const k3d::mesh::indices_t& first_points = *Mesh.point_groups->first_points;
		const k3d::mesh::counts_t& point_counts = *Mesh.point_groups->point_counts;
		const k3d::mesh::named_arrays& constant_data = Mesh.point_groups->constant_data;
		const k3d::mesh::materials_t& materials = *Mesh.point_groups->materials;
		const k3d::mesh::indices_t& group_points = *Mesh.point_groups->points;
		const k3d::mesh::named_arrays& varying_data = Mesh.point_groups->varying_data;
		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::named_arrays& vertex_data = Mesh.vertex_data;

		const size_t point_group_begin = 0;
		const size_t point_group_end = point_group_begin + first_points.size();
		for(size_t point_group = point_group_begin; point_group != point_group_end; ++point_group)
		{
			array_copier ri_constant_data;
			ri_constant_data.add_arrays(constant_data);

			array_copier ri_varying_data;
			ri_varying_data.add_arrays(varying_data);

			array_copier ri_vertex_data;
			ri_vertex_data.add_arrays(vertex_data);
			ri_vertex_data.add_array(k3d::ri::RI_P(), points);

			const size_t point_begin = first_points[point_group];
			const size_t point_end = point_begin + point_counts[point_group];
			for(size_t point = point_begin; point != point_end; ++point)
				ri_vertex_data.push_back(group_points[point]);

			ri_constant_data.push_back(point_group);
			ri_varying_data.insert(point_begin, point_end);

			k3d::ri::parameter_list ri_parameters;
			ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
			ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
			ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

			k3d::ri::setup_material(materials[point_group], RenderState);
			RenderState.engine.RiPointsV(point_counts[point_group], ri_parameters);
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<point_group_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x06fab90b, 0x2f034d69, 0xb4a1cb9f, 0xb9864144),
			"RenderManPointGroupPainter",
			_("Renders mesh point groups"),
			"RenderMan Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// point_group_painter_factory

k3d::iplugin_factory& point_group_painter_factory()
{
	return point_group_painter::get_factory();
}

} // namespace libk3drendermanpainters

