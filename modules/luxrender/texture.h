#ifndef MODULES_LUXRENDER_TEXTURE_H
#define MODULES_LUXRENDER_TEXTURE_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Tim Shead <tshead@k-3d.com>
*/

#include <k3dsdk/itexture.h>
#include <k3dsdk/node.h>
#include <k3dsdk/types.h>

#include <iosfwd>
#include <map>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// texture

/// Abstract interface for LuxRender texture objects
class texture :
	public k3d::node,
	public k3d::itexture
{
public:
	texture(k3d::iplugin_factory& Factory, k3d::idocument& Document);

	/// Provides storage for a mapping from texture objects to unique names
	typedef std::map<texture*, k3d::string_t> name_map;
	/// Inserts a texture definition into a scene, assigning the texture a unique name
	void setup(name_map& TextureNames, std::ostream& Stream);
	/// Inserts a texture reference into a scene, handling NULL textures gracefully.
	static void use(const name_map& TextureNames, k3d::iunknown* const Texture, const k3d::string_t& Name, std::ostream& Stream);

private:
	/// Implemented in derivatives to do the actual work of inserting the texture definition into the scene.
	virtual void on_setup(name_map& TextureNames, const k3d::string_t& Name, std::ostream& Stream) = 0;
};

} // namespace luxrender

} // namespace module

#endif // !MODULES_LUXRENDER_TEXTURE_H

