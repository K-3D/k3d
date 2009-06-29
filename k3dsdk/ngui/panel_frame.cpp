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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/stock.h>

#include <k3d-i18n-config.h>

#include "document_state.h"
#include "document_window.h"
#include "icons.h"
#include "image_toggle_button.h"
#include "panel.h"
#include "panel_frame.h"
#include "render.h"
#include "utility.h"
#include "viewport.h"
#include "widget_manip.h"

#include <k3dsdk/icamera.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iplugin_factory_collection.h>
#include <k3dsdk/log.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/string_cast.h>

#include <boost/format.hpp>

#include <iostream>

namespace k3d
{

namespace ngui
{

namespace panel_frame
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// window

class floating_window :
	public document_window
{
	typedef document_window base;

public:
	floating_window(document_state& Document) :
		base(Document)
	{
	}
};

} // namespace detail

control::control(document_state& DocumentState, panel_focus_signal_t& PanelFocusSignal) :
	base(),
	k3d::property_collection(),
	pinned(init_name("pinned") + init_label(_("Pinned Panel")) + init_description(_("Works with 'Hide Unpinned Panels' action")) + init_value(false)),
	automagic(init_name("automagic") + init_label(_("Automagic")) + init_description(_("Allows panel to change depending on active tool")) + init_value(false)),
	decorations(init_name("decorations") + init_label(_("Decorations")) + init_description(_("Hides/shows decorations")) + init_value(true)),
	m_decorations(false, 0),
	m_document_state(DocumentState),
	m_panel_focus_signal(PanelFocusSignal),
	m_panel_focus(false)
{
	m_panel_focus_changed_connection = m_panel_focus_signal.connect(sigc::mem_fun(*this, &control::on_panel_focus_changed));

	// Don't set it to NORMAL else the focus color won't show
	set_shadow_type(Gtk::SHADOW_ETCHED_IN);

	image_toggle_button::control* const pinned_control =
		new image_toggle_button::control(
			image_toggle_button::model(pinned), 0, load_icon("pinned", Gtk::ICON_SIZE_BUTTON),
			load_icon("unpinned", Gtk::ICON_SIZE_BUTTON)) <<
		set_tooltip(_("Pin Panel"));

	pinned_control->signal_clicked().connect(sigc::mem_fun(*this, &control::grab_panel_focus));

	decorations.changed_signal().connect(sigc::mem_fun(*this, &control::on_decorations_changed));

	Gtk::EventBox* const title_event_box = new Gtk::EventBox();
	title_event_box->add(m_title);
	title_event_box->add_events(Gdk::BUTTON_PRESS_MASK);
	title_event_box->signal_button_press_event().connect(sigc::bind_return(sigc::hide(sigc::mem_fun(*this, &control::on_grab_focus)), false));

	m_model = Gtk::ListStore::create(m_columns);
	m_panel_type.set_model(m_model);
	m_panel_type.pack_start(m_columns.icon, false);
	m_panel_type.pack_start(m_columns.label);

	m_panel_type_connection = m_panel_type.signal_changed().connect(sigc::mem_fun(*this, &control::on_panel_type_changed));

	m_decorations.pack_start(*Gtk::manage(pinned_control), Gtk::PACK_SHRINK);
	m_decorations.pack_start(m_panel_type, Gtk::PACK_SHRINK);
	m_decorations.pack_start(*Gtk::manage(title_event_box), Gtk::PACK_EXPAND_WIDGET);

	m_frame.set_shadow_type(Gtk::SHADOW_NONE);

	Gtk::VBox* const vbox1 = new Gtk::VBox(false);
	vbox1->pack_start(m_decorations, Gtk::PACK_SHRINK);
	vbox1->pack_start(m_frame, Gtk::PACK_EXPAND_WIDGET);

	base::add(*manage(vbox1));

	set_choices();
	show_all();
}

control::~control()
{
	// Prevent a crash caused by self-updating after we're gone ...
	m_panel_focus_changed_connection.disconnect();

	// If we have the panel focus, give it up ...
	if(m_panel_focus)
	{
		m_panel_focus = false;
		m_panel_focus_signal.emit(0);
	}
}

void control::add(Widget& widget)
{
	base::add(widget);
}

void control::remove()
{
	base::remove();
}

void control::grab_panel_focus()
{
	m_panel_focus_signal.emit(this);
}

void control::mount_panel(panel::control& Panel, const k3d::string_t& Type)
{
	unmount();

	m_grab_focus_connection = Panel.connect_focus_signal(sigc::mem_fun(*this, &control::on_grab_focus));
	m_frame.add(dynamic_cast<Gtk::Widget&>(Panel));

	m_panel_type_connection.block();
	m_panel_type.set_active(index(Type));
	m_panel_type_connection.unblock();
}

void control::mount_panel(const k3d::string_t& Type, bool RequestCamera)
{
	if("NGUIViewportPanel" == Type)
	{
		const k3d::nodes_t gl_engines = k3d::find_nodes<k3d::gl::irender_viewport>(m_document_state.document().nodes());
		k3d::gl::irender_viewport* const glengine1 = gl_engines.size() > 0 ? dynamic_cast<k3d::gl::irender_viewport*>(*(gl_engines.begin())) : 0;

		k3d::icamera* camera = 0;
		if (RequestCamera)
		{
			camera = pick_camera(m_document_state);
		}
		else
		{
			const k3d::nodes_t cameras = k3d::find_nodes<k3d::icamera>(m_document_state.document().nodes());
			camera = cameras.size() > 0 ? dynamic_cast<k3d::icamera*>(*(cameras.begin())) : 0;
		}

		if(glengine1 && camera)
		{
			viewport::control* const control = new viewport::control(m_document_state);
			control->set_camera(camera);
			control->set_gl_engine(glengine1);

			mount_panel(*Gtk::manage(control), Type);
			m_document_state.set_focus_viewport(control);
		}
		return;
	}

	if(m_type_plugin_map.count(Type))
	{
		k3d::iplugin_factory* const plugin = m_type_plugin_map[Type];
		return_if_fail(plugin);

		panel::control* const panel = k3d::plugin::create<panel::control>(*plugin);
		return_if_fail(panel);

		panel->initialize(m_document_state);

		Gtk::manage(dynamic_cast<Gtk::Widget*>(panel));
		mount_panel(*panel, Type);
		return;
	}

	k3d::log() << error << "Couldn't mount panel of type : " << Type << std::endl;
}

void control::on_mount_panel(const k3d::string_t& Type)
{
	mount_panel(Type);
}

void control::unmount()
{
	unset_bg_color();

	m_grab_focus_connection.disconnect();
	
	viewport::control* control = dynamic_cast<viewport::control*>(m_frame.get_child());
	if (control == m_document_state.get_focus_viewport())
		m_document_state.set_focus_viewport(0);

	delete m_frame.get_child();
}

void control::float_panel()
{
	if(!m_frame.get_child())
		return;

	m_grab_focus_connection.disconnect();

	detail::floating_window* const window = new detail::floating_window(m_document_state);
	m_frame.get_child()->reparent(*window);
	window->show();
}

void control::set_bg_color(const Gdk::Color& color)
{
	modify_bg(Gtk::STATE_NORMAL, color);
}

void control::unset_bg_color()
{
	unset_bg(Gtk::STATE_NORMAL);
}

panel::control* const control::mounted_panel()
{
	return dynamic_cast<panel::control*>(m_frame.get_child());
}

void control::save(k3d::xml::element& Document)
{
	k3d::xml::element& panel = Document.append(k3d::xml::element("panel"));

	if(mounted_panel())
		panel.append(k3d::xml::attribute("type", mounted_panel()->panel_type()));
	panel.append(k3d::xml::attribute("pinned", pinned.internal_value()));
	panel.append(k3d::xml::attribute("visible", is_visible()));
	panel.append(k3d::xml::attribute("automagic", automagic.internal_value()));
	panel.append(k3d::xml::attribute("decorations", decorations.internal_value()));
}

void control::load(k3d::xml::element& Element)
{
	return_if_fail("panel" == Element.name);

	const k3d::string_t type = attribute_text(Element, "type");
	const k3d::string_t is_pinned = attribute_text(Element, "pinned");
	const k3d::string_t is_visible = attribute_text(Element, "visible");
	const k3d::string_t is_automagic = attribute_text(Element, "automagic");
	const k3d::string_t is_decorated = attribute_text(Element, "decorations");

	mount_panel(type);

	pinned.set_value("true" == is_pinned);
	automagic.set_value("true" == is_automagic);
	decorations.set_value("true" == is_decorated);

	if("true" == is_visible)
		show();
	else
		hide();
}

void control::on_panel_focus_changed(control* Container)
{
	m_panel_focus = Container == this;
	m_frame.set_shadow_type(m_panel_focus ? Gtk::SHADOW_IN : Gtk::SHADOW_NONE);
}

void control::on_grab_focus()
{
	grab_panel_focus();
}

void control::set_choices()
{
	m_model->clear();

	add_choice("NGUIViewportPanel", quiet_load_icon("viewport_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Viewport"), sigc::bind(sigc::mem_fun(*this, &control::on_mount_panel), "NGUIViewportPanel"));

	const k3d::plugin::factory::collection_t factories = k3d::plugin::factory::lookup();
	for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		k3d::iplugin_factory::metadata_t metadata = (**factory).metadata();

		if(metadata["ngui:component-type"] != "panel")
			continue;

		const k3d::string_t panel_type = (**factory).name();

		const k3d::string_t panel_label = metadata["ngui:panel-label"];
		if(panel_label.empty())
		{
			k3d::log() << error << "Panel plugin [" << panel_type << "] without ngui:panel-label metadata will be ignored" << std::endl;
			continue;
		}

		m_type_plugin_map[panel_type] = *factory;

		add_choice(panel_type, quiet_load_icon(panel_type + "_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), panel_label, sigc::bind(sigc::mem_fun(*this, &control::on_mount_panel), panel_type));
	}

	m_model->set_sort_column(m_columns.label, Gtk::SORT_ASCENDING);
}

void control::add_choice(const k3d::string_t& PanelType, const Glib::RefPtr<Gdk::Pixbuf> Icon, const Glib::ustring& Label, sigc::slot<void> Slot)
{
	Gtk::TreeRow row = *m_model->append();

	row[m_columns.type] = PanelType;
	row[m_columns.label] = Label;
	row[m_columns.slot] = Slot;
	row[m_columns.icon] = Icon;
}

void control::on_panel_type_changed()
{
	return_if_fail(m_panel_type.get_active() != m_model->children().end());

	Gtk::TreeRow row = *m_panel_type.get_active();
	sigc::slot<void> slot = row.get_value(m_columns.slot);
	slot();
}

void control::on_decorations_changed(k3d::iunknown*)
{
	if(decorations.internal_value())
		m_decorations.show();
	else
		m_decorations.hide();
}

const unsigned long control::index(const k3d::string_t& PanelType)
{
	const Gtk::TreeModel::Children children = m_model->children();
	for(unsigned long i = 0; i != children.size(); ++i)
	{
		if(children[i][m_columns.type] == PanelType)
			return i;
	}

	k3d::log() << error << k3d_file_reference << ": error looking-up panel [" << PanelType << "]" << std::endl;
	return 0;
}

} // namespace panel_frame

} // namespace ngui

} // namespace k3d

