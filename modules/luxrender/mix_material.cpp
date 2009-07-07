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
#include "scalar_texture_reference.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// material

class mix_material :
	public material
{
	typedef material base;

public:
	mix_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_material_1(init_owner(*this) + init_name("material1") + init_label(_("Material 1")) + init_description(_("Material 1")) + init_value(static_cast<material*>(0))),
		m_material_2(init_owner(*this) + init_name("material2") + init_label(_("Material 2")) + init_description(_("Material 2")) + init_value(static_cast<material*>(0))),
		m_amount(*this, "amount", _("Amount"), _("Used to do linear interpolation between Material 1 and Material 2."), _("Amount Texture"), 0.5)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mix_material,
			k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0x63320de7, 0x6043a28b, 0xd214d7aa, 0x10682945),
			"LuxRenderMixMaterial",
			_("LuxRender Mix Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_setup(const texture::name_map& TextureNames, material::name_map& MaterialNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		material* const material1 = m_material_1.pipeline_value();
		if(material1)
			material1->setup(TextureNames, MaterialNames, Stream);

		material* const material2 = m_material_2.pipeline_value();
		if(material2)
			material2->setup(TextureNames, MaterialNames, Stream);

		Stream << k3d::standard_indent << "MakeNamedMaterial \"" << Name << "\"\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "\"string type\" [\"mix\"]\n";

		setup_bumpmap(TextureNames, Stream);

		if(material1 && MaterialNames.count(material1))
			Stream << k3d::standard_indent << "\"string namedmaterial1\" [\"" << MaterialNames.find(material1)->second << "\"]\n";

		if(material2 && MaterialNames.count(material2))
			Stream << k3d::standard_indent << "\"string namedmaterial2\" [\"" << MaterialNames.find(material2)->second << "\"]\n";

		m_amount.setup(TextureNames, "float", "amount", Stream);

		Stream << k3d::pop_indent;
	}

	k3d_data(material*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_material_1;
	k3d_data(material*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_material_2;
	scalar_texture_reference m_amount;
};

k3d::iplugin_factory& mix_material_factory()
{
	return mix_material::get_factory();
}

} // namespace luxrender

} // namespace module

