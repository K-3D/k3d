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

class null_material :
	public material
{
	typedef material base;

public:
	null_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<null_material,
			k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0xb55cd32a, 0x4b4f4bd2, 0x317685bc, 0x14327799),
			"LuxRenderNullMaterial",
			_("LuxRender Null Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_setup(const texture::name_map& TextureNames, material::name_map& MaterialNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "MakeNamedMaterial \"" << Name << "\"\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "\"string type\" [\"null\"]\n";
		setup_bumpmap(TextureNames, Stream);
		Stream << k3d::pop_indent;
	}
};

k3d::iplugin_factory& null_material_factory()
{
	return null_material::get_factory();
}

} // namespace luxrender

} // namespace module

