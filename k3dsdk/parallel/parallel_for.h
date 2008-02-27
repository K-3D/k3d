#ifndef K3DSDK_PARALLEL_FOR_H
#define K3DSDK_PARALLEL_FOR_H

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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-parallel-config.h>

#ifdef K3D_BUILD_PARALLEL
#include <tbb/parallel_for.h>
#endif // K3D_BUILD_PARALLEL

namespace k3d
{

namespace parallel
{

template<typename RangeT, typename BodyT>
void parallel_for(const RangeT& Range, const BodyT& Body);

#ifdef K3D_BUILD_PARALLEL

template<typename RangeT, typename BodyT>
void parallel_for(const RangeT& Range, const BodyT& Body)
{
	::tbb::parallel_for(Range, Body);
}

#else // K3D_BUILD_PARALLEL

template<typename RangeT, typename BodyT>
void parallel_for(const RangeT& Range, const BodyT& Body)
{
	Body(Range);
}

#endif // !K3D_BUILD_PARALLEL

} // namespace parallel

} // namespace k3d

#endif // K3DSDK_PARALLEL_FOR_H

