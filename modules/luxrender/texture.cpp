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

#include "texture.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/iomanip.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// texture

texture::texture(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
	k3d::node(Factory, Document)
{
}

void texture::setup(name_map& TextureNames, std::ostream& Stream)
{
	if(TextureNames.count(this))
		return;

	std::ostringstream name_buffer;
	name_buffer << "Texture" << TextureNames.size();
	const k3d::string_t name = name_buffer.str();

	TextureNames.insert(std::make_pair(this, name));

	on_setup(TextureNames, name, Stream);
}

void texture::use(const name_map& TextureNames, k3d::iunknown* const Texture, const k3d::string_t& Name, std::ostream& Stream)
{
	// Insert a reference to an existing texture ...
	if(texture* const lux_texture = dynamic_cast<texture*>(Texture))
	{
		if(TextureNames.count(lux_texture))
		{
			Stream << k3d::standard_indent << "\"texture " << Name << "\" [\"" << TextureNames.find(lux_texture)->second << "\"]\n";
			return;
		}

		k3d::log() << error << "Missing named texture." << std::endl;
	}
}

} // namespace luxrender

} // namespace module

