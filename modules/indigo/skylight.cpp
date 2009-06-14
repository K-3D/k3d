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

#include "light.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/node.h>
#include <k3dsdk/vector3.h>

namespace module
{

namespace indigo
{

/////////////////////////////////////////////////////////////////////////////
// skylight

class skylight :
	public k3d::node,
	public indigo::light
{
	typedef k3d::node base;

public:
	skylight(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_direction(init_owner(*this) + init_name("direction") + init_label(_("Direction")) + init_description(_("Vector pointing to the sun.")) + init_value(k3d::vector3(0, 0, 1))),
		m_turbidity(init_owner(*this) + init_name("turbidity") + init_label(_("Turbidity")) + init_description(_("Controls sky clarity / haziness.")) + init_value(2.0)),
		m_extra_atmospheric(init_owner(*this) + init_name("extra_atmospheric") + init_label(_("Extra-Atmospheric")) + init_description(_("Computes skylight without atmospheric scattering.")) + init_value(false))
	{
	}

  void setup(std::ostream& Stream)
  {
    const k3d::vector3 sundir = m_direction.pipeline_value();

    Stream << k3d::standard_indent << "<skylight>\n" << k3d::push_indent;
    Stream << k3d::standard_indent << "<sundir>" << -sundir[0] << " " << sundir[1] << " " << sundir[2] << "</sundir>\n";
    Stream << k3d::standard_indent << "<turbidity>" << m_turbidity.pipeline_value() << "</turbidity>\n";
    Stream << k3d::standard_indent << "<extra_atmospheric>" << (m_extra_atmospheric.pipeline_value() ? "true" : "false") << "</extra_atmospheric>\n";
    Stream << k3d::pop_indent << k3d::standard_indent << "</skylight>\n";
  }

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<skylight> factory(
			k3d::uuid(0xc3a70c62, 0xa7425db2, 0x08a93ba7, 0x788d3e9b),
			"IndigoSkylight",
			_("Indigo Skylight"),
			"Indigo Light",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_direction;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_turbidity;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_extra_atmospheric;
};

k3d::iplugin_factory& skylight_factory()
{
	return skylight::get_factory();
}

} // namespace indigo

} // namespace module

