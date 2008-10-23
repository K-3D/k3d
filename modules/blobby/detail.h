#ifndef MODULES_BLOBBY_DETAIL_H
#define MODULES_BLOBBY_DETAIL_H

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
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/mesh.h>
#include <vector>

namespace module
{

namespace blobby
{

namespace detail
{

typedef std::vector<const k3d::mesh*> mesh_collection;

void merge(const mesh_collection& Inputs, k3d::imaterial* const Material, const k3d::mesh::blobbies_t::operator_type Operator, const k3d::bool_t VariableArguments, k3d::mesh& Output);

} // namespace detail

} // namespace blobby

} // namespace module

#endif // !MODULES_BLOBBY_DETAIL_H

