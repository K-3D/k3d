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
#include <k3dsdk/bilinear_patch.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
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
// bilinear_patch_painter

class bilinear_patch_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	bilinear_patch_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::bilinear_patch::const_primitive> bilinear_patches(k3d::bilinear_patch::validate(Mesh, **primitive));
			if(!bilinear_patches)
				continue;

			const k3d::uint_t patch_begin = 0;
			const k3d::uint_t patch_end = patch_begin + (bilinear_patches->patch_points.size() / 4);
			for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
			{
				array_copier ri_constant_attributes;
				ri_constant_attributes.add_arrays(bilinear_patches->constant_attributes);

				array_copier ri_uniform_attributes;
				ri_uniform_attributes.add_arrays(bilinear_patches->patch_attributes);

				array_copier ri_varying_attributes;
				ri_varying_attributes.add_arrays(bilinear_patches->parameter_attributes);

				array_copier ri_point_attributes;
				ri_point_attributes.add_array(k3d::ri::RI_P(), *Mesh.points);

				array_copier ri_vertex_attributes;
				ri_vertex_attributes.add_arrays(bilinear_patches->vertex_attributes);

				ri_constant_attributes.push_back(0);
				ri_uniform_attributes.push_back(patch);

				const k3d::uint_t patch_points_begin = 4 * patch;
				const k3d::uint_t patch_points_end = patch_points_begin + 4;
				for(k3d::uint_t patch_point = patch_points_begin; patch_point != patch_points_end; ++patch_point)
					ri_point_attributes.push_back(bilinear_patches->patch_points[patch_point]);
				ri_vertex_attributes.insert(patch_points_begin, patch_points_end);
				ri_varying_attributes.insert(patch_points_begin, patch_points_end);

				k3d::ri::parameter_list ri_parameters;
				ri_constant_attributes.copy_to(k3d::ri::CONSTANT, ri_parameters);
				ri_uniform_attributes.copy_to(k3d::ri::UNIFORM, ri_parameters);
				ri_varying_attributes.copy_to(k3d::ri::VARYING, ri_parameters);
				ri_point_attributes.copy_to(k3d::ri::VERTEX, ri_parameters);
				ri_vertex_attributes.copy_to(k3d::ri::VERTEX, ri_parameters);

				k3d::ri::setup_material(bilinear_patches->patch_materials[patch], RenderState);
				RenderState.stream.RiPatchV("bilinear", ri_parameters);
			}
		}			
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bilinear_patch_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x3f51b275, 0x5a664903, 0x96ed9168, 0x503e30f0),
			"RenderManBilinearPatchPainter",
			_("Renders bilinear patches"),
			"RenderMan Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bilinear_patch_painter_factory

k3d::iplugin_factory& bilinear_patch_painter_factory()
{
	return bilinear_patch_painter::get_factory();
}

} // namespace painters

} // namespace renderman

} // namespace module

