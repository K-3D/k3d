#ifndef K3DSDK_PARTICLE_H
#define K3DSDK_PARTICLE_H

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

#include <k3dsdk/mesh.h>

namespace k3d
{

namespace particle
{

/// Gathers the member arrays of a particle primitive into a convenient package
class const_primitive
{
public:
	const_primitive(
		const typed_array<imaterial*>& Material,
		const uint_t_array& Points,
		const table& ConstantAttributes,
		const table& VertexAttributes
		);

	const typed_array<imaterial*>& material;
	const uint_t_array& points;
	const table& constant_attributes;
	const table& vertex_attributes;
};

/// Gathers the member arrays of a particle primitive into a convenient package
class primitive
{
public:
	primitive(
		typed_array<imaterial*>& Material,
		uint_t_array& Points,
		table& ConstantAttributes,
		table& VertexAttributes
		);

	typed_array<imaterial*>& material;
	uint_t_array& points;
	table& constant_attributes;
	table& vertex_attributes;
};

/// Creates a new particle mesh primitive, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
primitive* create(mesh& Mesh);

/// Tests the given mesh primitive to see if it is a valid particle primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_primitive* validate(const mesh& Mesh, const mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid particle primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(const mesh& Mesh, mesh::primitive& GenericPrimitive);
/// Tests the given mesh primitive to see if it is a valid particle primitive, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
primitive* validate(const mesh& Mesh, pipeline_data<mesh::primitive>& GenericPrimitive);

} // namespace particle

} // namespace k3d

#endif // !K3DSDK_PARTICLE_H

