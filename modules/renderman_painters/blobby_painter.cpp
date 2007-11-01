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
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

namespace libk3drendermanpainters
{

/////////////////////////////////////////////////////////////////////////////
// blobby_painter

class blobby_painter :
	public k3d::persistent<k3d::node>,
	public k3d::ri::imesh_painter
{
	typedef k3d::persistent<k3d::node> base;

public:
	blobby_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		if(!k3d::validate_blobbies(Mesh))
			return;

		const k3d::mesh::indices_t& first_primitives = *Mesh.blobbies->first_primitives;
		const k3d::mesh::counts_t& primitive_counts = *Mesh.blobbies->primitive_counts;
		const k3d::mesh::indices_t& first_operators = *Mesh.blobbies->first_operators;
		const k3d::mesh::counts_t& operator_counts = *Mesh.blobbies->operator_counts;
		const k3d::mesh::materials_t& materials = *Mesh.blobbies->materials;
		const k3d::mesh::named_arrays& constant_data = Mesh.blobbies->constant_data;
		const k3d::mesh::named_arrays& uniform_data = Mesh.blobbies->uniform_data;
		const k3d::mesh::blobbies_t::primitives_t& primitives = *Mesh.blobbies->primitives;
		const k3d::mesh::indices_t& primitive_first_floats = *Mesh.blobbies->primitive_first_floats;
		const k3d::mesh::counts_t& primitive_float_counts = *Mesh.blobbies->primitive_float_counts;
		const k3d::mesh::named_arrays& varying_data = Mesh.blobbies->varying_data;
		const k3d::mesh::named_arrays& vertex_data = Mesh.blobbies->vertex_data;
		const k3d::mesh::blobbies_t::operators_t& operators = *Mesh.blobbies->operators;
		const k3d::mesh::indices_t& operator_first_operands = *Mesh.blobbies->operator_first_operands;
		const k3d::mesh::counts_t& operator_operand_counts = *Mesh.blobbies->operator_operand_counts;
		const k3d::mesh::blobbies_t::floats_t& floats = *Mesh.blobbies->floats;
		const k3d::mesh::blobbies_t::operands_t& operands = *Mesh.blobbies->operands;

		const size_t blobby_begin = 0;
		const size_t blobby_end = blobby_begin + first_primitives.size();
		for(size_t blobby = blobby_begin; blobby != blobby_end; ++blobby)
		{
			array_copier ri_constant_data;
			ri_constant_data.add_arrays(constant_data);

			array_copier ri_uniform_data;
			ri_uniform_data.add_arrays(uniform_data);

			array_copier ri_varying_data;
			ri_varying_data.add_arrays(varying_data);

			array_copier ri_vertex_data;
			ri_vertex_data.add_arrays(vertex_data);

			const k3d::ri::unsigned_integer ri_nleaf = primitive_counts[blobby];
			k3d::ri::unsigned_integers ri_codes;
			k3d::ri::reals ri_floats;
			k3d::ri::strings ri_strings;

			const size_t primitives_begin = first_primitives[blobby];
			const size_t primitives_end = primitives_begin + primitive_counts[blobby];
			for(size_t primitive = primitives_begin; primitive != primitives_end; ++primitive)
			{
				ri_codes.push_back(primitives[primitive]);
				ri_codes.push_back(ri_floats.size());

				const size_t floats_begin = primitive_first_floats[primitive];
				const size_t floats_end = floats_begin + primitive_float_counts[primitive];
				ri_floats.insert(ri_floats.end(), &floats[floats_begin], &floats[floats_end]);

				ri_varying_data.push_back(primitive);
				ri_vertex_data.push_back(primitive);
			}

			const size_t operators_begin = first_operators[blobby];
			const size_t operators_end = operators_begin + operator_counts[blobby];
			for(size_t op = operators_begin; op != operators_end; ++op)
			{
				ri_codes.push_back(operators[op]);

				const size_t operands_begin = operator_first_operands[op];
				const size_t operands_end = operands_begin + operator_operand_counts[op];
				for(size_t operand = operands_begin; operand != operands_end; ++operand)
					ri_codes.push_back(operands[operand]);
			}

			ri_constant_data.push_back(blobby);
			ri_uniform_data.push_back(blobby);

			k3d::ri::parameter_list ri_parameters;
			ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
			ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
			ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
			ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

			k3d::ri::setup_material(materials[blobby], RenderState);
			RenderState.stream.RiBlobbyV(ri_nleaf, ri_codes, ri_floats, ri_strings, ri_parameters);
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
			"RenderMan Painters",
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

} // namespace libk3drendermanpainters

