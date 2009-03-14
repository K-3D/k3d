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
#include <k3dsdk/blobby.h>
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
// blobby_painter

class blobby_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	blobby_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		for(k3d::mesh::primitives_t::const_iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p)
		{
			boost::scoped_ptr<k3d::blobby::const_primitive> primitive(k3d::blobby::validate(**p));
			if(!primitive)
				continue;

			const size_t blobby_begin = 0;
			const size_t blobby_end = blobby_begin + primitive->first_primitives.size();
			for(size_t blobby = blobby_begin; blobby != blobby_end; ++blobby)
			{
				array_copier ri_constant_data;
				ri_constant_data.add_arrays(primitive->constant_data);

				array_copier ri_uniform_data;
				ri_uniform_data.add_arrays(primitive->uniform_data);

				array_copier ri_varying_data;
				ri_varying_data.add_arrays(primitive->varying_data);

				array_copier ri_vertex_data;
				ri_vertex_data.add_arrays(primitive->vertex_data);

				const k3d::ri::unsigned_integer ri_nleaf = primitive->primitive_counts[blobby];
				k3d::ri::unsigned_integers ri_codes;
				k3d::ri::reals ri_floats;
				k3d::ri::strings ri_strings;

				const size_t primitives_begin = primitive->first_primitives[blobby];
				const size_t primitives_end = primitives_begin + primitive->primitive_counts[blobby];
				for(size_t blobby_primitive = primitives_begin; blobby_primitive != primitives_end; ++blobby_primitive)
				{
					ri_codes.push_back(primitive->primitives[blobby_primitive]);
					ri_codes.push_back(ri_floats.size());

					const size_t floats_begin = primitive->primitive_first_floats[blobby_primitive];
					const size_t floats_end = floats_begin + primitive->primitive_float_counts[blobby_primitive];
					ri_floats.insert(ri_floats.end(), &primitive->floats[floats_begin], &primitive->floats[floats_end]);

					ri_varying_data.push_back(blobby_primitive);
					ri_vertex_data.push_back(blobby_primitive);
				}

				const size_t operators_begin = primitive->first_operators[blobby];
				const size_t operators_end = operators_begin + primitive->operator_counts[blobby];
				for(size_t op = operators_begin; op != operators_end; ++op)
				{
					ri_codes.push_back(primitive->operators[op]);

					const size_t operands_begin = primitive->operator_first_operands[op];
					const size_t operands_end = operands_begin + primitive->operator_operand_counts[op];
					for(size_t operand = operands_begin; operand != operands_end; ++operand)
						ri_codes.push_back(primitive->operands[operand]);
				}

				ri_constant_data.push_back(blobby);
				ri_uniform_data.push_back(blobby);

				k3d::ri::parameter_list ri_parameters;
				ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
				ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
				ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
				ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

				k3d::ri::setup_material(primitive->materials[blobby], RenderState);
				RenderState.stream.RiBlobbyV(ri_nleaf, ri_codes, ri_floats, ri_strings, ri_parameters);
			}
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<blobby_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x0221462c, 0xc3d44a12, 0x97b39425, 0x29774d3f),
			"RenderManBlobbyPainter",
			_("Renders blobbies (implicit surfaces)"),
			"RenderMan Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// blobby_painter_factory

k3d::iplugin_factory& blobby_painter_factory()
{
	return blobby_painter::get_factory();
}

} // namespace painters

} // namespace renderman

} // namespace module

