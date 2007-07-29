// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "basic_viewport_input_model.h"
#include "command_arguments.h"
#include "document_state.h"
#include "icons.h"
#include "interactive.h"
#include "keyboard.h"
#include "navigation_input_model.h"
#include "render.h"
#include "render_region_tool.h"
#include "rubber_band.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/color.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/high_res_timer.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/icrop_window.h>
#include <k3dsdk/ipreview_render_engine.h>
#include <k3dsdk/property.h>
#include <k3dsdk/state_change_set.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// render_region_tool::implementation

struct render_region_tool::implementation
{
public:
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_rubber_band(k3d::color(0, 0, 0.8)),
		m_navigation_model(DocumentState)
	{
		m_input_model.connect_lbutton_double_click(sigc::mem_fun(*this, &implementation::on_lbutton_double_click));
		m_input_model.connect_lbutton_start_drag(sigc::mem_fun(*this, &implementation::on_lbutton_start_drag));
		m_input_model.connect_lbutton_drag(sigc::mem_fun(*this, &implementation::on_lbutton_drag));
		m_input_model.connect_lbutton_end_drag(sigc::mem_fun(*this, &implementation::on_lbutton_end_drag));
		m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &implementation::on_rbutton_click));

		m_input_model.connect_mbutton_click(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_click));
		m_input_model.connect_mbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_start_drag));
		m_input_model.connect_mbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_drag));
		m_input_model.connect_mbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_end_drag));
		m_input_model.connect_rbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_start_drag));
		m_input_model.connect_rbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_drag));
		m_input_model.connect_rbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_end_drag));
		m_input_model.connect_scroll(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_scroll));	}

	void on_lbutton_double_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		// If the user double-clicks outside the current crop window, reset it to render the entire frame ...
		k3d::icrop_window* const crop_window = dynamic_cast<k3d::icrop_window*>(Viewport.camera());
		if(crop_window)
		{
			const k3d::rectangle window(
				boost::any_cast<double>(k3d::property::pipeline_value(crop_window->crop_left())),
				boost::any_cast<double>(k3d::property::pipeline_value(crop_window->crop_right())),
				boost::any_cast<double>(k3d::property::pipeline_value(crop_window->crop_top())),
				boost::any_cast<double>(k3d::property::pipeline_value(crop_window->crop_bottom())));

			if(!window.contains(widget_to_ndc(Viewport, k3d::point2(Event.x, Event.y))))
			{
				k3d::record_state_change_set change_set(m_document_state.document(), _("Reset Camera Crop Window"), K3D_CHANGE_SET_CONTEXT);
				k3d::property::set_internal_value(crop_window->crop_left(), 0.0);
				k3d::property::set_internal_value(crop_window->crop_right(), 1.0);
				k3d::property::set_internal_value(crop_window->crop_top(), 0.0);
				k3d::property::set_internal_value(crop_window->crop_bottom(), 1.0);

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("reset_region", arguments);

				return;
			}
		}

		// Otherwise, render a preview ...
		k3d::icamera* camera = Viewport.camera();
		if(!camera)
			camera = pick_camera(m_document_state);
		if(!camera)
			return;

		k3d::icamera_preview_render_engine* render_engine = Viewport.camera_preview_engine();
		if(!render_engine)
			render_engine = pick_camera_preview_render_engine(m_document_state);
		if(!render_engine)
			return;

		Viewport.set_camera(camera);
		Viewport.set_camera_preview_engine(render_engine);

		render_camera_preview(*camera, *render_engine);
		
		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		m_command_signal.emit("render_preview", arguments);
	}

	void on_lbutton_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		m_rubber_band.box = k3d::rectangle(Event.x, Event.x, Event.y, Event.y);
		m_rubber_band.draw(Viewport);

		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append_viewport_coordinates("box", Viewport, m_rubber_band.box);
		m_command_signal.emit("start_region", arguments);

		m_timer.restart();
	}

	void on_lbutton_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		m_rubber_band.draw(Viewport);
		m_rubber_band.box.right = Event.x;
		m_rubber_band.box.bottom = Event.y;
		m_rubber_band.draw(Viewport);

		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append_viewport_coordinates("box", Viewport, m_rubber_band.box);
		arguments.append("timestamp", m_timer.elapsed());
		m_command_signal.emit("region_motion", arguments);
	}

	void on_lbutton_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
	{
		m_rubber_band.draw(Viewport);

		k3d::icrop_window* const crop_window = dynamic_cast<k3d::icrop_window*>(Viewport.camera());
		return_if_fail(crop_window);

		const k3d::rectangle box = k3d::normalize(m_rubber_band.box);
		const k3d::point2 top_left = widget_to_ndc(Viewport, k3d::point2(box.left, box.top));
		const k3d::point2 bottom_right = widget_to_ndc(Viewport, k3d::point2(box.right, box.bottom));

		k3d::record_state_change_set change_set(m_document_state.document(), _("Set Camera Crop Window"), K3D_CHANGE_SET_CONTEXT);

		const double left = top_left[0];
		const double right = bottom_right[0];
		const double top = top_left[1];
		const double bottom = bottom_right[1];

		k3d::property::set_internal_value(crop_window->crop_left(), left);
		k3d::property::set_internal_value(crop_window->crop_right(), right);
		k3d::property::set_internal_value(crop_window->crop_top(), top);
		k3d::property::set_internal_value(crop_window->crop_bottom(), bottom);

		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("left", left);
		arguments.append("right", right);
		arguments.append("top", top);
		arguments.append("bottom", bottom);
		m_command_signal.emit("finish_region", arguments);
	}

	void on_rbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		m_document_state.set_active_tool(m_document_state.selection_tool());
	}

	bool execute_command(const std::string& Command, const std::string& Arguments)
	{
		try
		{
			if(Command == "start_region")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 viewport_coordinates = arguments.get_viewport_point2("mouse");
				const k3d::rectangle box = arguments.get_viewport_rectangle("box");

				interactive::move_pointer(viewport, viewport_coordinates);
				
				m_rubber_band.box = box;
				m_rubber_band.draw(viewport);
			
				m_timer.restart();
				
				return true;
			}
			else if(Command == "region_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 viewport_coordinates = arguments.get_viewport_point2("mouse");
				const k3d::rectangle box = arguments.get_viewport_rectangle("box");
				const double timestamp = arguments.get_double("timestamp");

				interactive::warp_pointer(viewport, viewport_coordinates, timestamp, m_timer);

				m_rubber_band.draw(viewport);
				m_rubber_band.box = box;
				m_rubber_band.draw(viewport);
				
				return true;
			}
			else if(Command == "finish_region")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 viewport_coordinates = arguments.get_viewport_point2("mouse");
				const double left = arguments.get_double("left");
				const double right = arguments.get_double("right");
				const double top = arguments.get_double("top");
				const double bottom = arguments.get_double("bottom");

				interactive::warp_pointer(viewport, viewport_coordinates);

				m_rubber_band.draw(viewport);

				k3d::icrop_window* const crop_window = dynamic_cast<k3d::icrop_window*>(viewport.camera());
				return_val_if_fail(crop_window, false);
		
				k3d::record_state_change_set change_set(m_document_state.document(), _("Set Camera Crop Window"), K3D_CHANGE_SET_CONTEXT);
		
				k3d::property::set_internal_value(crop_window->crop_left(), left);
				k3d::property::set_internal_value(crop_window->crop_right(), right);
				k3d::property::set_internal_value(crop_window->crop_top(), top);
				k3d::property::set_internal_value(crop_window->crop_bottom(), bottom);
				
				return true;
			}
			else if(Command == "render_preview")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 viewport_coordinates = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, viewport_coordinates);
				
				k3d::icamera* camera = viewport.camera();
				if(!camera)
					camera = pick_camera(m_document_state);
				if(!camera)
					return false;

				k3d::icamera_preview_render_engine* render_engine = viewport.camera_preview_engine();
				if(!render_engine)
					render_engine = pick_camera_preview_render_engine(m_document_state);
				if(!render_engine)
					return false;

				
				viewport.set_camera(camera);
				viewport.set_camera_preview_engine(render_engine);

				render_camera_preview(*camera, *render_engine);
				return true;
			}
			else if(Command == "reset_region")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 viewport_coordinates = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, viewport_coordinates);
				
				k3d::icrop_window* const crop_window = dynamic_cast<k3d::icrop_window*>(viewport.camera());
				return_val_if_fail(crop_window, false);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Reset Camera Crop Window"), K3D_CHANGE_SET_CONTEXT);
				k3d::property::set_internal_value(crop_window->crop_left(), 0.0);
				k3d::property::set_internal_value(crop_window->crop_right(), 1.0);
				k3d::property::set_internal_value(crop_window->crop_top(), 0.0);
				k3d::property::set_internal_value(crop_window->crop_bottom(), 1.0);

				return true;
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
		}

		return false;
	}

	/// Stores the owning document
	document_state& m_document_state;
	/// Draws a rubber-band selection into the viewport
	rubber_band m_rubber_band;
	/// Provides interactive navigation behavior
	navigation_input_model m_navigation_model;
	/// Dispatches incoming user input events
	basic_viewport_input_model m_input_model;
	/// Emitted to record command-node commands
	sigc::signal<void, const std::string&, const std::string&> m_command_signal;

	k3d::timer m_timer;
};

/////////////////////////////////////////////////////////////////////////////
// render_region_tool

render_region_tool::render_region_tool(document_state& DocumentState, const std::string& Name) :
	base(DocumentState, Name),
	m_implementation(new implementation(DocumentState))
{
	m_implementation->m_navigation_model.connect_command_signal(sigc::mem_fun(*this, &render_region_tool::record_command));
	m_implementation->m_command_signal.connect(sigc::mem_fun(*this, &render_region_tool::record_command));
}

render_region_tool::~render_region_tool()
{
	delete m_implementation;
}

const k3d::icommand_node::result render_region_tool::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(m_implementation->execute_command(Command, Arguments))
		return RESULT_CONTINUE;

	if(m_implementation->m_navigation_model.execute_command(Command, Arguments))
		return RESULT_CONTINUE;

	return RESULT_ERROR;
}

viewport_input_model& render_region_tool::get_input_model()
{
	return m_implementation->m_input_model;
}

} // namespace libk3dngui


