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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/dependencies.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/custom_property_control.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/serialization_xml.h>
#include <k3dsdk/type_registry.h>

#include <gtkmm/box.h>
#include <gtkmm/menu.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

#include <gtk/gtkmain.h>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a custom control for k3d::selection::set properties.
class control :
	public k3d::ngui::custom_property::control,
	public k3d::iunknown,
	public Gtk::VBox
{
public:
	control() :
		Gtk::VBox()
	{
	}

	~control()
	{
	}

	void initialize(document_state& DocumentState, k3d::iproperty& Property)
	{
		Gtk::Button* const menu_button = new Gtk::Button(_("Selection Options"));
		menu_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &control::on_choose), &Property));

		pack_start(*Gtk::manage(menu_button), Gtk::PACK_EXPAND_WIDGET);
	}

	void on_choose(k3d::iproperty* const Property)
	{
		const k3d::selection::set selection = k3d::property::pipeline_value<k3d::selection::set>(*Property);
			
		m_menu.reset(new Gtk::Menu());

		Gtk::MenuItem* const save_selection = new Gtk::MenuItem(_("Save selection ..."));
		save_selection->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &control::on_save_selection), selection));
		m_menu->items().push_back(*manage(save_selection));

		m_menu->show_all();
		m_menu->popup(1, gtk_get_current_event_time());
	}

	void on_save_selection(const k3d::selection::set Selection)
	{
		k3d::filesystem::path output_path;
		{
			file_chooser_dialog dialog(_("Save selection file:"), "k3d", Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(output_path))
				return;
		}

		const k3d::filesystem::path root_path = output_path.branch_path();
		k3d::dependencies dependencies;
		k3d::persistent_lookup lookup;
		k3d::ipersistent::save_context context(root_path, dependencies, lookup);

		k3d::xml::element xml("k3dml");
		k3d::xml::element& xml_selection = xml.append(k3d::xml::element("selection"));
		k3d::xml::save(Selection, xml_selection, context);

		k3d::filesystem::ofstream stream(output_path);
		stream << k3d::xml::declaration() << xml << std::endl;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<control> factory(
			k3d::uuid(0x7025b049, 0xdb4c25bc, 0xb37ca188, 0x0044ab19),
			"NGUISelectionControl",
			_("Provides a standard control for selection properties."),
			"NGUI Control",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "property-control")("ngui:property-type", "k3d::selection::set"));

		return factory;
	}

	boost::scoped_ptr<Gtk::Menu> m_menu;
};

} // namespace selection

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::selection::control::get_factory());
K3D_MODULE_END

