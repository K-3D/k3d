#ifndef K3DSDK_NAMED_ARRAYS_H
#define K3DSDK_NAMED_ARRAYS_H

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

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

namespace k3d
{

class array;

/// Defines a heterogeneous collection of named, shared arrays
class named_arrays :
	public std::map<std::string, boost::shared_ptr<array> >
{
public:
	/// Returns an object containing empty arrays with the same name and type as the originals
	named_arrays clone_types() const;
	/// Returns an object containing deep copies of all the original arrays
	named_arrays clone() const;
	/// Returns an object containing copies of a half-open range of all the original arrays
	named_arrays clone(size_t Begin, size_t End) const;
};

} // namespace k3d

#endif // K3DSDK_NAMED_ARRAYS_H

