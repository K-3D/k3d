#ifndef K3DSDK_STREAM_IO_RI_H
#define K3DSDK_STREAM_IO_RI_H

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
*/

#include "types_ri.h"

namespace k3d
{

namespace ri
{

/// Serializes storage_class_t to a stream
std::ostream& operator<<(std::ostream& Stream, const storage_class_t RHS);

/// iostream-compatible manipulator that returns true iff inline types are enabled for an output stream
bool inline_types(std::ostream& Stream);
/// iostream-compatible manipulator that controls whether inline types are enabled for an output stream
bool set_inline_types(std::ostream& Stream, const bool Enabled);

/// Formats a string with real-quotes for inclusion in a RIB file; designed to be used as an inline formatting object
class format_string
{
public:
	explicit format_string(const string& Token);
	friend std::ostream& operator<<(std::ostream& Stream, const format_string& RHS);

private:
	const string& token;
};

/// Formats a matrix for inclusion in a RIB file; designed to be used as an inline formatting object
class format_matrix
{
public:
	explicit format_matrix(const matrix& Matrix);
	friend std::ostream& operator<<(std::ostream& Stream, const format_matrix& RHS);

private:
	const matrix& m;
};

/// Formats an array of values within square brackets for inclusion in a RIB file; designed to be used as an inline formatting object
template<typename iterator_t, typename value_t>
class format_array_t
{
public:
	format_array_t(const iterator_t Begin, const iterator_t End) :
		begin(Begin),
		end(End)
	{
	}

	friend std::ostream& operator << (std::ostream& Stream, const format_array_t& RHS)
	{
		Stream << "[ ";
		std::copy(RHS.begin, RHS.end, std::ostream_iterator<value_t>(Stream, " "));
		Stream << "]";

		return Stream;
	}

private:
	const iterator_t begin;
	const iterator_t end;
};

/// Partial specialization of format_array_t for use with string values
template<typename iterator_t>
class format_array_t<iterator_t, k3d::ri::string>
{
public:
	format_array_t(const iterator_t Begin, const iterator_t End) :
		begin(Begin),
		end(End)
	{
	}

	friend std::ostream& operator << (std::ostream& Stream, const format_array_t& RHS)
	{
		Stream << "[ ";
		for(iterator_t element = RHS.begin; element != RHS.end; ++element)
			Stream << format_string(*element) << " ";
		Stream << "]";

		return Stream;
	}

private:
	const iterator_t begin;
	const iterator_t end;
};

/// Convenience factory function for creating format_array_t objects
template<typename iterator_t>
format_array_t<iterator_t, typename std::iterator_traits<iterator_t>::value_type> format_array(const iterator_t Begin, const iterator_t End)
{
	return format_array_t<iterator_t, typename std::iterator_traits<iterator_t>::value_type>(Begin, End);
}

/// Serializes a parameter to a stream
std::ostream& operator<<(std::ostream& Stream, const parameter& RHS);
/// Serializes a parameter list to a stream
std::ostream& operator<<(std::ostream& Stream, const parameter_list& RHS);

} // namespace ri

} // namespace k3d

#endif // !K3DSDK_STREAM_IO_RI_H

