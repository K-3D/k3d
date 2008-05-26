// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
#include <k3dsdk/ngui/custom_property_page.h>

#include <gtkmm/label.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace ngui
{

namespace morph_points
{

/////////////////////////////////////////////////////////////////////////////
// page

/// Provides a custom property-page for the MorphPoints modifier
class page :
	public k3d::ngui::custom_property_page::control,
	public k3d::iunknown
{
public:
	page() :
		m_label("MorphPoints custom property page")
	{
	}

	Gtk::Widget& get_widget(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent, k3d::inode& Node)
	{
		m_label.show();
		return m_label;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<page> factory(
			k3d::uuid(0xb9897383, 0xba485a29, 0x9166ff98, 0xb4cb6be6),
			"NGUIMorphPointsPage",
			_("Provides a custom property page for the MorphPoints modifier."),
			"NGUI Page",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "property-page")("ngui:plugin-type", "MorphPoints"));

		return factory;
	}

private:
	Gtk::Label m_label;
};

} // namespace morph_points

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::morph_points::page::get_factory());
K3D_MODULE_END

