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

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// material

class glass_material :
	public k3d::node ,
  public luxrender::material
{
	typedef k3d::node base;

public:
	glass_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_Kr(*this, "kr", _("Kr"), _("Surface reflectivity."), k3d::color(1, 1, 1)),
		m_Kt(*this, "kt", _("Kt"), _("Fraction of light transmitted through the surface."), k3d::color(1, 1, 1)),
		m_index(init_owner(*this) + init_name("index") + init_label(_("Index")) + init_description(_("Index of refraction.")) + init_value(1.5)),
		m_cauchy_b(init_owner(*this) + init_name("cauchy_b") + init_label(_("Cauchy B")) + init_description(_("Cauchy B coefficient.")) + init_value(0.0)),
		m_film(init_owner(*this) + init_name("film") + init_label(_("Film")) + init_description(_("Thin film coating thickness (nanometers).")) + init_value(0.001)),
		m_film_index(init_owner(*this) + init_name("film_index") + init_label(_("Film Index")) + init_description(_("Index of refraction of the thin film coating.")) + init_value(1.5))
	{
	}

	void setup(std::ostream& Stream)
	{
		m_Kr.setup("a", Stream);
		m_Kt.setup("b", Stream);
		Stream << k3d::standard_indent << "Texture \"c\" \"float\" \"constant\" \"float value\" [" << m_index.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "Texture \"d\" \"float\" \"constant\" \"float value\" [" << m_cauchy_b.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "Texture \"e\" \"float\" \"constant\" \"float value\" [" << m_film.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "Texture \"f\" \"float\" \"constant\" \"float value\" [" << m_film_index.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "Material \"glass\" \"texture Kr\" \"a\" \"texture Kt\" \"b\" \"texture index\" \"c\" \"texture cauchyb\" \"d\" \"texture film\" \"e\" \"texture filmindex\" \"f\" \n";
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
	color_texture_reference m_Kr;
	color_texture_reference m_Kt;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_index;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cauchy_b;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_film;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_film_index;
};

k3d::iplugin_factory& glass_material_factory()
{
	return glass_material::get_factory();
}

} // namespace luxrender

} // namespace module

