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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/named_array_copier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// add

class add :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	add(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_user_property_changed_signal(*this)
	{
		m_user_property_changed_signal.connect(sigc::mem_fun(*this, &add::mesh_changed));
	}
	
	void mesh_changed(k3d::ihint* Hint)
	{
		mesh_topology_changed(Hint);
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		// Get the set of input meshes ...
		typedef std::vector<const k3d::mesh*> meshes_t;
		meshes_t meshes;

		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::mesh*))
			{
				const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property));
				if(!mesh)
					continue;
				
				if(!k3d::validate_blobbies(*mesh))
					continue;

				meshes.push_back(mesh);
			}
		}

		k3d::named_arrays::named_arrays_collection source_varying_data;
		k3d::named_arrays::named_arrays_collection source_vertex_data;
		for(meshes_t::iterator mesh = meshes.begin(); mesh != meshes.end(); ++mesh)
		{
			source_varying_data.push_back(&(**mesh).blobbies->varying_data);
			source_vertex_data.push_back(&(**mesh).blobbies->vertex_data);
		}

		// Setup the initial state of the output mesh ...
		k3d::mesh::blobbies_t& target_blobbies = *k3d::make_unique(Mesh.blobbies);
		k3d::mesh::indices_t& target_first_primitives = *k3d::make_unique(target_blobbies.first_primitives);
		k3d::mesh::counts_t& target_primitive_counts = *k3d::make_unique(target_blobbies.primitive_counts);
		k3d::mesh::indices_t& target_first_operators = *k3d::make_unique(target_blobbies.first_operators);
		k3d::mesh::counts_t& target_operator_counts = *k3d::make_unique(target_blobbies.operator_counts);
		k3d::mesh::materials_t& target_materials = *k3d::make_unique(target_blobbies.materials);
		k3d::mesh::blobbies_t::primitives_t& target_primitives = *k3d::make_unique(target_blobbies.primitives);
		k3d::mesh::indices_t& target_primitive_first_floats = *k3d::make_unique(target_blobbies.primitive_first_floats);
		k3d::mesh::counts_t& target_primitive_float_counts = *k3d::make_unique(target_blobbies.primitive_float_counts);
		k3d::named_arrays& target_varying_data = target_blobbies.varying_data;
		k3d::named_arrays& target_vertex_data = target_blobbies.vertex_data;
		k3d::mesh::blobbies_t::operators_t& target_operators = *k3d::make_unique(target_blobbies.operators);
		k3d::mesh::indices_t& target_operator_first_operands = *k3d::make_unique(target_blobbies.operator_first_operands);
		k3d::mesh::counts_t& target_operator_operand_counts = *k3d::make_unique(target_blobbies.operator_operand_counts);
		k3d::mesh::blobbies_t::floats_t& target_floats = *k3d::make_unique(target_blobbies.floats);
		k3d::mesh::blobbies_t::operands_t& target_operands = *k3d::make_unique(target_blobbies.operands);

		target_varying_data = k3d::named_arrays::clone_types(source_varying_data);
		target_vertex_data = k3d::named_arrays::clone_types(source_vertex_data);

		target_first_primitives.push_back(0);
		target_primitive_counts.push_back(0);
		target_first_operators.push_back(0);
		target_operator_counts.push_back(0);
		target_materials.push_back(m_material.pipeline_value());

		k3d::mesh::indices_t blobby_roots;

		// Iterate over each input mesh ...
		for(meshes_t::iterator mesh = meshes.begin(); mesh != meshes.end(); ++mesh)
		{
			const k3d::mesh::indices_t& source_first_primitives = *(*mesh)->blobbies->first_primitives;
			const k3d::mesh::counts_t& source_primitive_counts = *(*mesh)->blobbies->primitive_counts;
			const k3d::mesh::indices_t& source_first_operators = *(*mesh)->blobbies->first_operators;
			const k3d::mesh::counts_t& source_operator_counts = *(*mesh)->blobbies->operator_counts;
			const k3d::mesh::materials_t& source_materials = *(*mesh)->blobbies->materials;
			const k3d::mesh::blobbies_t::primitives_t& source_primitives = *(*mesh)->blobbies->primitives;
			const k3d::mesh::indices_t& source_primitive_first_floats = *(*mesh)->blobbies->primitive_first_floats;
			const k3d::mesh::counts_t& source_primitive_float_counts = *(*mesh)->blobbies->primitive_float_counts;
			const k3d::named_arrays& source_varying_data = (*mesh)->blobbies->varying_data;
			const k3d::named_arrays& source_vertex_data = (*mesh)->blobbies->vertex_data;
			const k3d::mesh::blobbies_t::operators_t& source_operators = *(*mesh)->blobbies->operators;
			const k3d::mesh::indices_t& source_operator_first_operands = *(*mesh)->blobbies->operator_first_operands;
			const k3d::mesh::counts_t& source_operator_operand_counts = *(*mesh)->blobbies->operator_operand_counts;
			const k3d::mesh::blobbies_t::floats_t& source_floats = *(*mesh)->blobbies->floats;
			const k3d::mesh::blobbies_t::operands_t& source_operands = *(*mesh)->blobbies->operands;

			k3d::named_array_copier varying_copier(source_varying_data, target_varying_data, k3d::named_array_copier::copy_subset());
			k3d::named_array_copier vertex_copier(source_vertex_data, target_vertex_data, k3d::named_array_copier::copy_subset());

			const k3d::uint_t blobby_begin = 0;
			const k3d::uint_t blobby_end = blobby_begin + source_first_primitives.size();
			for(k3d::uint_t blobby = blobby_begin; blobby != blobby_end; ++blobby)
			{
				const k3d::uint_t operand_offset = target_primitive_counts[0] + target_operator_counts[0];

				const k3d::uint_t source_primitive_count = source_primitive_counts[blobby];
				const k3d::uint_t source_primitives_begin = source_first_primitives[blobby];
				const k3d::uint_t source_primitives_end = source_primitives_begin + source_primitive_count;
				for(k3d::uint_t source_primitive = source_primitives_begin; source_primitive != source_primitives_end; ++source_primitive)
				{
					target_primitives.push_back(source_primitives[source_primitive]);
					target_primitive_first_floats.push_back(target_floats.size());
					target_primitive_float_counts.push_back(source_primitive_float_counts[source_primitive]);
					varying_copier.push_back(source_primitive);
					vertex_copier.push_back(source_primitive);

					const k3d::uint_t source_floats_begin = source_primitive_first_floats[source_primitive];
					const k3d::uint_t source_floats_end = source_floats_begin + source_primitive_float_counts[source_primitive];
					for(k3d::uint_t source_float = source_floats_begin; source_float != source_floats_end; ++source_float)
						target_floats.push_back(source_floats[source_float]);
				}
				target_primitive_counts[0] += source_primitive_count;

				const k3d::uint_t source_operator_count = source_operator_counts[blobby];
				const k3d::uint_t source_operators_begin = source_first_operators[blobby];
				const k3d::uint_t source_operators_end = source_operators_begin + source_operator_count;
				for(k3d::uint_t source_operator = source_operators_begin; source_operator != source_operators_end; ++source_operator)
				{
					target_operators.push_back(source_operators[source_operator]);
					target_operator_first_operands.push_back(target_operands.size());
					target_operator_operand_counts.push_back(source_operator_operand_counts[source_operator]);

					const k3d::uint_t source_operands_begin = source_operator_first_operands[source_operator];
					const k3d::uint_t source_operands_end = source_operands_begin + source_operator_operand_counts[source_operator];
					for(k3d::uint_t source_operand = source_operands_begin; source_operand != source_operands_end; ++source_operand)
						target_operands.push_back(source_operands[source_operand] + operand_offset);
				}
				target_operator_counts[0] += source_operator_count;

				if(source_primitive_count || source_operator_count)
					blobby_roots.push_back(target_primitive_counts[0] + target_operator_counts[0] - 1);
			}
		}	

		if(!blobby_roots.empty())
		{
			target_operator_counts[0] += 1;
			target_operators.push_back(k3d::mesh::blobbies_t::ADD);
			target_operator_first_operands.push_back(target_operands.size());
			target_operator_operand_counts.push_back(blobby_roots.size() + 1);
			target_operands.push_back(blobby_roots.size());
			target_operands.insert(target_operands.end(), blobby_roots.begin(), blobby_roots.end());
		}
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imulti_mesh_sink> > > factory(
			k3d::uuid(0xf075d161, 0x2b42365f, 0x5eeb458b, 0x67c2a928),
			"BlobbyAdd",
			_("Combines blobbies using the Addition operator."),
			"Blobby",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d::user_property_changed_signal m_user_property_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// add_factory

k3d::iplugin_factory& add_factory()
{
	return add::get_factory();
}

} // namespace blobby

} // namespace module

