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

#include "shader_collection_ri.h"

namespace k3d
{

namespace ri
{

/////////////////////////////////////////////////////////////////////////////
// shader_collection::implementation

class shader_collection::implementation
{
public:
	/// Stores a set of shaders
	shaders_t m_shaders;
};

/////////////////////////////////////////////////////////////////////////////
// shader_collection

shader_collection::shader_collection() :
	m_implementation(new implementation())
{
}

shader_collection::~shader_collection()
{
	delete m_implementation;
}

void shader_collection::use_shader(const path& Path)
{
	m_implementation->m_shaders.insert(Path);
}

const shader_collection::shaders_t& shader_collection::shaders() const
{
	return m_implementation->m_shaders;
}

} // namespace ri

} // namespace k3d

