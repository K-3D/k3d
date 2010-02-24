#ifndef K3DSDK_TABLE_H
#define K3DSDK_TABLE_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3dsdk/difference.h>
#include <k3dsdk/pipeline_data.h>
#include <k3dsdk/types.h>

#include <map>
#include <vector>

namespace k3d
{

class array;

/// Defines a heterogeneous collection of named, shared arrays of equal length.  Note that the length of every
/// array in the collection must remain equal at all times.
/// For a concrete list of the datatypes that can be stored using k3d::table, see k3d::named_array_types.
class table :
	public std::map<string_t, pipeline_data<array> >
{
	typedef std::map<string_t, pipeline_data<array> > base;

public:
	/// Creates a new array with given name and type, inserting it into the table and returning a reference to the result.
	/** \note: An existing array with the same name will be replaced by the new array. */
	template<typename ArrayT>
	ArrayT& create(const string_t& Name)
	{
		ArrayT* const array = new ArrayT();
		(*this)[Name].create(array);
		return *array;
	}
	/// Inserts a new array into the table with the given name, returning a reference to the result.
	/** \note: An existing array with the same name will be replaced by the new array. */
	template<typename ArrayT>
	ArrayT& create(const string_t& Name, ArrayT* Array)
	{
		(*this)[Name].create(Array);
		return *Array;
	}
	/// Returns an existing array with the given name, or NULL if no matching array exists.
	const array* lookup(const string_t& Name) const;
	/// Returns an existing array with the given name and type, or NULL if no matching array exists.
	template<typename ArrayT>
	const ArrayT* lookup(const string_t& Name) const
	{
		return dynamic_cast<const ArrayT*>(lookup(Name));
	}
	/// Returns an existing array with the given name, or NULL if no matching array exists.
	array* writable(const string_t& Name);
	/// Returns an existing array with the given name and type, or NULL if no matching array exists.
	template<typename ArrayT>
	ArrayT* writable(const string_t& Name)
	{
		return dynamic_cast<ArrayT*>(writable(Name));
	}
	/// Returns an table containing empty arrays with the same name and type as the originals.
	table clone_types() const;
	/// Returns an table containing deep copies of all the original arrays.
	table clone() const;
	/// Returns an table containing copies of a half-open range of all the original arrays.
	table clone(const uint_t Begin, const uint_t End) const;
	/// Returns the difference between two tables, using the imprecise semantics of difference().
	void difference(const table& Other, bool_t& Equal, uint64_t& ULPS) const;

	typedef std::vector<const table*> table_collection;
	static table clone_types(const table_collection& AttributeArrays);

	/// Returns the number of columns in the table.
	uint_t column_count() const;

	/// Returns the number of rows in the table.
	uint_t row_count() const;
	/// Sets the number of rows in the table (i.e: resizes every column array).
	void set_row_count(const uint_t NewSize);

private:
	uint_t size() const;
};

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const table& RHS);

/// Specialization of difference for k3d::table
inline void difference(const table& A, const table& B, bool_t& Equal, uint64_t& ULPS)
{
	A.difference(B, Equal, ULPS);
}

} // namespace k3d

#endif // !K3DSDK_TABLE_H

