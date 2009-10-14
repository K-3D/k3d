#ifndef K3DSDK_NAMED_TABLES_H
#define K3DSDK_NAMED_TABLES_H

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

#include <k3dsdk/table.h>

namespace k3d
{

/// Defines a collection of named table objects.  The length of the individual
/// table objects may vary.
class named_tables :
	public std::map<string_t, table>
{
public:
	/// Return an attribute_array by name, or NULL
	const table* lookup(const string_t& Name) const;
	/// Return an attribute_array by name, or NULL
	table* writable(const string_t& Name);
	/// Returns true iff two collections are equivalent, using the imprecise semantics of almost_equal to compare values.
	bool_t almost_equal(const named_tables& Other, const uint64_t Threshold) const;
};

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const named_tables& RHS);

/// Specialization of almost_equal that tests named_tables for equality
template<>
class almost_equal<named_tables>
{
	typedef named_tables T;

public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline bool_t operator()(const T& A, const T& B) const
	{
		return A.almost_equal(B, threshold);
	}

	const uint64_t threshold;
};

} // namespace k3d

#endif // !K3DSDK_NAMED_TABLES_H

