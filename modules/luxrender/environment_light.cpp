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
#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/node.h>
#include <k3dsdk/path.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// environment_light

class environment_light :
	public k3d::node,
	public luxrender::light
{
	typedef k3d::node base;

public:
	environment_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("The color of the light.")) + init_value(k3d::color(1, 1, 1))),
		m_samples(init_owner(*this) + init_name("samples") + init_label(_("Samples")) + init_description(_("Shadow samples.")) + init_value(1)),
		m_map(init_owner(*this) + init_name("map") + init_label(_("Map")) + init_description(_("Path to an HDR environment map.")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("hdr"))
	{
	}

  void setup(std::ostream& Stream)
  {
    Stream << k3d::standard_indent << "AttributeBegin\n" << k3d::push_indent;
    Stream << k3d::standard_indent << "LightSource \"infinite\"\n" << k3d::push_indent;
    Stream << k3d::standard_indent << "\"color L\" [" << m_color.pipeline_value() << "]\n";
    Stream << k3d::standard_indent << "\"integer nsamples\" [" << m_samples.pipeline_value() << "]\n";
    Stream << k3d::standard_indent << "\"string mapname\" [\"" << m_map.pipeline_value().native_filesystem_string() << "\"]\n";
    Stream << k3d::pop_indent << k3d::pop_indent << k3d::standard_indent << "AttributeEnd\n";
  }

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<environment_light> factory(
			k3d::uuid(0x88c6e2b5, 0xb64bcd1e, 0x03861c9e, 0x034584f8),
			"LuxRenderEnvironmentLight",
			_("LuxRender EnvironmentLight"),
			"LuxRender Light",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_samples;
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, with_serialization) m_map;
};

k3d::iplugin_factory& environment_light_factory()
{
	return environment_light::get_factory();
}

} // namespace luxrender

} // namespace module

