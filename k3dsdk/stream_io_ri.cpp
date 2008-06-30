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

/** \file
	\author Tim Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include "stream_io_ri.h"
#include "texture3.h"
#include <set>

namespace k3d
{

namespace ri
{

namespace detail
{

/// A collection of RenderMan RIB parameter types
typedef std::set<std::string> predefined_types_t;

/// Returns the set of standard predefined RIB parameter types
predefined_types_t& predefined_types()
{
	static predefined_types_t types;
	if(types.empty())
	{
		types.insert("P");
		types.insert("Pz");
		types.insert("Pw");
/*
		types.insert("N");
		types.insert("Cs");
		types.insert("Os");
		types.insert("s");
		types.insert("t");
		types.insert("blur");
		types.insert("sblur");
		types.insert("tblur");
		types.insert("width");
		types.insert("swidth");
		types.insert("twidth");
		types.insert("filter");
		types.insert("fill");
		types.insert("fov");
		types.insert("shader");
*/
	}

	return types;
}

template<typename T>
bool print_parameter(const std::string& Type, std::ostream& Stream, const parameter& RHS)
{
	typedef const typed_array<T> array_t;

	if(array_t* const array = dynamic_cast<array_t*>(RHS.storage.get()))
	{
		// First, print the parameter name, with optional inline type info (only if inlining is enabled and the type isn't predefined) ...
		Stream << "\"";

		if(inline_types(Stream))
		{
			if(!detail::predefined_types().count(RHS.name))
			{
				Stream << RHS.storage_class << " " << Type << " ";
				if(RHS.tuple_size > 1)
					Stream << "[" << RHS.tuple_size << "] ";
			}
		}

		Stream << RHS.name << "\" ";

		// Next, print the parameter values
		Stream << format_array(array->begin(), array->end());

		return true;
	}

	return false;
}

} // namespace detail

///////////////////////////////////////////////////////////////////////////////////
// storage_class_t

std::ostream& operator<<(std::ostream& Stream, const storage_class_t RHS)
{
	switch(RHS)
	{
		case k3d::ri::CONSTANT:
			Stream << "constant";
			break;
		case k3d::ri::UNIFORM:
			Stream << "uniform";
			break;
		case k3d::ri::VARYING:
			Stream << "varying";
			break;
		case k3d::ri::VERTEX:
			Stream << "vertex";
			break;
		case k3d::ri::FACEVARYING:
			Stream << "facevarying";
			break;
		default:
			assert_not_reached();
	}

	return Stream;
}

///////////////////////////////////////////////////////////////////////////////////////
// inline_types

long& inline_types_storage(std::ios& Stream)
{
	static const int index = std::ios_base::xalloc();
	return Stream.iword(index);
}

bool inline_types(std::ostream& Stream)
{
	return inline_types_storage(Stream) ? true : false;
}

bool set_inline_types(std::ostream& Stream, const bool Enabled)
{
	bool old_state = inline_types_storage(Stream) ? true : false;
	inline_types_storage(Stream) = Enabled;
	return old_state;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// format_string

format_string::format_string(const string& Token) :
	token(Token)
{
}

std::ostream& operator<<(std::ostream& Stream, const format_string& RHS)
{
	Stream << "\"" << RHS.token << "\"";
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// format_matrix

format_matrix::format_matrix(const matrix& Matrix) :
	m(Matrix)
{
}

std::ostream& operator<<(std::ostream& Stream, const format_matrix& RHS)
{
	Stream << "[";
	for(int i = 0; i != 4; ++i)
	{
		for(int j = 0; j != 4; ++j)
		{
			Stream << RHS.m[j][i] << " ";
		}
	}
	Stream << "]";

	return Stream;
}

//////////////////////////////////////////////////////////////////////////////////////
// parameter

std::ostream& operator<<(std::ostream& Stream, const parameter& RHS)
{
	if(detail::print_parameter<integer>("integer", Stream, RHS))
		return Stream;
	if(detail::print_parameter<real>("float", Stream, RHS))
		return Stream;
	if(detail::print_parameter<string>("string", Stream, RHS))
		return Stream;
	if(detail::print_parameter<point>("point", Stream, RHS))
		return Stream;
	if(detail::print_parameter<vector>("vector", Stream, RHS))
		return Stream;
	if(detail::print_parameter<normal>("normal", Stream, RHS))
		return Stream;
	if(detail::print_parameter<texture3>("float[3]", Stream, RHS))
		return Stream;
	if(detail::print_parameter<color>("color", Stream, RHS))
		return Stream;
	if(detail::print_parameter<hpoint>("hpoint", Stream, RHS))
		return Stream;
	if(detail::print_parameter<matrix>("matrix", Stream, RHS))
		return Stream;

	log() << error << k3d_file_reference << " Parameter [" << RHS.name << "] unknown type" << std::endl;

	return Stream;
}

//////////////////////////////////////////////////////////////////////////////////////
// parameter_list

std::ostream& operator<<(std::ostream& Stream, const parameter_list& RHS)
{
	std::copy(RHS.begin(), RHS.end(), std::ostream_iterator<parameter>(Stream, " "));
	return Stream;
}

} // namespace ri

} // namespace k3d

