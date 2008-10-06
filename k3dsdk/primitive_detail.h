#ifndef K3DSDK_PRIMITIVE_DETAIL_H
#define K3DSDK_PRIMITIVE_DETAIL_H

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

namespace k3d
{

//@{

/// Helper macros for use with in mesh primitive validate() functions only!

#define require_const_array(Primitive, Name, Type) \
	const Type* const Name = Primitive.topology.lookup< Type >(#Name); \
	if(!Name) \
	{ \
		log() << error << "[" << Primitive.type << "] primitive missing [" << #Name << "] array" << std::endl; \
		return 0; \
	}

#define require_array(Primitive, Name, Type) \
	Type* const Name = Primitive.topology.writable< Type >(#Name); \
	if(!Name) \
	{ \
		log() << error << "[" << Primitive.type << "] primitive missing [" << #Name << "] array" << std::endl; \
		return 0; \
	}

#define require_const_attribute_arrays(Primitive, Name) \
	const attribute_arrays* const Name##_data = Primitive.attributes.lookup(#Name); \
	if(!Name##_data) \
	{ \
		log() << error << "[" << Primitive.type << "] primitive missing [" << #Name << "] attribute data" << std::endl; \
		return 0; \
	}

#define require_attribute_arrays(Primitive, Name) \
	attribute_arrays* const Name##_data = Primitive.attributes.writable(#Name); \
	if(!Name##_data) \
	{ \
		log() << error << "[" << Primitive.type << "] primitive missing [" << #Name << "] attribute data" << std::endl; \
		return 0; \
	}

//@}

} // namespace k3d

#endif // !K3DSDK_PRIMITIVE_DETAIL_H
