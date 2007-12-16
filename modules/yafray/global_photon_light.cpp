// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Romain Behar <romainbehar@yahoo.com>
*/

#include "lights.h"

namespace module
{

namespace yafray
{

/////////////////////////////////////////////////////////////////////////////
// global_photon_light

class global_photon_light :
	public photons<light<k3d::persistent<k3d::node> > >
{
	typedef photons<light<k3d::persistent<k3d::node> > > base;

public:
	global_photon_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(2.0))
	{
	}

	void setup_light(const k3d::string_t& Name, std::ostream& Stream)
	{
		if(!m_emit.pipeline_value())
			return;

		Stream << "<!-- K-3D plugin: " << factory().name() << " name: " << name() << " -->\n";
		Stream << "<light type=\"globalphotonlight\" name=\"" << Name << "\" photons=\"" << m_photons.pipeline_value() << "\" radius=\"" << m_radius.pipeline_value() << "\" depth=\"" << m_depth.pipeline_value() << "\" search=\"" << m_search.pipeline_value() << "\">\n";
		Stream << "</light>\n";
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<global_photon_light> factory(
			k3d::uuid(0x8c651262, 0x2e2d4fcc, 0xbb20f617, 0xf0c56f41),
			"YafrayGlobalPhotonLight",
			_("Yafray Global Photon Light"),
			"Yafray Lights",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_radius;
};

k3d::iplugin_factory& global_photon_light_factory()
{
	return global_photon_light::get_factory();
}

} // namespace yafray

} // namespace module

