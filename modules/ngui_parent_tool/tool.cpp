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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <gdkmm/cursor.h>
#include <gtkmm/widget.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iparentable.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/basic_viewport_input_model.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/interactive.h>
#include <k3dsdk/ngui/modifiers.h>
#include <k3dsdk/ngui/navigation_input_model.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/ngui/tool.h>
#include <k3dsdk/ngui/transform.h>
#include <k3dsdk/ngui/viewport.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/transform.h>

#include <boost/assign/list_of.hpp>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace parent
{

/////////////////////////////////////////////////////////////////////////////
// implementation

struct implementation
{
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_set_parent(false),
		m_navigation_model(DocumentState)
	{
		m_input_model.connect_lbutton_click(sigc::mem_fun(*this, &implementation::on_lbutton_click));
		m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &implementation::on_rbutton_click));
		
		m_input_model.connect_mbutton_click(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_click));
		m_input_model.connect_mbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_start_drag));
		m_input_model.connect_mbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_drag));
		m_input_model.connect_mbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_end_drag));
		m_input_model.connect_rbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_start_drag));
		m_input_model.connect_rbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_drag));
		m_input_model.connect_rbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_end_drag));
		m_input_model.connect_scroll(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_scroll));
	}

	void on_lbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		if(m_set_parent)
			on_set_parent(Viewport, Event);
		else
			on_pick(Viewport, Event);
	}

	void on_rbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		m_set_parent = false;

		m_document_state.set_active_tool(m_document_state.selection_tool());
	}

	void set_parent(viewport::control& Viewport, k3d::inode& Parent)
	{
		m_set_parent = false;
		m_document_state.clear_cursor_signal().emit();

		k3d::itransform_source* const transform_source = dynamic_cast<k3d::itransform_source*>(&Parent);
		return_if_fail(transform_source);

		const k3d::matrix4 parent_compensation = k3d::inverse(k3d::node_to_world_matrix(Parent));

		k3d::ipipeline::dependencies_t dependencies;

		k3d::record_state_change_set changeset(m_document_state.document(), _("Set Parent"), K3D_CHANGE_SET_CONTEXT);

		// Note - we enumerate over a *copy* of the set of document nodes, since we're adding nodes as we go ...
		const k3d::nodes_t nodes = m_document_state.document().nodes().collection();
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			// Ensure we don't try to parent an object to itself ...
			if(&Parent == *node)
				continue;

			if(!selection::state(m_document_state.document()).is_selected(**node))
				continue;

			const transform_history_t history = parent_to_node_history(**node);
			if(!history.empty())
			{
				if(k3d::itransform_sink* const transform_sink = dynamic_cast<k3d::itransform_sink*>(history.front()))
				{
					const transform_modifier modifier = create_transform_modifier(m_document_state.document(), k3d::classes::FrozenTransformation(), "Parent Compensation");
					if(modifier)
					{
						k3d::property::set_internal_value(*modifier.node, "matrix", parent_compensation);
						dependencies.insert(std::make_pair(&transform_sink->transform_sink_input(), &modifier.source->transform_source_output()));
						dependencies.insert(std::make_pair(&modifier.sink->transform_sink_input(), &transform_source->transform_source_output()));
					}
				}
			}

			if(k3d::iparentable* const parentable = dynamic_cast<k3d::iparentable*>(*node))
			{
				if(k3d::iwritable_property* const writable_parent = dynamic_cast<k3d::iwritable_property*>(&parentable->parent()))
					writable_parent->property_set_value(&Parent);
			}
		}

		selection::state(m_document_state.document()).deselect_all();
		selection::state(m_document_state.document()).select(Parent);
		m_document_state.document().pipeline().set_dependencies(dependencies);
	}
	
	void on_set_parent(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::selection::record selection = Viewport.pick_node(k3d::point2(Event.x, Event.y));
		if(selection.empty())
			return;

		k3d::inode* const node = k3d::selection::get_node(selection);
		if(!node)
			return;

		set_parent(Viewport, *node);

	}

	void on_pick(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::selection::record selection = Viewport.pick_node(k3d::point2(Event.x, Event.y));
		if(selection.empty())
			return;

		k3d::inode* const node = k3d::selection::get_node(selection);
		return_if_fail(node);

		if(selection::state(m_document_state.document()).is_selected(*node))
		{
			m_set_parent = true;
			m_document_state.set_cursor_signal().emit(load_icon("parent_cursor", Gtk::ICON_SIZE_BUTTON));
		}
		else
		{
			selection::state(m_document_state.document()).select(selection);
		}
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Set to true iff the next pick should choose a new parent for all selected objects
	bool m_set_parent;
	/// Provides interactive navigation behavior
	navigation_input_model m_navigation_model;
	/// Dispatches incoming user input events
	basic_viewport_input_model m_input_model;
};

/////////////////////////////////////////////////////////////////////////////
// tool

/// User-interface tool that provides interactive node parenting
class tool :
	public k3d::ngui::tool,
	public k3d::iunknown
{
public:
	tool() :
		m_implementation(0)
	{
	}

	~tool()
	{
		delete m_implementation;
	}

	const k3d::string_t tool_type()
	{
		return get_factory().name();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<tool> factory(
			k3d::uuid(0x07371997, 0x1040b5a8, 0x2cf245be, 0xf1bbb199),
			"NGUIParentTool",
			_("Provides interactive controls for reparenting nodes."),
			"NGUI Tool",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "tool"));

		return factory;
	}

private:
	virtual void on_initialize(document_state& DocumentState)
	{
		m_implementation = new parent::implementation(DocumentState);
	}

	virtual void on_deactivate()
	{
		m_implementation->m_document_state.clear_cursor_signal().emit();
	}

	virtual viewport_input_model& get_input_model()
	{
		return m_implementation->m_input_model;
	}

	parent::implementation* m_implementation;
};

} // namespace parent

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::parent::tool::get_factory());
K3D_MODULE_END

