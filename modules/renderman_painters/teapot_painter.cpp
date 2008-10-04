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
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/teapot.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace renderman
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// teapot_painter

class teapot_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	teapot_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::teapot::const_primitive> teapot(k3d::teapot::validate(**primitive));
			if(!teapot)
				continue;

			for(k3d::uint_t i = 0; i != teapot->matrices.size(); ++i)
			{
				RenderState.stream.RiAttributeBegin();
				RenderState.stream.RiConcatTransform(k3d::ri::convert(teapot->matrices[i]));

				k3d::ri::setup_material(teapot->materials[i], RenderState);

				for(k3d::uint_t patch = 0; patch != 32; ++patch)
				{
					k3d::ri::parameter_list ri_parameters;

					array_copier ri_constant_data;
					ri_constant_data.add_arrays(teapot->constant_data);
					ri_constant_data.push_back(0);
					ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);

					array_copier ri_uniform_data;
					ri_uniform_data.add_arrays(teapot->uniform_data);
					ri_uniform_data.push_back(i);
					ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);

					k3d::typed_array<k3d::point3>* const ri_points = new k3d::typed_array<k3d::point3>(16);
					for(k3d::uint_t vertex = 0; vertex != 16; ++vertex)
					{
						(*ri_points)[vertex] = k3d::point3(
							teapot_points[teapot_patches[patch][vertex]][0],
							teapot_points[teapot_patches[patch][vertex]][1],
							teapot_points[teapot_patches[patch][vertex]][2]);
					}
					ri_parameters.push_back(k3d::ri::parameter(k3d::ri::RI_P(), k3d::ri::VERTEX, 1, ri_points));
					
					RenderState.stream.RiBasis("bezier", 3, "bezier", 3);
					RenderState.stream.RiPatchV("bicubic", ri_parameters);
				}

				RenderState.stream.RiAttributeEnd();
			}
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<teapot_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0xcf54aa9b, 0x9c4e886c, 0xa8cf2fa2, 0x470d44ed),
			"RenderManTeapotPainter",
			_("Renders teapot primitives"),
			"RenderMan Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

	static const k3d::teapot::points_array_t& teapot_points;
	static const k3d::teapot::patches_array_t& teapot_patches;
};

const k3d::teapot::points_array_t& teapot_painter::teapot_points = k3d::teapot::points();
const k3d::teapot::patches_array_t& teapot_painter::teapot_patches = k3d::teapot::patches();

/////////////////////////////////////////////////////////////////////////////
// teapot_painter_factory

k3d::iplugin_factory& teapot_painter_factory()
{
	return teapot_painter::get_factory();
}

} // namespace painters

} // namespace renderman

} // namespace module

