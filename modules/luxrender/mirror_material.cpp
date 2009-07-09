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

#include "color_texture_reference.h"
#include "material.h"
#include "scalar_texture_reference.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// material

class mirror_material :
	public material
{
	typedef material base;

public:
	mirror_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_kr(*this, "kr", _("Kr"), _("Surface reflectivity."), _("Kr Texture"), k3d::color(1, 1, 1)),
		m_film(*this, "film", _("Film"), _("Thin film coating thickness (nanometers)."), _("Film Texture"), 0.001),
		m_film_index(*this, "film_index", _("Film Index"), _("Index of refraction of the thin film coating."), _("Film Index Texture"), 1.5)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mirror_material,
			k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0x3969bf64, 0xb9419f4c, 0xe515a99c, 0x11cccf42),
			"LuxRenderMirrorMaterial",
			_("LuxRender Mirror Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_setup(const texture::name_map& TextureNames, material::name_map& MaterialNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "MakeNamedMaterial \"" << Name << "\"\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "\"string type\" [\"mirror\"]\n";
		
		setup_bumpmap(TextureNames, Stream);
		m_kr.setup(TextureNames, "color", "Kr", Stream);
		m_film.setup(TextureNames, "float", "film", Stream);
		m_film_index.setup(TextureNames, "float", "filmindex", Stream);

		Stream << k3d::pop_indent;
	}

	color_texture_reference m_kr;
	scalar_texture_reference m_film;
	scalar_texture_reference m_film_index;
};

k3d::iplugin_factory& mirror_material_factory()
{
	return mirror_material::get_factory();
}

} // namespace luxrender

} // namespace module

