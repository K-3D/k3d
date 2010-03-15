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
	/// Returns the difference between two collections, using the imprecise semantics of difference::test().
	void difference(const named_tables& Other, difference::test_result& Result) const;
};

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const named_tables& RHS);

/// Specialization of difference::test for k3d::named_tables
namespace difference
{

inline void test(const named_tables& A, const named_tables& B, test_result& Result)
{
	A.difference(B, Result);
}

} // namespace difference

} // namespace k3d

#endif // !K3DSDK_NAMED_TABLES_H

