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

class glass_material :
	public material
{
	typedef material base;

public:
	glass_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_kr(*this, "kr", _("Kr"), _("Surface reflectivity."), _("Kr Texture"), k3d::color(1, 1, 1)),
		m_kt(*this, "kt", _("Kt"), _("Fraction of light transmitted through the surface."), _("Kt Texture"), k3d::color(1, 1, 1)),
		m_index(*this, "index", _("Index"), _("Index of refraction."), _("Index Texture"), 1.5),
		m_cauchy_b(*this, "cauchy_b", _("Cauchy B"), _("Cauchy B coefficient."), _("Cauchy B Texture"), 0.0),
		m_film(*this, "film", _("Film"), _("Thin film coating thickness (nanometers)."), _("Film Texture"), 0.001),
		m_film_index(*this, "film_index", _("Film Index"), _("Index of refraction of the thin film coating."), _("Film Index Texture"), 1.5)
	{
	}

	void setup(std::ostream& Stream)
	{
		setup_bumpmap("a", Stream);
		m_kr.setup("b", Stream);
		m_kt.setup("c", Stream);
		m_index.setup("d", Stream);
		m_cauchy_b.setup("e", Stream);
		m_film.setup("f", Stream);
		m_film_index.setup("g", Stream);

		Stream << k3d::standard_indent << "Material \"glass\"";
		Stream << " \"texture bumpmap\" \"a\"";
		Stream << " \"texture Kr\" \"b\"";
		Stream << " \"texture Kt\" \"c\"";
		Stream << " \"texture index\" \"d\"";
		Stream << " \"texture cauchyb\" \"e\"";
		Stream << " \"texture film\" \"f\"";
		Stream << " \"texture filmindex\" \"g\"";
		Stream << "\n";
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<glass_material,
			k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0x3fd264f3, 0xd7478ccc, 0xb6992185, 0x4c0c9dd2),
			"LuxRenderGlassMaterial",
			_("LuxRender Glass Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	color_texture_reference m_kr;
	color_texture_reference m_kt;
	scalar_texture_reference m_index;
	scalar_texture_reference m_cauchy_b;
	scalar_texture_reference m_film;
	scalar_texture_reference m_film_index;
};

k3d::iplugin_factory& glass_material_factory()
{
	return glass_material::get_factory();
}

} // namespace luxrender

} // namespace module

