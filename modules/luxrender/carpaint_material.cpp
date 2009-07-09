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
// name_values

static const k3d::ilist_property<std::string>::values_t& name_values()
{
	static k3d::ilist_property<std::string>::values_t values;
	if(values.empty())
	{
		values.push_back("ford f8");
		values.push_back("polaris silber");
		values.push_back("opel titan");
		values.push_back("bmw339");
		values.push_back("2k acrylack");
		values.push_back("white");
		values.push_back("blue");
		values.push_back("blue matte");
		values.push_back("<custom>");
	}
	return values;
}

/////////////////////////////////////////////////////////////////////////////
// material

class carpaint_material :
	public material
{
	typedef material base;

public:
	carpaint_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_name(init_owner(*this) + init_name("name") + init_label(_("Name")) + init_description(_("Paint name.")) + init_value(k3d::string_t("ford f8")) + init_values(name_values())),
		m_kd(*this, "kd", _("Kd"), _("Diffuse Color"), _("Kd Texture"), k3d::color(0.2, 0.2, 0.4)),
		m_ks1(*this, "ks1", _("Ks1"), _("Specular Color 1"), _("Ks1 Texture"), k3d::color(0.2, 0.2, 0.4)),
		m_ks2(*this, "ks2", _("Ks2"), _("Specular Color 2"), _("Ks2 Texture"), k3d::color(0.2, 0.2, 0.4)),
		m_ks3(*this, "ks3", _("Ks3"), _("Specular Color 3"), _("Ks3 Texture"), k3d::color(0.2, 0.2, 0.4)),
		m_r1(*this, "r1", _("R1"), _("Fresnel Constant 1"), _("R1 Texture"), 0.1),
		m_r2(*this, "r2", _("R2"), _("Fresnel Constant 2"), _("R2 Texture"), 0.1),
		m_r3(*this, "r3", _("R3"), _("Fresnel Constant 3"), _("R3 Texture"), 0.1),
		m_m1(*this, "m1", _("M1"), _("Microfacet Roughness 1"), _("M1 Texture"), 0.1),
		m_m2(*this, "m2", _("M2"), _("Microfacet Roughness 2"), _("M2 Texture"), 0.1),
		m_m3(*this, "m3", _("M3"), _("Microfacet Roughness 3"), _("M3 Texture"), 0.1)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<carpaint_material,
			k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0xa8a9550f, 0x0c496f29, 0xf31fc2a8, 0xa7467491),
			"LuxRenderCarpaintMaterial",
			_("LuxRender Carpaint Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_setup(const texture::name_map& TextureNames, material::name_map& MaterialNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		const k3d::string_t name = m_name.pipeline_value();

		Stream << k3d::standard_indent << "MakeNamedMaterial \"" << Name << "\"\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "\"string type\" [\"carpaint\"]\n";
		Stream << k3d::standard_indent << "\"string name\" [\"" << name << "\"]\n";
		setup_bumpmap(TextureNames, Stream);

		if(name == "<custom>")
		{
			m_kd.setup(TextureNames, "color", "Kd", Stream);
			m_ks1.setup(TextureNames, "color", "Ks1", Stream);
			m_ks2.setup(TextureNames, "color", "Ks2", Stream);
			m_ks3.setup(TextureNames, "color", "Ks3", Stream);
			m_r1.setup(TextureNames, "float", "R1", Stream);
			m_r2.setup(TextureNames, "float", "R2", Stream);
			m_r3.setup(TextureNames, "float", "R3", Stream);
			m_m1.setup(TextureNames, "float", "M1", Stream);
			m_m2.setup(TextureNames, "float", "M2", Stream);
			m_m3.setup(TextureNames, "float", "M3", Stream);
		}

		Stream << k3d::pop_indent;
	}

	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_name;
	color_texture_reference m_kd;
	color_texture_reference m_ks1;
	color_texture_reference m_ks2;
	color_texture_reference m_ks3;
	scalar_texture_reference m_r1;
	scalar_texture_reference m_r2;
	scalar_texture_reference m_r3;
	scalar_texture_reference m_m1;
	scalar_texture_reference m_m2;
	scalar_texture_reference m_m3;
};

k3d::iplugin_factory& carpaint_material_factory()
{
	return carpaint_material::get_factory();
}

} // namespace luxrender

} // namespace module

