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

class matte_material :
	public material
{
	typedef material base;

public:
	matte_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(*this, "kd", _("Color"), _("Diffuse Color."), _("Color Texture"), k3d::color(0.8, 0.8, 0.8)),
		m_sigma(*this, "sigma", _("Sigma"), _("The sigma parameter in the Oren-Nayer shader in degrees.  Zero for pure Lambertian reflection."), _("Sigma Texture"), 0.0)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<matte_material,
			k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0x77bbeecb, 0x59483223, 0x142c92b9, 0x235096bb),
			"LuxRenderMatteMaterial",
			_("LuxRender Matte Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_setup(material::name_map& MaterialNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		setup_bumpmap("a", Stream);
		m_color.setup("b", Stream);
		m_sigma.setup("c", Stream);

		Stream << k3d::standard_indent << "MakeNamedMaterial \"" << Name << "\"";
		Stream << " \"string type\" [\"matte\"]";
		Stream << " \"texture bumpmap\" [\"a\"]";
		Stream << " \"texture Kd\" [\"b\"]";
		Stream << " \"texture sigma\" [\"c\"]";
		Stream << "\n";
	}

	color_texture_reference m_color;
	scalar_texture_reference m_sigma;
};

k3d::iplugin_factory& matte_material_factory()
{
	return matte_material::get_factory();
}

} // namespace luxrender

} // namespace module

