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
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/point_group.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace renderman
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// point_group_painter

class point_group_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	point_group_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		if(!k3d::validate_points(Mesh))
			return;

		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::attribute_arrays_t& vertex_data = Mesh.vertex_data;

		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::point_group::const_primitive> point_group(k3d::point_group::validate(**primitive));
			if(!point_group)
				continue;

			array_copier ri_constant_data;
			ri_constant_data.add_arrays(point_group->constant_data);

			array_copier ri_varying_data;
			ri_varying_data.add_arrays(point_group->varying_data);

			array_copier ri_vertex_data;
			ri_vertex_data.add_arrays(vertex_data);
			ri_vertex_data.add_array(k3d::ri::RI_P(), points);

			const k3d::uint_t point_begin = 0;
			const k3d::uint_t point_end = point_begin + point_group->points.size();
			for(k3d::uint_t point = point_begin; point != point_end; ++point)
				ri_vertex_data.push_back(point_group->points[point]);

			ri_constant_data.push_back(0);
			ri_varying_data.insert(point_begin, point_end);

			k3d::ri::parameter_list ri_parameters;
			ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
			ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
			ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

			k3d::ri::setup_material(point_group->material[0], RenderState);
			RenderState.stream.RiPointsV(point_group->points.size(), ri_parameters);
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
			"RenderMan Painter",
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

} // namespace painters

} // namespace renderman

} // namespace module

