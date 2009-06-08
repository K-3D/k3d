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
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace indigo
{

/////////////////////////////////////////////////////////////////////////////
// rectangle_light

class rectangle_light :
	public k3d::transformable<k3d::node>,
	public indigo::light
{
	typedef k3d::transformable<k3d::node> base;

public:
	rectangle_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Width")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Height")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
	}

  void setup(std::ostream& Stream)
  {
    Stream << "<rectanglelight>\n";
    Stream << "<pos>" << k3d::world_position(*this) << "</pos>\n";
    Stream << "<width>" << m_width.pipeline_value() << "</width>\n";
    Stream << "<height>" << m_height.pipeline_value() << "</height>\n";
    Stream <<
      "<spectrum>"
        "<blackbody>"
          "<temperature>6400</temperature>"
          "<gain>0.0001</gain>"
        "</blackbody>"
      "</spectrum>"
      "\n";
    Stream << "</rectanglelight>\n";

  }

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<rectangle_light,
				k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0xfe05aa8d, 0xcf485202, 0x82a460ba, 0xab3184a3),
			"IndigoRectangleLight",
			_("Indigo Rectangle Light"),
			"Indigo Rectangle Light",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
};

k3d::iplugin_factory& rectangle_light_factory()
{
	return rectangle_light::get_factory();
}

} // namespace indigo

} // namespace module

