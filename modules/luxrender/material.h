#ifndef MODULES_LUXRENDER_MATERIAL_H
#define MODULES_LUXRENDER_MATERIAL_H

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

#include "scalar_texture.h"
#include "texture.h"

#include <k3dsdk/data.h>
#include <k3dsdk/imaterial.h>
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
// material

/// Abstract interface for LuxRender material nodes.
class material :
	public k3d::node,
	public k3d::imaterial
{
public:
	material(k3d::iplugin_factory& Factory, k3d::idocument& Document);

	/// Provides storage for a mapping from material objects to unique names
	typedef std::map<material*, k3d::string_t> name_map;
	/// Inserts a material definition into a scene, assigning the material a unique name
	void setup(const texture::name_map& TextureNames, name_map& MaterialNames, std::ostream& Stream);
	/// Inserts a material reference into a scene, handling NULL materials by supplying a standard default.
	static void use(const name_map& MaterialNames, k3d::imaterial* const Material, std::ostream& Stream);

protected:
	void setup_bumpmap(const texture::name_map& TextureNames, std::ostream& Stream);

private:
	/// Implemented in derivatives to do the actual work of inserting the material definition into the scene.
	virtual void on_setup(const texture::name_map& TextureNames, name_map& MaterialNames, const k3d::string_t& Name, std::ostream& Stream) = 0;

	k3d_data(scalar_texture*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_bumpmap;
};

} // namespace luxrender

} // namespace module

#endif // !MODULES_LUXRENDER_MATERIAL_H

