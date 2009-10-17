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

#include <k3dsdk/imesh_source.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/selection.h>

namespace k3d
{

namespace selection
{

namespace detail
{

persistent_lookup node_lookup;

} // namespace detail

std::ostream& operator<<(std::ostream& Stream, const type& RHS)
{
	switch(RHS)
	{
		case NONE:
			Stream << "none";
			break;
		case NODE:
			Stream << "node";
			break;
		case MESH:
			Stream << "mesh";
			break;
		case USER1:
			Stream << "user1";
			break;
		case PRIMITIVE:
			Stream << "primitive";
			break;
		case CONSTANT:
			Stream << "constant";
			break;
		case SURFACE:
			Stream << "surface";
			break;
		case VARYING:
			Stream << "varying";
			break;
		case EDGE:
			Stream << "edge";
			break;
		case POINT:
			Stream << "point";
			break;
		case CURVE:
			Stream << "curve";
			break;
		case FACE:
			Stream << "face";
			break;
		case PATCH:
			Stream << "patch";
			break;
		case VERTEX:
			Stream << "vertex";
			break;
		default:
			Stream << RHS;
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, type& RHS)
{
	std::string buffer;
	Stream >> buffer;

	if(buffer == "none")
		RHS = NONE;
	else if(buffer == "node")
		RHS = NODE;
	else if(buffer == "mesh")
		RHS = MESH;
	else if(buffer == "user1")
		RHS = USER1;
	else if(buffer == "primitive")
		RHS = PRIMITIVE;
	else if(buffer == "constant")
		RHS = CONSTANT;
	else if(buffer == "surface")
		RHS = SURFACE;
	else if(buffer == "varying")
		RHS = VARYING;
	else if(buffer == "edge")
		RHS = EDGE;
	else if(buffer == "point")
		RHS = POINT;
	else if(buffer == "curve")
		RHS = CURVE;
	else if(buffer == "face")
		RHS = FACE;
	else if(buffer == "patch")
		RHS = PATCH;
	else if(buffer == "vertex")
		RHS = VERTEX;
	else
		log() << error << k3d_file_reference << ": could not extract value [" << buffer << "]" << std::endl;

	return Stream;
}

id null_id()
{
	return static_cast<id>(-1);
}

token::token() :
	type(NONE),
	id(null_id())
{
}

/////////////////////////////////////////////////////////////////
// token

token::token(const selection::type Type, const selection::id ID) :
	type(Type),
	id(ID)
{
}

bool operator==(const token& LHS, const token& RHS)
{
	return LHS.type == RHS.type && LHS.id == RHS.id;
}

bool operator!=(const token& LHS, const token& RHS)
{
	return !(LHS == RHS);
}

std::ostream& operator<<(std::ostream& Stream, const token& RHS)
{
	Stream << RHS.type << " " << RHS.id;
	return Stream;
}

std::istream& operator>>(std::istream& Stream, token& RHS)
{
	Stream >> RHS.type >> RHS.id;
	return Stream;
}

/////////////////////////////////////////////////////////////////
// record

record::record() :
	zmin(0),
	zmax(0)
{
}

const record record::empty_record()
{
	return record();
}

bool record::empty() const
{
	return tokens.empty();
}

id record::get_id(const type Type) const
{
	for(tokens_t::const_iterator token = tokens.begin(); token != tokens.end(); ++token)
	{
		if(token->type == Type)
			return token->id;
	}

	return null_id();
}

const token record::get_token(const type Type) const
{
	for(tokens_t::const_iterator token = tokens.begin(); token != tokens.end(); ++token)
	{
		if(token->type == Type)
			return *token;
	}

	return token();
}

std::ostream& operator<<(std::ostream& Stream, const record& RHS)
{
	Stream << "zmin: " << RHS.zmin << " zmax: " << RHS.zmax << " ";
	std::copy(RHS.tokens.begin(), RHS.tokens.end(), std::ostream_iterator<token>(Stream, " "));

	return Stream;
}

std::istream& operator>>(std::istream& Stream, record& RHS)
{
	size_t count = 0;

	Stream >> RHS.zmin >> RHS.zmax >> count;

	selection::token token;
	while(Stream && count)
	{
		Stream >> token;
		RHS.tokens.push_back(token);
		--count;
	}

	return Stream;
}

//////////////////////////////////////////////////////////////////////////////////
// make_record

const record make_record(inode* Node)
{
	record result;
	result.zmin = 0;
	result.zmax = 0;

	result.tokens.push_back(token(NODE, detail::node_lookup.lookup_id(Node)));

	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// get_node

inode* get_node(const record& Record)
{
	const selection::id id = Record.get_id(NODE);
	if(id != null_id())
		return dynamic_cast<inode*>(k3d::selection::detail::node_lookup.lookup_object(id));

	return 0;
}

/*
///////////////////////////////////////////////////////////////////////////////////
// get_mesh

mesh* get_mesh(const record& Record)
{
	inode* const node = get_node(Record);
	if(!node)
		return 0;

	const selection::id id = Record.get_id(MESH);
	return_val_if_fail(id == 0, 0); // Should never get a node with more than one mesh!

	imesh_source* const mesh_source = dynamic_cast<imesh_source*>(node);
	return_val_if_fail(mesh_source, 0);

	return boost::any_cast<k3d::mesh*>(mesh_source->mesh_source_output().property_internal_value());
}

///////////////////////////////////////////////////////////////////////////////////
// get_mesh

const mesh::primitive* get_primitive(const record& Record)
{
	const mesh* const m = get_mesh(Record);
	if(!mesh)
		return 0;

	const selection::id primitive_id = Record.get_id(PRIMITIVE);
	if(primitive_id == null_id())
		return 0;

	primitive_id = token->id;
	return_val_if_fail(mesh, 0);
	return_val_if_fail(primitive_id < mesh->primitives.size(), 0);
	return mesh->primitives[primitive_id].get();
}
*/

} // namespace selection

namespace gl
{

///////////////////////////////////////////////////////////////////////////////////
// push_selection_token

void push_selection_token(inode* Node)
{
	::glPushName(selection::NODE);
	::glPushName(k3d::selection::detail::node_lookup.lookup_id(Node));
}

///////////////////////////////////////////////////////////////////////////////////
// push_selection_token

void push_selection_token(const selection::token& Token)
{
	::glPushName(Token.type);
	::glPushName(Token.id);
}

///////////////////////////////////////////////////////////////////////////////////
// push_selection_token

void push_selection_token(const selection::type Type, const selection::id ID)
{
	::glPushName(Type);
	::glPushName(ID);
}

///////////////////////////////////////////////////////////////////////////////////
// pop_selection_token

void pop_selection_token()
{
	::glPopName();
	::glPopName();
}

} // namespace gl

namespace selection
{

////////////////////////////////////////////////////////////////////////////////////
// storage

storage::storage()
{
}

storage::storage(const string_t& Type) :
	type(Type)
{
}

bool_t storage::almost_equal(const storage& Other, const uint64_t Threshold) const
{
	return
		k3d::almost_equal<string_t>(Threshold)(type, Other.type) &&
		k3d::almost_equal<named_arrays>(Threshold)(structure, Other.structure)
		;
}

std::ostream& operator<<(std::ostream& Stream, const storage& RHS)
{
	Stream << standard_indent << "type: \"" << RHS.type << "\"\n";
	Stream << standard_indent << "structure (" << RHS.structure.size() << "):\n";
	Stream << push_indent << RHS.structure << pop_indent;

	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////
// set

storage& set::create(const string_t& Type)
{
	push_back(pipeline_data<storage>());
	return back().create(new storage(Type));
}

bool_t set::almost_equal(const set& Other, const uint64_t Threshold) const
{
	if(size() != Other.size())
		return false;

	const set& self = *this;
	for(uint_t i = 0; i != self.size(); ++i)
	{
		if(!k3d::almost_equal<storage>(Threshold)(*self[i], *Other[i]))
			return false;
	}

	return true;
}

void set::append(const set& Source, set& Target)
{
	Target.insert(Target.end(), Source.begin(), Source.end());
}

std::ostream& operator<<(std::ostream& Stream, const set& RHS)
{
	for(set::const_iterator storage = RHS.begin(); storage != RHS.end(); ++storage)
	{
		Stream << standard_indent << "storage:\n";
		Stream << push_indent << **storage << pop_indent;
	}

	return Stream;
}

} // namespace selection

} // namespace k3d

