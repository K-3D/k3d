#ifndef K3DSDK_NAMED_ARRAYS_H
#define K3DSDK_NAMED_ARRAYS_H

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

#include <k3dsdk/difference.h>
#include <k3dsdk/pipeline_data.h>
#include <k3dsdk/types.h>

#include <map>

namespace k3d
{

class array;

/// Defines a heterogeneous collection of named, shared arrays.  Arrays in the collection are not length-constrained.
/// For a collection of arrays that all have the same length, see table.  For a concrete list of the
/// datatypes that can be stored using named_arrays, see k3d::named_array_types.
class named_arrays :
	public std::map<string_t, pipeline_data<array> >
{
public:
	/// Creates a new array with given name and type, inserting it into the collection and returning a reference to the result.
	/** \note: An existing array with the same name will be replaced by the new array. */
	template<typename ArrayT>
	ArrayT& create(const string_t& Name)
	{
		ArrayT* const array = new ArrayT();
		(*this)[Name].create(array);
		return *array;
	}
	/// Inserts a new array into the collection with the given name, returning a reference to the result.
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
	/// Returns an object containing empty arrays with the same name and type as the originals.
	named_arrays clone_types() const;
	/// Returns an object containing deep copies of all the original arrays.
	named_arrays clone() const;
	/// Compares two collections using the imprecise semantics of difference().
	void difference(const named_arrays& Other, bool_t& Equal, uint64_t& ULPS) const;
};

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const named_arrays& RHS);

/// Specialization of difference for named_arrays
inline void difference(const named_arrays& A, const named_arrays& B, bool_t& Equal, uint64_t& ULPS)
{
	A.difference(B, Equal, ULPS);
}

} // namespace k3d

#endif // !K3DSDK_NAMED_ARRAYS_H

