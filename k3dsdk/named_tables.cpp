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

#include <k3dsdk/iomanip.h>
#include <k3dsdk/named_tables.h>

namespace k3d
{

///////////////////////////////////////////////////////////////////////////
// named_tables

const table* named_tables::lookup(const string_t& Name) const
{
	const_iterator result = find(Name);
	return result == end() ? static_cast<table*>(0) : &result->second;
}

table* named_tables::writable(const string_t& Name)
{
	iterator result = find(Name);
	return result == end() ? static_cast<table*>(0) : &result->second;
}

void named_tables::difference(const named_tables& Other, difference::test_result& Result) const
{
	// If we have differing numbers of tables, we definitely aren't equal
	Result.insert(size() == Other.size());

	for(named_tables::const_iterator a = begin(), b = Other.begin(); a != end() && b != Other.end(); ++a, ++b)
	{
		// Each pair of tables must have equal names
		Result.insert(a->first == b->first);

		// Perform element-wise comparisons of the tables 
		a->second.difference(b->second, Result);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// operator<<

std::ostream& operator<<(std::ostream& Stream, const named_tables& RHS)
{
	for(named_tables::const_iterator attributes = RHS.begin(); attributes != RHS.end(); ++attributes)
		Stream << standard_indent << "table \"" << attributes->first << "\"\n" << push_indent << attributes->second << pop_indent << "\n";

	return Stream;
}

} // namespace k3d

