#ifndef K3DSDK_DEPENDENCIES_H
#define K3DSDK_DEPENDENCIES_H

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
		\brief Declares the k3d::dependencies class, which provides an implementation of k3d::idependencies
		\author Tim Shead (tshead@k-3d.com)
*/

#include "idependencies.h"
#include <map>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// dependencies

class dependencies :
	public idependencies
{
public:
	void register_object_dependency(inode& Object, const bool CopyByReference);

	typedef std::map<inode*, bool> objects_t;
	const objects_t& objects() const;
	
private:
	// The set of dependent objects ...
	objects_t m_objects;
};

} // namespace k3d

#endif // !K3DSDK_DEPENDENCIES_H
