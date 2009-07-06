#ifndef MODULES_LUXRENDER_SCALAR_TEXTURE_REFERENCE_H
#define MODULES_LUXRENDER_SCALAR_TEXTURE_REFERENCE_H

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

#include "color_texture.h"

#include <k3dsdk/color.h>
#include <k3dsdk/data.h>
#include <k3dsdk/itexture.h>
#include <k3dsdk/tokens.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// scalar_texture_reference

/// Wraps a set of K-3D properties for use as a LuxRender color texture.
class scalar_texture_reference
{
public:
	template<typename OwnerT>
	scalar_texture_reference(OwnerT& Owner, const char* const Name, const char* const Label, const char* const Description, const char* TextureLabel, const k3d::double_t Value) :
	m_scalar(
		init_owner(Owner)
		+ init_name(Name)
		+ init_label(Label)
		+ init_description(Description)
		+ init_value(Value)
		),
	m_texture(
		init_owner(Owner)
		+ init_name(k3d::make_token((k3d::string_t(Name) + "_texture").c_str()))
		+ init_label(TextureLabel)
		+ init_description(Description)
		+ init_value(static_cast<scalar_texture*>(0))
		)
	{
	}

/// Inserts the K-3D state into a scene, ready for use by a material.
void setup(const k3d::string_t& Name, std::ostream& Stream);

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_scalar;
	k3d_data(scalar_texture*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_texture;
};

} // namespace luxrender

} // namespace module

#endif // !MODULES_LUXRENDER_SCALAR_TEXTURE_REFERENCE_H

