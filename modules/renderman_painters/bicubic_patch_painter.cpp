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
#include <k3dsdk/i18n.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

namespace libk3drendermanpainters
{

/////////////////////////////////////////////////////////////////////////////
// bicubic_patch_painter

class bicubic_patch_painter :
	public k3d::persistent<k3d::node>,
	public k3d::ri::imesh_painter
{
	typedef k3d::persistent<k3d::node> base;

public:
	bicubic_patch_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::dev::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		if(!k3d::dev::validate_bicubic_patches(Mesh))
			return;

		const k3d::dev::mesh::materials_t& patch_materials = *Mesh.bicubic_patches->patch_materials;
		const k3d::dev::mesh::named_arrays constant_data = Mesh.bicubic_patches->constant_data;
		const k3d::dev::mesh::named_arrays uniform_data = Mesh.bicubic_patches->uniform_data;
		const k3d::dev::mesh::indices_t& patch_points = *Mesh.bicubic_patches->patch_points;
		const k3d::dev::mesh::named_arrays varying_data = Mesh.bicubic_patches->varying_data;
		const k3d::dev::mesh::points_t& points = *Mesh.points;
		const k3d::dev::mesh::named_arrays vertex_data = Mesh.vertex_data;

		const size_t patch_begin = 0;
		const size_t patch_end = patch_begin + (patch_points.size() / 16);
		for(size_t patch = patch_begin; patch != patch_end; ++patch)
		{
			array_copier ri_constant_data;
			ri_constant_data.add_arrays(constant_data);

			array_copier ri_uniform_data;
			ri_uniform_data.add_arrays(uniform_data);

			array_copier ri_varying_data;
			ri_varying_data.add_arrays(varying_data);

			array_copier ri_vertex_data;
			ri_vertex_data.add_arrays(vertex_data);
			ri_vertex_data.add_array(k3d::ri::RI_P(), points);

			const size_t point_begin = patch * 16;
			const size_t point_end = point_begin + 16;
			for(size_t point = point_begin; point != point_end; ++point)
				ri_vertex_data.push_back(patch_points[point]);

			ri_constant_data.push_back(patch);
			ri_uniform_data.push_back(patch);
			ri_varying_data.insert(4 * patch, 4 * (patch + 1));

			k3d::ri::parameter_list ri_parameters;
			ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
			ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
			ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
			ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

			k3d::ri::setup_material(patch_materials[patch], RenderState);

			// At the moment, we force all bicubic patches to be Bezier ...
			RenderState.engine.RiBasis("bezier", 3, "bezier", 3);
			RenderState.engine.RiPatchV("bicubic", ri_parameters);
		}
	}

	void paint_complete(const k3d::dev::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bicubic_patch_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x64dfefde, 0xeedc4047, 0xb59fc8f6, 0x972a9b86),
			"RenderManBicubicPatchPainter",
			_("Renders bicubic patches"),
			"RenderMan Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bicubic_patch_painter_factory

k3d::iplugin_factory& bicubic_patch_painter_factory()
{
	return bicubic_patch_painter::get_factory();
}

} // namespace libk3drendermanpainters

