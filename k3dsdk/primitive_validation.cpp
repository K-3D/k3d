// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/array.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/primitive_validation.h>

namespace k3d
{

static void require_valid_table(const mesh& Mesh, const string_t& Name, const table& Table)
{
	if(Name == "constant" && Table.column_count() && Table.row_count() != 1)
		throw std::runtime_error("'constant' table must have length 1.");

	for(mesh::table_t::const_iterator array_iterator = Table.begin(); array_iterator != Table.end(); ++array_iterator)
	{
		const array* const current_array = array_iterator->second.get();
		if(!current_array)
			throw std::runtime_error("NULL table array.");

		const array* const first_array = Table.begin()->second.get();
		if(current_array->size() != first_array->size())
			throw std::runtime_error("Table array length mismatch.");

		if(current_array->get_metadata_value(metadata::key::domain()) == metadata::value::mesh_point_indices_domain())
		{
			if(!Mesh.points)
				throw std::runtime_error("Mesh missing points array.");

			if(!Mesh.point_selection)
				throw std::runtime_error("Mesh missing point selections array.");

			const mesh::indices_t* const indices = dynamic_cast<const mesh::indices_t*>(current_array);
			if(!indices)
				throw std::runtime_error("Point indices array must be an index type.");

			const mesh::indices_t::const_iterator max = std::max_element(indices->begin(), indices->end());
			if(max != indices->end() && *max >= Mesh.points->size())
				throw std::runtime_error("Point indices array out-of-bounds.");
		}
	}
}

void require_valid_primitive(const mesh& Mesh, const mesh::primitive& Primitive)
{
	for(mesh::named_tables_t::const_iterator structure = Primitive.structure.begin(); structure != Primitive.structure.end(); ++structure)
		require_valid_table(Mesh, structure->first, structure->second);

	for(mesh::named_tables_t::const_iterator attributes = Primitive.attributes.begin(); attributes != Primitive.attributes.end(); ++attributes)
		require_valid_table(Mesh, attributes->first, attributes->second);

	for(mesh::named_tables_t::const_iterator attributes = Primitive.attributes.begin(); attributes != Primitive.attributes.end(); ++attributes)
	{
		if(0 == attributes->second.column_count())
			continue;

		const mesh::named_tables_t::const_iterator structure = Primitive.structure.find(attributes->first);
		if(structure != Primitive.structure.end())
		{
			if(structure->second.row_count() != attributes->second.row_count())
				throw std::runtime_error("Attribute / structure table length mismatch.");
		}
	}
}

const mesh::table_t& require_structure(const mesh::primitive& Primitive, const string_t& Name)
{
	const table* const structure = Primitive.structure.lookup(Name);

	if(!structure)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing structure [" + Name + "]");

	return *structure;
}

mesh::table_t& require_structure(mesh::primitive& Primitive, const string_t& Name)
{
	table* const structure = Primitive.structure.writable(Name);

	if(!structure)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing structure [" + Name + "]");

	return *structure;
}

const mesh::table_t& require_attributes(const mesh::primitive& Primitive, const string_t& Name)
{
	const table* const attributes = Primitive.attributes.lookup(Name);

	if(!attributes)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing attributes [" + Name + "]");

	return *attributes;
}

mesh::table_t& require_attributes(mesh::primitive& Primitive, const string_t& Name)
{
	table* const attributes = Primitive.attributes.writable(Name);

	if(!attributes)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing attributes [" + Name + "]");

	return *attributes;
}

void require_table_row_count(const mesh::primitive& Primitive, const table& Table, const string_t& TableName, const uint_t RowCount)
{
	if(TableName == "constant")
		throw std::runtime_error("'constant' tables are automatically tested, and must have length 1.");

	if(0 == Table.column_count())
		return;

	if(Table.row_count() != RowCount)
	{
		std::ostringstream buffer;
		buffer << "[" << Primitive.type << "] table [" << TableName << "] incorrect length [" << Table.row_count() << "], expected [" << RowCount << "]";
		throw std::runtime_error(buffer.str());
	}
}

void require_metadata(const mesh::primitive& Primitive, const array& Array, const string_t& ArrayName, const string_t& MetadataName, const string_t& MetadataValue)
{
	if(Array.get_metadata_value(MetadataName) != MetadataValue)
	{
		std::ostringstream buffer;
		buffer << "[" << Primitive.type << "] primitive [" << ArrayName << "] array missing [" << MetadataName << "] metadata value [" << MetadataValue << "]";
		throw std::runtime_error(buffer.str());
	}
}

} // namespace k3d

