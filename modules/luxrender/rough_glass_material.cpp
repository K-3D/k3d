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

class rough_glass_material :
	public material
{
	typedef material base;

public:
	rough_glass_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_kr(*this, "kr", _("Kr"), _("Surface reflectivity."), _("Kr Texture"), k3d::color(1, 1, 1)),
		m_kt(*this, "kt", _("Kt"), _("Fraction of light transmitted through the surface."), _("Kt Texture"), k3d::color(1, 1, 1)),
		m_u_roughness(*this, "u_roughness", _("U Roughness"), _("U Roughness."), _("U Roughness Texture"), 0.001),
		m_v_roughness(*this, "v_roughness", _("V Roughness"), _("V Roughness."), _("V Roughness Texture"), 0.001),
		m_index(*this, "index", _("Index"), _("Index of refraction."), _("Index Texture"), 1.5),
		m_cauchy_b(*this, "cauchy_b", _("Cauchy B"), _("Cauchy B coefficient."), _("Cauchy B Texture"), 0.0)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<rough_glass_material,
			k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0x4f3bbd0c, 0x2746e725, 0xb37e4b9a, 0x1a7a0299),
			"LuxRenderRoughGlassMaterial",
			_("LuxRender Rough Glass Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_setup(const texture::name_map& TextureNames, material::name_map& MaterialNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "MakeNamedMaterial \"" << Name << "\"\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "\"string type\" [\"roughglass\"]\n";
		
		setup_bumpmap(TextureNames, Stream);
		m_kr.setup(TextureNames, "color", "Kr", Stream);
		m_kt.setup(TextureNames, "color", "Kt", Stream);
		m_u_roughness.setup(TextureNames, "float", "uroughness", Stream);
		m_v_roughness.setup(TextureNames, "float", "vroughness", Stream);
		m_index.setup(TextureNames, "float", "index", Stream);
		m_cauchy_b.setup(TextureNames, "float", "cauchyb", Stream);

		Stream << k3d::pop_indent;
	}

	color_texture_reference m_kr;
	color_texture_reference m_kt;
	scalar_texture_reference m_u_roughness;
	scalar_texture_reference m_v_roughness;
	scalar_texture_reference m_index;
	scalar_texture_reference m_cauchy_b;
};

k3d::iplugin_factory& rough_glass_material_factory()
{
	return rough_glass_material::get_factory();
}

} // namespace luxrender

} // namespace module

