#ifndef GTS_HELPERS_H
#define GTS_HELPERS_H

// K-3D
// Copyright (c) 2004-2007, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
	\brief Defines GTS interface: data types and functions
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <gts.h>

namespace k3d { class mesh; }

namespace module
{

namespace gts
{

/// Converts a K-3D mesh into a GtsSurface
GtsSurface* convert(const k3d::mesh& Mesh);

/// Smart pointer for holding GTS objects
template<typename T>
class gts_ptr
{
public:
	gts_ptr(T* const Object) :
		m_object(Object)
	{
	}

	~gts_ptr()
	{
		if(m_object)
			gts_object_destroy(GTS_OBJECT(m_object));
	}

	operator T*() const
	{
		return m_object;
	}

	T* operator->() const
	{
		return m_object;
	}

private:
	T* const m_object;
};

} // namespace gts

} // namespace module

#endif // GTS_HELPERS_H

