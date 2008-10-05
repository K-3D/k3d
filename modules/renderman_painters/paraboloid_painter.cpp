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
#include <k3dsdk/paraboloid.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace renderman
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// paraboloid_painter

class paraboloid_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	paraboloid_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::paraboloid::const_primitive> paraboloid(k3d::paraboloid::validate(**primitive));
			if(!paraboloid)
				continue;

			for(k3d::uint_t i = 0; i != paraboloid->matrices.size(); ++i)
			{
				RenderState.stream.RiAttributeBegin();
				RenderState.stream.RiConcatTransform(k3d::ri::convert(paraboloid->matrices[i]));

				k3d::ri::setup_material(paraboloid->materials[i], RenderState);

				k3d::ri::parameter_list ri_parameters;

				array_copier ri_constant_data;
				ri_constant_data.add_arrays(paraboloid->constant_data);
				ri_constant_data.push_back(0);
				ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);

				array_copier ri_uniform_data;
				ri_uniform_data.add_arrays(paraboloid->uniform_data);
				ri_uniform_data.push_back(i);
				ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);

				array_copier ri_varying_data;
				ri_varying_data.add_arrays(paraboloid->varying_data);
				for(k3d::uint_t j = 0; j != 4; ++j)
					ri_varying_data.push_back((i * 4) + j);
				ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
				
				RenderState.stream.RiParaboloidV(
					paraboloid->radii[i],
					paraboloid->z_min[i],
					paraboloid->z_max[i],
					k3d::degrees(paraboloid->sweep_angles[i]),
					ri_parameters);

				RenderState.stream.RiAttributeEnd();
			}
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<paraboloid_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x49dafd22, 0xd34c22e6, 0xcb8e30b3, 0x0f74f5ef),
			"RenderManParaboloidPainter",
			_("Renders paraboloid primitives"),
			"RenderMan Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// paraboloid_painter_factory

k3d::iplugin_factory& paraboloid_painter_factory()
{
	return paraboloid_painter::get_factory();
}

} // namespace painters

} // namespace renderman

} // namespace module

