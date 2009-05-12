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
#include <k3dsdk/command_tree.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/auto_property_page.h>
#include <k3dsdk/ngui/bitmap_preview.h>
#include <k3dsdk/ngui/bounding_box.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/tool.h>
#include <k3dsdk/ngui/widget_manip.h>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

#include <boost/assign/list_of.hpp>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace tool_properties
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// implementation

class implementation :
	public asynchronous_update
{
public:
	implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
		m_document_state(DocumentState),
		m_tool(0),
		m_object_properties(DocumentState, Parent)
	{
		m_label.set_alignment(Gtk::ALIGN_LEFT);
		m_label.set_padding(5, 5);

		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_object_properties.get_widget());

		m_vbox.pack_start(m_label, Gtk::PACK_SHRINK);
		m_vbox.pack_start(m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

		m_document_state.connect_active_tool_changed_signal(sigc::mem_fun(*this, &implementation::on_active_tool_changed));

		m_tool = &m_document_state.active_tool();
		schedule_update();
	}

	void on_active_tool_changed()
	{
		m_tool = &m_document_state.active_tool();

		m_vbox.hide();
		schedule_update();
	}

	void on_update()
	{
		if(m_tool)
			m_label.set_text(k3d::string_cast(boost::format(_("%1% Properties")) % k3d::command_tree().name(*m_tool)));
		else
			m_label.set_text("");

		m_object_properties.set_properties(m_tool ? m_tool->properties() : 0);

		m_vbox.show();
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores the current tool to be displayed (if any)
	k3d::ngui::tool* m_tool;
	/// Contains the other widgets
	Gtk::VBox m_vbox;
	/// Displays the current tool name
	Gtk::Label m_label;
	/// Contains the set of tool properties
	Gtk::ScrolledWindow m_scrolled_window;
	/// Provides a collection of property controls
	k3d::ngui::auto_property_page::control m_object_properties;

	sigc::signal<void, const std::string&, const std::string&> m_command_signal;

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;
};

} // namespace detail


/////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public k3d::ngui::panel::control,
	public k3d::ngui::ui_component,
	public Gtk::VBox
{
	typedef Gtk::VBox base;

public:
	panel() :
		m_implementation(0)
	{
	}

	~panel()
	{
		delete m_implementation;
	}

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
	{
		k3d::command_tree().add(*this, "tool_properties", &Parent);

		m_implementation = new detail::implementation(DocumentState, *this);

		m_implementation->m_scrolled_window.signal_button_press_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);
		
		pack_start(m_implementation->m_vbox, Gtk::PACK_EXPAND_WIDGET);
		show_all();
	}

	const k3d::string_t panel_type()
	{
		return get_factory().name();
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
		return m_implementation->m_panel_grab_signal.connect(Slot);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
			k3d::uuid(0x2d5c6b09, 0x4b860158, 0x901dc7b6, 0xca8cb11e),
			"NGUIToolPropertiesPanel",
			_("Displays tool properties"),
			"NGUI Panel",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-label", "Tool Properties"));

		return factory;
	}

private:
	detail::implementation* m_implementation;
};

} // namespace tool_properties

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::tool_properties::panel::get_factory());
K3D_MODULE_END

