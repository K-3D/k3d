#ifndef K3DSDK_SHADER_COLLECTION_RI_H
#define K3DSDK_SHADER_COLLECTION_RI_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "ishader_collection_ri.h"
#include <set>

namespace k3d
{

namespace ri
{

/////////////////////////////////////////////////////////////////////////////
// shader_collection

/// Provides a boilerplate implementation of k3d::ri::ishader_collection
class shader_collection :
	public ishader_collection
{
public:
	shader_collection();
	~shader_collection();

	void use_shader(const path& Path);

	/// Stores a collection of shader paths
	typedef std::set<path> shaders_t;
	/// Returns the set of shaders within the collection
	const shaders_t& shaders() const;

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace ri

} // namespace k3d

#endif // !K3DSDK_SHADER_COLLECTION_RI_H

