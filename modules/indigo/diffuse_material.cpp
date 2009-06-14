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

namespace indigo
{

/////////////////////////////////////////////////////////////////////////////
// material

class diffuse_material :
	public k3d::node ,
  public indigo::material
{
	typedef k3d::node base;

public:
	diffuse_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(0.8, 0.8, 0.8)))
	{
	}

  void setup(const k3d::string_t& Name, std::ostream& Stream)
  {
    Stream << k3d::standard_indent << "<material>\n" << k3d::push_indent;
    Stream << k3d::standard_indent << "<name>" << Name << "</name>\n";
    Stream << k3d::standard_indent << "<diffuse>\n" << k3d::push_indent;
    Stream << k3d::standard_indent << "<colour>" << m_color.pipeline_value() << "</colour>\n";
    Stream << k3d::pop_indent << k3d::standard_indent << "</diffuse>\n";
    Stream << k3d::pop_indent << k3d::standard_indent << "</material>\n";
  }

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<diffuse_material,
				k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0x7fbdeb17, 0xce413e5d, 0x65f51688, 0xa789b421),
			"IndigoDiffuseMaterial",
			_("Indigo Diffuse Material"),
			"Indigo Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

k3d::iplugin_factory& diffuse_material_factory()
{
	return diffuse_material::get_factory();
}

} // namespace indigo

} // namespace module

