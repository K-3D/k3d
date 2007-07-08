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
#include "interactive.h"
#include "node_history.h"
#include "node_list.h"
#include "node_properties.h"
#include "panel.h"
#include "panel_frame.h"
#include "pipeline_profiler.h"
#include "render.h"
#include "timeline.h"
#include "tool_panel.h"
#include "tool_properties.h"
#include "undo_tree.h"
#include "utility.h"
#include "viewport.h"
#include "widget_manip.h"

#include <k3dsdk/application.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iplugin_factory_collection.h>
#include <k3dsdk/log.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/string_cast.h>

#include <boost/format.hpp>

#include <iostream>

namespace libk3dngui
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
		base(Document, "floating_window")
	{
	}
};

} // namespace detail

control::control(document_state& DocumentState, k3d::icommand_node& Parent, panel_focus_signal_t& PanelFocusSignal) :
	base(),
	ui_component("panel", &Parent),
	k3d::property_collection(),
	pinned(init_name("pinned") + init_label(_("Pinned Panel")) + init_description(_("Works with 'Hide Unpinned Panels' action")) + init_value(false)),
	automagic(init_name("automagic") + init_label(_("Automagic")) + init_description(_("Allows panel to change depending on active tool")) + init_value(false)),
	decorations(init_name("decorations") + init_label(_("Decorations")) + init_description(_("Hides/shows decorations")) + init_value(true)),
	m_decorations(false, 0),
	m_document_state(DocumentState),
	m_panel_focus_signal(PanelFocusSignal),
	m_panel_focus(false),
	m_parent(Parent)
{
	m_panel_focus_changed_connection = m_panel_focus_signal.connect(sigc::mem_fun(*this, &control::on_panel_focus_changed));

	// Don't set it to NORMAL else the focus color won't show
	set_shadow_type(Gtk::SHADOW_ETCHED_IN);

	image_toggle_button::control* const pinned_control =
		new image_toggle_button::control(*this, "pinned",
			image_toggle_button::proxy(pinned), load_icon("pinned", Gtk::ICON_SIZE_BUTTON),
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

void control::mount(panel::control& Panel)
{
	m_grab_focus_connection = Panel.connect_focus_signal(sigc::mem_fun(*this, &control::on_grab_focus));
	m_frame.add(dynamic_cast<Gtk::Widget&>(Panel));
}

void control::mount_node_list()
{
	unmount();
	mount(*Gtk::manage(new node_list::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(0);
	m_panel_type_connection.unblock();
}

void control::on_mount_node_list()
{
	record_command("mount_node_list");
	mount_node_list();
}

void control::mount_node_history()
{
	unmount();
	mount(*Gtk::manage(new node_history::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(1);
	m_panel_type_connection.unblock();
}

void control::on_mount_node_history()
{
	record_command("mount_node_history");
	mount_node_history();
}

void control::mount_node_properties()
{
	unmount();
	mount(*Gtk::manage(new node_properties::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(2);
	m_panel_type_connection.unblock();
}

void control::on_mount_node_properties()
{
	record_command("mount_node_properties");
	mount_node_properties();
}

void control::mount_tool_properties()
{
	unmount();
	mount(*Gtk::manage(new tool_properties::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(3);
	m_panel_type_connection.unblock();
}

void control::on_mount_tool_properties()
{
	record_command("mount_tool_properties");
	mount_tool_properties();
}

void control::mount_undo_tree()
{
	unmount();
	mount(*Gtk::manage(new undo_tree::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(4);
	m_panel_type_connection.unblock();
}

void control::on_mount_undo_tree()
{
	record_command("mount_undo_tree");
	mount_undo_tree();
}

void control::mount_timeline()
{
	unmount();
	mount(*Gtk::manage(new timeline::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(5);
	m_panel_type_connection.unblock();
}

void control::on_mount_timeline()
{
	record_command("mount_timeline");
	mount_timeline();
}

void control::mount_viewport(k3d::icamera& Camera, k3d::gl::irender_engine& RenderEngine)
{
	unmount();

	viewport::control* const control = new viewport::control(m_document_state, m_parent);
	control->set_camera(&Camera);
	control->set_gl_engine(&RenderEngine);

	mount(*Gtk::manage(control));

	m_panel_type_connection.block();
	m_panel_type.set_active(6);
	m_panel_type_connection.unblock();
}

void control::on_mount_viewport()
{
	k3d::icamera* camera = default_camera(m_document_state);
	if(!camera)
		camera = pick_camera(m_document_state);
	if(!camera)
		return;

	k3d::gl::irender_engine* engine = default_gl_render_engine(m_document_state);
	if(!engine)
		engine = pick_gl_render_engine(m_document_state);
	if(!engine)
		return;

	record_command("mount_viewport");

	mount_viewport(*camera, *engine);
}

void control::mount_toolbar()
{
	unmount();
	mount(*Gtk::manage(new tool_panel::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(7);
	m_panel_type_connection.unblock();
}

void control::on_mount_toolbar()
{
	record_command("mount_toolbar");
	mount_toolbar();
}

void control::mount_pipeline_profiler()
{
	unmount();
	mount(*Gtk::manage(new pipeline_profiler::control(m_document_state, m_parent)));

	m_panel_type_connection.block();
	m_panel_type.set_active(8);
	m_panel_type_connection.unblock();
}

void control::on_mount_pipeline_profiler()
{
	record_command("mount_pipeline_profiler");
	mount_pipeline_profiler();
}

void control::on_mount_plugin(k3d::iplugin_factory* Plugin)
{
	return_if_fail(Plugin);

	k3d::iplugin_factory::metadata_t metadata = Plugin->metadata();
	const std::string panel_name = metadata["panel_name"];

	record_command("mount_" + panel_name);

	panel::control* const panel = k3d::create_plugin<panel::control>(*Plugin);
	return_if_fail(panel);

	unmount();
	Gtk::manage(dynamic_cast<Gtk::Widget*>(panel));
	mount(*panel);
}

void control::mount_panel(const std::string Name)
{
	if("node_list" == Name)
		mount_node_list();
	else if("node_history" == Name)
		mount_node_history();
	else if("node_properties" == Name)
		mount_node_properties();
	else if("tool_properties" == Name)
		mount_tool_properties();
	else if("undo_tree" == Name)
		mount_undo_tree();
	else if("timeline" == Name)
		mount_timeline();
	else if("pipeline_profiler" == Name)
		mount_pipeline_profiler();
	else if("viewport" == Name)
	{
		const k3d::nodes_t gl_engines = k3d::find_nodes<k3d::gl::irender_engine>(m_document_state.document().nodes());
		k3d::gl::irender_engine* const glengine1 = gl_engines.size() > 0 ? dynamic_cast<k3d::gl::irender_engine*>(*(gl_engines.begin())) : 0;

		const k3d::nodes_t cameras = k3d::find_nodes<k3d::icamera>(m_document_state.document().nodes());
		k3d::icamera* const camera1 = cameras.size() > 0 ? dynamic_cast<k3d::icamera*>(*(cameras.begin())) : 0;

		if(glengine1 && camera1)
			mount_viewport(*camera1, *glengine1);
	}
	else if("toolbar" == Name)
		mount_toolbar();
	else
	{
		k3d::log() << error << "Couldn't mount panel of type : " << Name << std::endl;
	}
}

void control::unmount()
{
	unset_bg_color();

	m_grab_focus_connection.disconnect();

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

Gtk::Widget* const control::mounted_panel()
{
	return m_frame.get_child();
}

const std::string control::mounted_panel_type()
{
	if(dynamic_cast<node_list::control*>(mounted_panel()))
		return "node_list";
	if(dynamic_cast<node_history::control*>(mounted_panel()))
		return "node_history";
	if(dynamic_cast<node_properties::control*>(mounted_panel()))
		return "node_properties";
	if(dynamic_cast<tool_properties::control*>(mounted_panel()))
		return "tool_properties";
	if(dynamic_cast<undo_tree::control*>(mounted_panel()))
		return "undo_tree";
	if(dynamic_cast<timeline::control*>(mounted_panel()))
		return "timeline";
	if(dynamic_cast<viewport::control*>(mounted_panel()))
		return "viewport";
	if(dynamic_cast<tool_panel::control*>(mounted_panel()))
		return "toolbar";
	if(dynamic_cast<pipeline_profiler::control*>(mounted_panel()))
		return "pipeline_profiler";

	return "";
}

void control::save(k3d::xml::element& Document)
{
	k3d::xml::element& panel = Document.append(k3d::xml::element("panel"));

	panel.append(k3d::xml::attribute("type", mounted_panel_type()));
	panel.append(k3d::xml::attribute("pinned", pinned.value()));
	panel.append(k3d::xml::attribute("visible", is_visible()));
	panel.append(k3d::xml::attribute("automagic", automagic.value()));
	panel.append(k3d::xml::attribute("decorations", decorations.value()));
}

void control::load(k3d::xml::element& Element)
{
	return_if_fail("panel" == Element.name);

	const std::string type = attribute_text(Element, "type");
	const std::string is_pinned = attribute_text(Element, "pinned");
	const std::string is_visible = attribute_text(Element, "visible");
	const std::string is_automagic = attribute_text(Element, "automagic");
	const std::string is_decorated = attribute_text(Element, "decorations");

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

	add_choice(quiet_load_icon("node_list_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Node List"), sigc::mem_fun(*this, &control::on_mount_node_list));
	add_choice(quiet_load_icon("node_history_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Node History"), sigc::mem_fun(*this, &control::on_mount_node_history));
	add_choice(quiet_load_icon("node_properties_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Node Properties"), sigc::mem_fun(*this, &control::on_mount_node_properties));
	add_choice(quiet_load_icon("tool_properties_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Tool Properties"), sigc::mem_fun(*this, &control::on_mount_tool_properties));
	add_choice(quiet_load_icon("undo_tree_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Undo Tree"), sigc::mem_fun(*this, &control::on_mount_undo_tree));
	add_choice(quiet_load_icon("timeline_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Timeline"), sigc::mem_fun(*this, &control::on_mount_timeline));
	add_choice(quiet_load_icon("viewport_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Viewport"), sigc::mem_fun(*this, &control::on_mount_viewport));
	add_choice(quiet_load_icon("toolbar_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Toolbar"), sigc::mem_fun(*this, &control::on_mount_toolbar));
	add_choice(quiet_load_icon("pipeline_profiler_panel", Gtk::ICON_SIZE_SMALL_TOOLBAR), _("Pipeline Profiler"), sigc::mem_fun(*this, &control::on_mount_pipeline_profiler));

	const k3d::iplugin_factory_collection::factories_t& factories = k3d::application().plugins();
	for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		k3d::iplugin_factory::metadata_t metadata = (**factory).metadata();

		if(metadata["NextGenerationUI"] != "true")
			continue;

		if(metadata["component_type"] != "panel")
			continue;

		const std::string panel_name = metadata["panel_name"];
		if(panel_name.empty())
		{
			k3d::log() << error << "Panel plugin without panel_name metadata will be ignored" << std::endl;
			continue;
		}

		const std::string panel_label = metadata["panel_label"];
		if(panel_label.empty())
		{
			k3d::log() << error << "Panel plugin [" << panel_name << "] without panel_label metadata will be ignored" << std::endl;
			continue;
		}

		add_choice(quiet_load_icon(panel_name, Gtk::ICON_SIZE_SMALL_TOOLBAR), panel_label, sigc::bind(sigc::mem_fun(*this, &control::on_mount_plugin), *factory));
	}
}

void control::add_choice(const Glib::RefPtr<Gdk::Pixbuf> Icon, const Glib::ustring& Label, sigc::slot<void> Slot)
{
	Gtk::TreeRow row = *m_model->append();

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
	if(decorations.value())
		m_decorations.show();
	else
		m_decorations.hide();
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "float")
	{
		float_panel();
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_node_list")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("0")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_node_history")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("1")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_node_properties")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("2")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_tool_properties")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("3")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_undo_tree")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("4")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_timeline")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("5")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_viewport")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("6")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_toolbar")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("7")));
		return RESULT_CONTINUE;
	}
	else if(Command == "mount_pipeline_profiler")
	{
		interactive::select_row(m_panel_type, m_model->get_iter(Gtk::TreePath("8")));
		return RESULT_CONTINUE;
	}

	return ui_component::execute_command(Command, Arguments);
}

} // namespace panel_frame

} // namespace libk3dngui

