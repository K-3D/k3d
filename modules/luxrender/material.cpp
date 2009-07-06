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

#include "material.h"
#include "scalar_texture.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/material.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// material

material::material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
	k3d::node(Factory, Document),
	m_bumpmap(
		init_owner(*this)
		+ init_name("bumpmap")
		+ init_label(_("Bumpmap"))
		+ init_description(_("Optional bumpmap texture."))
		+ init_value(static_cast<scalar_texture*>(0))
		)
{
}

void material::setup(k3d::imaterial* const Material, std::ostream& Stream)
{
  if(material* const lux_material = k3d::material::lookup<material>(Material))
  {
    lux_material->setup(Stream);
    return;
  }

  // Provide a default material ...
  Stream << k3d::standard_indent << "Texture \"a\" \"color\" \"constant\" \"color value\" [1 1 1]\n";
  Stream << k3d::standard_indent << "Texture \"b\" \"color\" \"constant\" \"color value\" [0.1 0.1 0.1]\n";
  Stream << k3d::standard_indent << "Texture \"c\" \"float\" \"constant\" \"float value\" [0.000571]\n";
  Stream << k3d::standard_indent << "Material \"plastic\" \"texture Kd\" \"a\" \"texture Ks\" \"b\" \"texture bumpmap\" \"c\"\n";
}

void material::setup_bumpmap(const k3d::string_t& Name, std::ostream& Stream)
{
	if(scalar_texture* const texture = dynamic_cast<scalar_texture*>(m_bumpmap.pipeline_value()))
	{
		texture->setup_scalar_texture(Name, Stream);
		return;
	}

	Stream << k3d::standard_indent << "Texture \"" << Name << "\" \"float\" \"constant\" \"float value\" [0.0]\n";
}

} // namespace luxrender

} // namespace module

