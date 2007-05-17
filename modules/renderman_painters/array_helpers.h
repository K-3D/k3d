#ifndef LIBK3DRENDERMAN_ARRAY_HELPERS_H
#define LIBK3DRENDERMAN_ARRAY_HELPERS_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/mesh.h>
#include <k3dsdk/types_ri.h>

namespace libk3drendermanpainters
{

/// Makes an internal copy of a set of named mesh arrays
class array_copier
{
public:
	array_copier();
	~array_copier();

	/// Adds a collection of named mesh arrays to be copied
	void add_arrays(const k3d::mesh::named_arrays& Sources);
	/// Adds a single named mesh array to be copied
	void add_array(const std::string& Name, const k3d::array& Source);

	/// Copies a half-open range of values from the source arrays into the internal copy
	void insert(const size_t Begin, const size_t End);
	/// Copies an individual set of values from the source arrays into the internal copy
	void push_back(const size_t Index);

	/// Assigns the internal copy to the given RenderMan parameter list
	void copy_to(const k3d::ri::storage_class_t StorageClass, k3d::ri::parameter_list& Destination);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace libk3drendermanpainters

#endif // !LIBK3DRENDERMAN_ARRAY_HELPERS_H

