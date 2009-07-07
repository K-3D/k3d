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
#include <k3dsdk/vector3.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// distant_light

class distant_light :
	public k3d::node,
	public luxrender::light
{
	typedef k3d::node base;

public:
	distant_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_from(init_owner(*this) + init_name("from") + init_label(_("From")) + init_description(_("Light origin.")) + init_value(k3d::point3(0, 0, 1))),
		m_to(init_owner(*this) + init_name("to") + init_label(_("To")) + init_description(_("Light target.")) + init_value(k3d::point3(0, 0, 0))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Light color.")) + init_value(k3d::color(1, 1, 1)))
	{
	}

	void setup(std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "AttributeBegin\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "LightSource \"distant\"\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "\"point from\" [" << convert(m_from.pipeline_value()) << "]\n";
		Stream << k3d::standard_indent << "\"point to\" [" << convert(m_to.pipeline_value()) << "]\n";
		Stream << k3d::standard_indent << "\"color L\" [" << m_color.pipeline_value() << "]\n";
		Stream << k3d::pop_indent << k3d::pop_indent << k3d::standard_indent << "AttributeEnd\n";
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<distant_light> factory(
			k3d::uuid(0xb303503d, 0xc2448c83, 0xe363b7a6, 0x275afc81),
			"LuxRenderDistantLight",
			_("LuxRender Distant Light"),
			"LuxRender Light",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_from;
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_to;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

k3d::iplugin_factory& distant_light_factory()
{
	return distant_light::get_factory();
}

} // namespace luxrender

} // namespace module

