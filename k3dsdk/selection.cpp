// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include "imesh_source.h"
#include "inode.h"
#include "iomanip.h"
#include "iproperty.h"
#include "legacy_mesh.h"
#include "persistent_lookup.h"
#include "selection.h"

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
		case ABSOLUTE_POINT:
			Stream << "absolute_point";
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
		case UNIFORM:
			Stream << "uniform";
			break;
		case VARYING:
			Stream << "varying";
			break;
		case FACE_VARYING:
			Stream << "face_varying";
			break;
		case SPLIT_EDGE:
			Stream << "split_edge";
			break;
		case POINT:
			Stream << "point";
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
	else if(buffer == "absolute_point")
		RHS = ABSOLUTE_POINT;
/*
	else if(buffer == "absolute_face")
		RHS = ABSOLUTE_FACE;
	else if(buffer == "absolute_split_edge")
		RHS = ABSOLUTE_SPLIT_EDGE;
	else if(buffer == "absolute_linear_curve")
		RHS = ABSOLUTE_LINEAR_CURVE;
	else if(buffer == "absolute_cubic_curve")
		RHS = ABSOLUTE_CUBIC_CURVE;
	else if(buffer == "absolute_nurbs_curve" || buffer == "absolute_nucurve")
		RHS = ABSOLUTE_NURBS_CURVE;
	else if(buffer == "absolute_bilinear_patch")
		RHS = ABSOLUTE_BILINEAR_PATCH;
	else if(buffer == "absolute_bicubic_patch")
		RHS = ABSOLUTE_BICUBIC_PATCH;
	else if(buffer == "absolute_nurbs_patch" || buffer == "absolute_nupatch")
		RHS = ABSOLUTE_NURBS_PATCH;
*/
	else if(buffer == "user1")
		RHS = USER1;
	else if(buffer == "primitive")
		RHS = PRIMITIVE;
	else if(buffer == "constant")
		RHS = CONSTANT;
	else if(buffer == "uniform")
		RHS = UNIFORM;
	else if(buffer == "varying")
		RHS = VARYING;
	else if(buffer == "face_varying")
		RHS = FACE_VARYING;
	else if(buffer == "split_edge")
		RHS = SPLIT_EDGE;
	else if(buffer == "point")
		RHS = POINT;
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
// node_id

id node_id(inode* Node)
{
	return detail::node_lookup.lookup_id(Node);
}

//////////////////////////////////////////////////////////////////////////////////
// make_record

const record make_record(inode* Node)
{
	record result;
	result.zmin = 0;
	result.zmax = 0;

	result.tokens.push_back(token(NODE, node_id(Node)));

	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// make_records

const records make_records(inode* Node)
{
	return records(1, make_record(Node));
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

/*
///////////////////////////////////////////////////////////////////////////////////
// get_point_group

legacy::point_group* get_point_group(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(POINT_GROUP);
	if(id == null_id())
		return 0;
	return_val_if_fail(id < Mesh.point_groups.size(), 0);

	return Mesh.point_groups[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_polyhedron

legacy::polyhedron* get_polyhedron(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(POLYHEDRON);
	if(id == null_id())
		return 0;
	return_val_if_fail(id < Mesh.polyhedra.size(), 0);

	return Mesh.polyhedra[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_split_edge

legacy::split_edge* get_split_edge(legacy::mesh& Mesh, const record& Record)
{
	const selection::id edge_id = Record.get_id(SPLIT_EDGE);
	if(edge_id == null_id())
		return 0;

	k3d::legacy::face* const face = get_face(Mesh, Record);
	if(!face)
		return 0;

	const selection::id hole_id = Record.get_id(FACE_HOLE);
	if(hole_id == null_id())
	{
		selection::id id = 0;
		for(legacy::split_edge* edge = face->first_edge; edge; edge = edge->face_clockwise)
		{
			if(edge_id == id++)
				return edge;

			if(edge->face_clockwise == face->first_edge)
				break;
		}
	}
	else
	{
		return_val_if_fail(hole_id < face->holes.size(), 0);

		selection::id id = 0;
		for(legacy::split_edge* edge = face->holes[hole_id]; edge; edge = edge->face_clockwise)
		{
			if(edge_id == id++)
				return edge;

			if(edge->face_clockwise == face->holes[hole_id])
				break;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// get_linear_curve_group

legacy::linear_curve_group* get_linear_curve_group(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(LINEAR_CURVE_GROUP);
	if(id == null_id())
		return 0;
	return_val_if_fail(id < Mesh.linear_curve_groups.size(), 0);

	return Mesh.linear_curve_groups[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_linear_curve

legacy::linear_curve* get_linear_curve(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(LINEAR_CURVE);
	if(id == null_id())
		return 0;

	legacy::linear_curve_group* const group = get_linear_curve_group(Mesh, Record);
	if(!group)
		return 0;
	return_val_if_fail(id < group->curves.size(), 0);

	return group->curves[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_cubic_curve_group

legacy::cubic_curve_group* get_cubic_curve_group(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(CUBIC_CURVE_GROUP);
	if(id == null_id())
		return 0;
	return_val_if_fail(id < Mesh.cubic_curve_groups.size(), 0);

	return Mesh.cubic_curve_groups[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_cubic_curve

legacy::cubic_curve* get_cubic_curve(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(CUBIC_CURVE);
	if(id == null_id())
		return 0;

	legacy::cubic_curve_group* const group = get_cubic_curve_group(Mesh, Record);
	if(!group)
		return 0;
	return_val_if_fail(id < group->curves.size(), 0);

	return group->curves[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_nucurve_group

legacy::nucurve_group* get_nucurve_group(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(NUCURVE_GROUP);
	if(id == null_id())
		return 0;

	return_val_if_fail(id < Mesh.nucurve_groups.size(), 0);
	return Mesh.nucurve_groups[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_nucurve

legacy::nucurve* get_nucurve(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(NUCURVE);
	if(id == null_id())
		return 0;

	legacy::nucurve_group* const group = get_nucurve_group(Mesh, Record);
	if(!group)
		return 0;
	return_val_if_fail(id < group->curves.size(), 0);

	return group->curves[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_bilinear_patch

legacy::bilinear_patch* get_bilinear_patch(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(BILINEAR_PATCH);
	if(id == null_id())
		return 0;
	return_val_if_fail(id < Mesh.bilinear_patches.size(), 0);

	return Mesh.bilinear_patches[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_bicubic_patch

legacy::bicubic_patch* get_bicubic_patch(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(BICUBIC_PATCH);
	if(id == null_id())
		return 0;
	return_val_if_fail(id < Mesh.bicubic_patches.size(), 0);

	return Mesh.bicubic_patches[id];
}

///////////////////////////////////////////////////////////////////////////////////
// get_nupatch

legacy::nupatch* get_nupatch(legacy::mesh& Mesh, const record& Record)
{
	const selection::id id = Record.get_id(NUPATCH);
	if(id == null_id())
		return 0;
	return_val_if_fail(id < Mesh.nupatches.size(), 0);

	return Mesh.nupatches[id];
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
	Stream << standard_indent << "type: " << RHS.type << "\n";
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

