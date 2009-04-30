#ifndef K3DSDK_ATTRIBUTE_ARRAY_COPIER_H
#define K3DSDK_ATTRIBUTE_ARRAY_COPIER_H

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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include "attribute_arrays.h"

namespace k3d
{

/// Handles random-access copying among attribute arrays
class attribute_array_copier
{
public:
	/// Abstract interface for policy objects that determine which source arrays get copied to which target arrays.
	class copy_policy
	{
	protected:
		copy_policy() {}
		~copy_policy() {}

	public:
		/// Return true to indicate that the given source array should be copied to the given target array.  Note that this method
		/// will be called once for every permutation of source and unused-target arrays, so a derivative could implement special behavior
		/// including mapping source and target arrays with different names, mapping a single source array to multiple target arrays, etc.
		virtual bool_t copy(const string_t& SourceName, const array& Source, const string_t& TargetName, const array& Target) const = 0;
		/// Called once for each source array that isn't used.  Implementations may optionally choose to generate errors.
		virtual void unused_source(const string_t& SourceName, const array& Source) const = 0;
		/// Called once for each target array that isn't used.  Implementations may optionally choose to generate errors.
		virtual void unused_target(const string_t& TargetName, const array& Target) const = 0; 
	};

	/// Strict copy policy that matches arrays by name, generating errors for all unused arrays and arrays that match names but not types.
	/// This policy is useful in the majority of cases where a node is copying data from one mesh input to one mesh output and the set of
	/// arrays is identical for both.
	class strict_copy :
		public copy_policy
	{
	public:
		bool_t copy(const string_t& SourceName, const array& Source, const string_t& TargetName, const array& Target) const; 
		void unused_source(const string_t& SourceName, const array& Source) const; 
		void unused_target(const string_t& TargetName, const array& Target) const; 
	};

	/// Copy policy that matches arrays by name, quietly ignoring unused source arrays and arrays with mismatched types. This policy is useful
	/// for nodes that are merging data from multiple input meshes to a single output mesh, where the output will contain a subset (usually
	/// the intersection) of all available input arrays.
	class copy_subset :
		public copy_policy
	{
	public:
		bool_t copy(const string_t& SourceName, const array& Source, const string_t& TargetName, const array& Target) const; 
		void unused_source(const string_t& SourceName, const array& Source) const; 
		void unused_target(const string_t& TargetName, const array& Target) const; 
	};

	/// Initializes attribute_array_copier to copy data from a collection of source arrays to a collection of target arrays, using a copy_policy
	/// object to determine how each source array maps to each target array.
	attribute_array_copier(const attribute_arrays& Source, attribute_arrays& Target, const copy_policy& CopyPolicy = strict_copy());
	~attribute_array_copier();

	/// Appends the given index value from each source array to each corresponding target array.
	void push_back(const uint_t Index);
	/// Computes a weighted sum of N values from each source array and appends the result to the corresponding target array.
	void push_back(const uint_t Count, const uint_t* Indices, const double_t* Weights);
	/// Copies the given source index value from each source array to the TargetIndex in each corresponding target array.
	void copy(const uint_t SourceIndex, const uint_t TargetIndex);
	/// Computes a weighted sum of N values from each source array and copies the result to the corresponding target array at the given TargetIndex.
	void copy(const uint_t Count, const uint_t* Indices, const double_t* Weights, const uint_t TargetIndex);

private:
	class implementation;
	implementation* const m_implementation;

	attribute_array_copier(const attribute_array_copier&);
	attribute_array_copier& operator=(const attribute_array_copier&);
};

} // namespace k3d

#endif // !K3DSDK_ATTRIBUTE_ARRAY_COPIER_H

