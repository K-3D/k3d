#ifndef K3DSDK_TYPES_H
#define K3DSDK_TYPES_H

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

/** \file
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-platform-config.h>
#include "Half/half.h"
#include <boost/cstdint.hpp>
#include <string>

namespace k3d
{

typedef bool bool_t;

typedef boost::int8_t int8_t;
typedef boost::int16_t int16_t;
typedef boost::int32_t int32_t;
typedef boost::int64_t int64_t;

typedef boost::uint8_t uint8_t;
typedef boost::uint16_t uint16_t;
typedef boost::uint32_t uint32_t;
typedef boost::uint64_t uint64_t;

typedef ::half half_t;
typedef float float_t;
typedef double double_t;

typedef std::string string_t;

#if defined K3D_UINT_T_32_BITS
	typedef uint32_t uint_t;
#elif defined K3D_UINT_T_64_BITS
	typedef uint64_t uint_t;
#else
	#error "Unknown size for k3d::uint_t"
#endif

} // namespace k3d

#endif // K3DSDK_TYPES_H

