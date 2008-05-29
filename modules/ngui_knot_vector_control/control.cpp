// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/custom_property_control.h>

#include <gtkmm/label.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace ngui
{

namespace knot_vector
{

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a custom property-control for NURBS knot-vectors
class control :
	public k3d::ngui::custom_property::control,
	public k3d::iunknown,
	public Gtk::Label
{
public:
	control() :
		Gtk::Label("Custom Knot Vector Control")
	{
	}

	~control()
	{
	}

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent, k3d::iproperty& Property)
	{
		show();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<control> factory(
			k3d::uuid(0xade18285, 0x9a4333fc, 0x7449f382, 0x39628d15),
			"NGUIKnotVectorControl",
			_("Provides a custom property control for NURBS knot vectors."),
			"NGUI Control",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "property-control")("ngui:property-type", "k3d:nurbs-knot-vector"));

		return factory;
	}
};

} // namespace knot_vector

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::knot_vector::control::get_factory());
K3D_MODULE_END

