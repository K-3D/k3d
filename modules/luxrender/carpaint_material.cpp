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

class carpaint_material :
	public k3d::node ,
  public luxrender::material
{
	typedef k3d::node base;

public:
	carpaint_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_name(init_owner(*this) + init_name("name") + init_label(_("Name")) + init_description(_("Paint name.")) + init_value(k3d::string_t("ford f8")) + init_values(wrap_names()))
	{
	}

  void setup(std::ostream& Stream)
  {
    Stream << k3d::standard_indent << "Material \"carpaint\" \"string name\" \"" << m_name.pipeline_value() << "\" \n";
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
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_name;

	static const k3d::ilist_property<std::string>::values_t& wrap_names()
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
			values.push_back("");
		}
		return values;
	}
};

k3d::iplugin_factory& carpaint_material_factory()
{
	return carpaint_material::get_factory();
}

} // namespace luxrender

} // namespace module

