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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/node.h>
#include <k3dsdk/vector3.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// sun_light

class sun_light :
	public k3d::node,
	public luxrender::light
{
	typedef k3d::node base;

public:
	sun_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_gain(init_owner(*this) + init_name("gain") + init_label(_("Gain")) + init_description(_("Scales light intensity.")) + init_value(0.005)),
		m_samples(init_owner(*this) + init_name("samples") + init_label(_("Samples")) + init_description(_("Shadow samples.")) + init_value(1)),
		m_direction(init_owner(*this) + init_name("direction") + init_label(_("Direction")) + init_description(_("Vector pointing to the sun.")) + init_value(k3d::vector3(0, 0, 1))),
		m_turbidity(init_owner(*this) + init_name("turbidity") + init_label(_("Turbidity")) + init_description(_("Controls sun_light clarity / haziness.")) + init_value(2.0)),
		m_size(init_owner(*this) + init_name("size") + init_label(_("Size")) + init_description(_("Relative size.")) + init_value(1.0))
	{
	}

	void setup(std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "AttributeBegin\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "LightSource \"sun\"\n" << k3d::push_indent;
		Stream << k3d::standard_indent << "\"float gain\" [" << m_gain.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"integer nsamples\" [" << m_samples.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"vector sundir\" [" << convert(m_direction.pipeline_value()) << "]\n";
		Stream << k3d::standard_indent << "\"float turbidity\" [" << m_turbidity.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"float relsize\" [" << m_size.pipeline_value() << "]\n";
		Stream << k3d::pop_indent << k3d::pop_indent << k3d::standard_indent << "AttributeEnd\n";
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sun_light> factory(
			k3d::uuid(0x31a4dc66, 0xba47822e, 0xfa07b286, 0xc4534449),
			"LuxRenderSunLight",
			_("LuxRender SunLight"),
			"LuxRender Light",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gain;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_samples;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_direction;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_turbidity;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_size;
};

k3d::iplugin_factory& sun_light_factory()
{
	return sun_light::get_factory();
}

} // namespace luxrender

} // namespace module

