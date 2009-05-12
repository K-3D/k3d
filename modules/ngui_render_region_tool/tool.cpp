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
#include <k3dsdk/color.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/icrop_window.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/basic_viewport_input_model.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/interactive.h>
#include <k3dsdk/ngui/keyboard.h>
#include <k3dsdk/ngui/navigation_input_model.h>
#include <k3dsdk/ngui/render.h>
#include <k3dsdk/ngui/rubber_band.h>
#include <k3dsdk/ngui/tool.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/ngui/viewport.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/state_change_set.h>

#include <boost/assign/list_of.hpp>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace render_region
{

/////////////////////////////////////////////////////////////////////////////
// implementation

struct implementation
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
				k3d::property::pipeline_value<double>(crop_window->crop_left()),
				k3d::property::pipeline_value<double>(crop_window->crop_right()),
				k3d::property::pipeline_value<double>(crop_window->crop_top()),
				k3d::property::pipeline_value<double>(crop_window->crop_bottom())
				);

			if(!window.contains(widget_to_ndc(Viewport, k3d::point2(Event.x, Event.y))))
			{
				k3d::record_state_change_set change_set(m_document_state.document(), _("Reset Camera Crop Window"), K3D_CHANGE_SET_CONTEXT);
				k3d::property::set_internal_value(crop_window->crop_left(), 0.0);
				k3d::property::set_internal_value(crop_window->crop_right(), 1.0);
				k3d::property::set_internal_value(crop_window->crop_top(), 0.0);
				k3d::property::set_internal_value(crop_window->crop_bottom(), 1.0);

				return;
			}
		}

		// Otherwise, render a preview ...
		k3d::icamera* camera = Viewport.camera();
		if(!camera)
			camera = pick_camera(m_document_state);
		if(!camera)
			return;

		k3d::irender_camera_preview* render_engine = Viewport.camera_preview_engine();
		if(!render_engine)
			render_engine = pick_camera_preview_render_engine(m_document_state);
		if(!render_engine)
			return;

		Viewport.set_camera(camera);
		Viewport.set_camera_preview_engine(render_engine);

		render(*camera, *render_engine);
	}

	void on_lbutton_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		m_rubber_band.box = k3d::rectangle(Event.x, Event.x, Event.y, Event.y);
		m_rubber_band.draw(Viewport);

		m_timer.restart();
	}

	void on_lbutton_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		m_rubber_band.draw(Viewport);
		m_rubber_band.box.right = Event.x;
		m_rubber_band.box.bottom = Event.y;
		m_rubber_band.draw(Viewport);
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
	}

	void on_rbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		m_document_state.set_active_tool(m_document_state.selection_tool());
	}

	/// Stores the owning document
	document_state& m_document_state;
	/// Draws a rubber-band selection into the viewport
	rubber_band m_rubber_band;
	/// Provides interactive navigation behavior
	navigation_input_model m_navigation_model;
	/// Dispatches incoming user input events
	basic_viewport_input_model m_input_model;

	k3d::timer m_timer;
};

/////////////////////////////////////////////////////////////////////////////
// tool

/// User-interface tool that provides interactive controls for cropped rendering
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

	virtual k3d::ngui::viewport_input_model& get_input_model()
	{
		return m_implementation->m_input_model;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<tool> factory(
			k3d::uuid(0x96a0445e, 0x85412ced, 0xfa1b9ca5, 0xd74e4922),
			"NGUIRenderRegionTool",
			_("Provides interactive controls for cropped rendering."),
			"NGUI Tool",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "tool"));

		return factory;
	}

private:
	virtual void on_initialize(document_state& DocumentState)
	{
		m_implementation = new render_region::implementation(DocumentState);
	}

	render_region::implementation* m_implementation;
};

} // namespace render_region

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::render_region::tool::get_factory());
K3D_MODULE_END

