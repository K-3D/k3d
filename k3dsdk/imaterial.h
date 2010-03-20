#ifndef K3DSDK_IMATERIAL_H
#define K3DSDK_IMATERIAL_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/difference.h>
#include <k3dsdk/iunknown.h>

namespace k3d
{

/// Abstract interface implemented by objects that can act as geometric surface materials
class imaterial :
	public virtual iunknown
{
protected:
	imaterial() {}
	imaterial(const imaterial& Other) : iunknown(Other) {}
	imaterial& operator=(const imaterial&) { return *this; }
	virtual ~imaterial() {}
};

/// Specialization of difference::test that tests imaterial pointers for equality
namespace difference
{

inline void test(imaterial* const A, imaterial* const B, accumulator& Result)
{
	Result.exact(A == B);
}

} // namespace difference

} // namespace k3d

#endif // !K3DSDK_IMATERIAL_H

