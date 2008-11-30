#ifndef K3DSDK_ARRAY_H
#define K3DSDK_ARRAY_H

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

#include "almost_equal.h"
#include "pipeline_data.h"
#include <map>

namespace k3d
{

/// Abstract interface that can be used to store heterogeneous collections of arrays.
/// Methods are provided for cloning arrays (virtual ctor pattern), plus type-agnostic
/// methods similar to std::vector, and storage for arbitrary metadata (name-value pairs).
class array
{
public:
	/// Storage for array metadata
	typedef std::map<string_t, string_t> metadata_t;

	array();
	array(const metadata_t& Metadata);
	virtual ~array();

	/// Returns an empty array with the same concrete type as this array (a variation on virtual ctor)
	virtual array* clone_type() const = 0;
	/// Returns a copy of this array (virtual ctor)
	virtual array* clone() const = 0;
	/// Returns a copy of a half-open range of this array (a variation on virtual ctor)
	virtual array* clone(const uint_t Begin, const uint_t End) const = 0;

	/// Sets the size of this array.
	virtual void resize(const uint_t NewSize) = 0;
	/// Returns the size of this array
	virtual const uint_t size() const = 0;
	/// Returns true iff this array is empty
	virtual const bool_t empty() const = 0;
	/// Returns true iff this array is equivalent to another, using the imprecise semantics of almost_equal to compare values.
	/// \note: Returns false if given an array with a different concrete type.
	virtual const bool_t almost_equal(const array& Other, const uint64_t Threshold) const = 0;

	/// Sets a new name-value pair, overwriting the value if the name already exists
	void set_metadata_value(const string_t& Name, const string_t& Value);
	/// Sets a collection of name-value pairs, overwriting any existing values
	void set_metadata(const metadata_t& Values);
	/// Returns the set of all name-value pairs
	metadata_t get_metadata() const;
	/// Returns a value by name, or empty-string if the name doesn't exist
	const string_t get_metadata_value(const string_t& Name) const;
	/// Erases an existing name-value pair
	void erase_metadata_value(const string_t& Name);

protected:
	/// Storage for array metadata
	metadata_t metadata;
};

/// Specialization of almost_equal that tests k3d::array for equality
template<>
class almost_equal<array>
{
	typedef array T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool operator()(const T& A, const T& B) const
	{
		return A.almost_equal(B, threshold);
	}

	const uint64_t threshold;
};

/// Specialization of pipeline_data_traits for use with k3d::array
template<>
class pipeline_data_traits<array>
{
public:
	static array* clone(const array& Other)
	{
		return Other.clone();
	}
};

} // namespace k3d

#endif // !K3DSDK_ARRAY_H

