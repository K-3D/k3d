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
		\author Tim Shead (tshead@k-3d.com)
*/

#include <gdkmm/display.h>

#include "command_arguments.h"
#include "context_menu.h"
#include "document_state.h"
#include "interactive.h"
#include "navigation_input_model.h"
#include "target.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/high_res_timer.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/property.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>
#include <k3dsdk/xml.h>

#include <gtkmm/menu.h>

namespace libk3dngui
{

namespace detail
{

/// Returns the angle of this vector with respect to the positive X axis in radians [-pi, pi]
double angle(const k3d::vector2& Vector)
{
	return atan2(Vector[1], Vector[0]);
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// navigation_input_model::implementation

struct navigation_input_model::implementation
{
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState)
	{
	}

	void on_button1_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		return_if_fail(interactive_target(Viewport));
		return_if_fail(Viewport.camera());

		if(Event.state & GDK_CONTROL_MASK)
		{
			motion_state = MOTION_ZOOM;
		}
		else if(Event.state & GDK_SHIFT_MASK)
		{
			motion_state = MOTION_PAN_TILT;
		}
		else
		{
			motion_state = MOTION_TRACK;
		}

		k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);
		m_last_mouse = screen_pointer_coordinates(Viewport);
		m_timer.restart();

		switch(motion_state)
		{
			case MOTION_ZOOM:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("start_zoom", arguments);
				break;
			}
			case MOTION_PAN_TILT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("start_pan_tilt", arguments);
				break;
			}
			case MOTION_TRACK:
			{
				m_track_sensitivity = k3d::distance(k3d::position(Viewport.get_view_matrix()), Viewport.get_target());

				// Adapt sensitivity to FOV
				double top = 0;
				double bottom = 0;
				double near = 0;
				if(k3d::iperspective* const perspective = dynamic_cast<k3d::iperspective*>(&Viewport.camera()->projection()))
				{
					top = k3d::property::pipeline_value<double>(perspective->top());
					bottom = k3d::property::pipeline_value<double>(perspective->bottom());
					near = k3d::property::pipeline_value<double>(perspective->near());

				}
				else if(k3d::iorthographic* const orthographic = dynamic_cast<k3d::iorthographic*>(&Viewport.camera()->projection()))
				{
					top = k3d::property::pipeline_value<double>(orthographic->top());
					bottom = k3d::property::pipeline_value<double>(orthographic->bottom());
					near = k3d::property::pipeline_value<double>(orthographic->near());
				}

				if(near > 0)
				{
					const double height = top - bottom;
					const double tan_fov = height / near;
					m_track_sensitivity *= tan_fov;
				}

				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("start_track", arguments);
				break;
			}
			default:
				assert_not_reached();
		}
	}

	void on_button2_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		return_if_fail(interactive_target(Viewport));
		return_if_fail(Viewport.camera());

		if(Event.state & GDK_CONTROL_MASK)
		{
			motion_state = MOTION_DOLLY;
		}
		else if(Event.state & GDK_SHIFT_MASK)
		{
			motion_state = MOTION_ROLL;
		}
		else
		{
			motion_state = MOTION_ORBIT;
		}

		k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);
		m_last_mouse = screen_pointer_coordinates(Viewport);
		m_timer.restart();

		switch(motion_state)
		{
			case MOTION_DOLLY:
			{
				const double target_distance = k3d::distance(k3d::position(Viewport.get_view_matrix()), Viewport.get_target());
				m_dolly_sensitivity = target_distance ? target_distance : 0.001;
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("start_dolly", arguments);
				break;
			}
			case MOTION_ROLL:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("start_roll", arguments);
				break;
			}
			case MOTION_ORBIT:
			{
				command_arguments arguments;
				arguments.append_viewport_coordinates("mouse", Viewport, Event);
				m_command_signal.emit("start_orbit", arguments);
				break;
			}
			default:
				assert_not_reached();
		}
	}

	void on_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		return_if_fail(Viewport.camera());

		switch(motion_state)
		{
			case MOTION_TRACK:
				on_track_motion(Viewport, Event);
				break;
			case MOTION_DOLLY:
				on_dolly_motion(Viewport, Event);
				break;
			case MOTION_ZOOM:
				on_zoom_motion(Viewport, Event);
				break;
			case MOTION_PAN_TILT:
				on_pan_tilt_motion(Viewport, Event);
				break;
			case MOTION_ORBIT:
				on_orbit_motion(Viewport, Event);
				break;
			case MOTION_ROLL:
				on_roll_motion(Viewport, Event);
				break;
		}
	}

	void on_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
	{
		return_if_fail(Viewport.camera());

		Viewport.get_window()->set_cursor();

		switch(motion_state)
		{
			case MOTION_TRACK:
				k3d::finish_state_change_set(m_document_state.document(), _("Track Viewport"), K3D_CHANGE_SET_CONTEXT);
				m_command_signal.emit("finish_track", "");
				break;
			case MOTION_DOLLY:
				k3d::finish_state_change_set(m_document_state.document(), _("Dolly Viewport"), K3D_CHANGE_SET_CONTEXT);
				m_command_signal.emit("finish_dolly", "");
				break;
			case MOTION_ZOOM:
				k3d::finish_state_change_set(m_document_state.document(), _("Zoom Viewport"), K3D_CHANGE_SET_CONTEXT);
				m_command_signal.emit("finish_zoom", "");
				break;
			case MOTION_PAN_TILT:
				k3d::finish_state_change_set(m_document_state.document(), _("Pan & Tilt Viewport"), K3D_CHANGE_SET_CONTEXT);
				m_command_signal.emit("finish_pan_tilt", "");
				break;
			case MOTION_ORBIT:
				k3d::finish_state_change_set(m_document_state.document(), _("Orbit Viewport"), K3D_CHANGE_SET_CONTEXT);
				m_command_signal.emit("finish_orbit", "");
				break;
			case MOTION_ROLL:
				k3d::finish_state_change_set(m_document_state.document(), _("Roll Viewport"), K3D_CHANGE_SET_CONTEXT);
				m_command_signal.emit("finish_roll", "");
				break;
		}
	}

	/// Called to track the viewport (move left/right/up/down in screen space) in response to mouse motion
	void on_track_motion(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
		const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
		const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
		const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
		const k3d::point3 position = k3d::position(view_matrix);

		const k3d::point2 current_mouse = screen_pointer_coordinates(Viewport);
		const k3d::point2 current_ndc = ndc(Viewport, current_mouse);
		const k3d::point2 last_ndc = ndc(Viewport, m_last_mouse);

		const double direction = 1.0; // Change the sign to set the One True Tracking Behavior
		const double deltax = last_ndc[0] - current_ndc[0];
		const double deltay = last_ndc[1] - current_ndc[1];

		const k3d::vector3 offset =
			(m_track_sensitivity * direction * deltax * right_vector) +
			(m_track_sensitivity * direction * deltay * up_vector);

		const k3d::point3 new_position = position + offset;
		const k3d::point3 new_target = Viewport.get_target() + offset;
		const k3d::matrix4 new_view_matrix = k3d::view_matrix(look_vector, up_vector, new_position);

		Viewport.set_view_matrix(new_view_matrix);
		Viewport.set_target(new_target);

		command_arguments arguments;
		arguments.append_viewport(Viewport);
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("timestamp", m_timer.elapsed());
		arguments.append("new_view_matrix", new_view_matrix);
		arguments.append("new_target", new_target);
		m_command_signal.emit("track_motion", arguments);

		m_last_mouse = current_mouse;
		wrap_mouse_pointer(Viewport);
	}

	/// Called to dolly the viewport (move forward/backward in screen space) in response to mouse motion
	void on_dolly_motion(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::point2 current_mouse = screen_pointer_coordinates(Viewport);
		const double deltay = ndc(Viewport, current_mouse)[1] - ndc(Viewport, m_last_mouse)[1];

		const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
		const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
		const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
		const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
		const k3d::point3 position = k3d::position(view_matrix);
		const k3d::point3 new_position = position + (deltay * m_dolly_sensitivity * look_vector);
		const k3d::matrix4 new_view_matrix = k3d::view_matrix(look_vector, up_vector, new_position);

		Viewport.set_view_matrix(new_view_matrix);

		command_arguments arguments;
		arguments.append_viewport(Viewport);
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("timestamp", m_timer.elapsed());
		arguments.append("new_view_matrix", new_view_matrix);
		m_command_signal.emit("dolly_motion", arguments);

		m_last_mouse = current_mouse;
		wrap_mouse_pointer(Viewport);
	}

	/// Called to zoom the viewport (change focal length) in response to mouse motion
	void on_zoom_motion(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::point2 current_mouse = screen_pointer_coordinates(Viewport);
		const k3d::point2 current_ndc = ndc(Viewport, current_mouse);
		const k3d::point2 last_ndc = ndc(Viewport, m_last_mouse);

		const double sensitivity = 4.0;
		const double zoom_factor = (current_ndc[1] < last_ndc[1]) ? std::pow(sensitivity, last_ndc[1] - current_ndc[1]) : std::pow(1 / sensitivity, current_ndc[1] - last_ndc[1]);

		k3d::iprojection* const projection = Viewport.camera() ? &Viewport.camera()->projection() : 0;
		return_if_fail(projection);

		if(k3d::iperspective* const perspective = dynamic_cast<k3d::iperspective*>(projection))
		{
			const double left = boost::any_cast<double>(perspective->left().property_value()) * zoom_factor;
			const double right = boost::any_cast<double>(perspective->right().property_value()) * zoom_factor;
			const double top = boost::any_cast<double>(perspective->top().property_value()) * zoom_factor;
			const double bottom = boost::any_cast<double>(perspective->bottom().property_value()) * zoom_factor;

			k3d::property::set_internal_value(perspective->left(), left);
			k3d::property::set_internal_value(perspective->right(), right);
			k3d::property::set_internal_value(perspective->top(), top);
			k3d::property::set_internal_value(perspective->bottom(), bottom);

			command_arguments arguments;
			arguments.append_viewport(Viewport);
			arguments.append_viewport_coordinates("mouse", Viewport, Event);
			arguments.append("timestamp", m_timer.elapsed());
			arguments.append("left", left);
			arguments.append("right", right);
			arguments.append("top", top);
			arguments.append("bottom", bottom);
			m_command_signal.emit("zoom_motion", arguments);

			m_last_mouse = current_mouse;
			wrap_mouse_pointer(Viewport);
			return;
		}

		if(k3d::iorthographic* const orthographic = dynamic_cast<k3d::iorthographic*>(projection))
		{
			const double left = boost::any_cast<double>(orthographic->left().property_value()) * zoom_factor;
			const double right = boost::any_cast<double>(orthographic->right().property_value()) * zoom_factor;
			const double top = boost::any_cast<double>(orthographic->top().property_value()) * zoom_factor;
			const double bottom = boost::any_cast<double>(orthographic->bottom().property_value()) * zoom_factor;

			k3d::property::set_internal_value(orthographic->left(), left);
			k3d::property::set_internal_value(orthographic->right(), right);
			k3d::property::set_internal_value(orthographic->top(), top);
			k3d::property::set_internal_value(orthographic->bottom(), bottom);

			command_arguments arguments;
			arguments.append_viewport_coordinates("mouse", Viewport, Event);
			arguments.append("timestamp", m_timer.elapsed());
			arguments.append("left", left);
			arguments.append("right", right);
			arguments.append("top", top);
			arguments.append("bottom", bottom);
			m_command_signal.emit("zoom_motion", arguments);

			m_last_mouse = current_mouse;
			wrap_mouse_pointer(Viewport);
			return;
		}

		k3d::log() << error << "Unknown projection type" << std::endl;
	}

	/// Called to orbit the viewport (rotate it around an arbitrary origin) in response to mouse motion
	void on_orbit_motion(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
		const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
		const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
		const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
		const k3d::point3 position = k3d::position(view_matrix);
		const k3d::point3 target = Viewport.get_target();

		const k3d::point2 current_mouse = screen_pointer_coordinates(Viewport);
		const k3d::point2 current_ndc = ndc(Viewport, current_mouse);
		const k3d::point2 last_ndc = ndc(Viewport, m_last_mouse);

/* This snippet orbits the camera within the view plane.
   Pros: works intuitively regardless of camera orientation, completely axis-independent.
   Cons: weak for modeling, the camera quickly becomes "tilted" WRT the ground plane.

		const k3d::point3 target = Viewport.get_target();
		const double target_distance = k3d::length(position - target);

		const double direction = 1.0;
		const double sensitivity = k3d::pi() * target_distance;
		const double deltax = last_ndc[0] - current_ndc[0];
		const double deltay = last_ndc[1] - current_ndc[1];

		const k3d::vector3 offset =
			(sensitivity * direction * deltax * right_vector) +
			(sensitivity * direction * deltay * up_vector);

		k3d::point3 new_position = position + offset;
		new_position *= (target_distance / k3d::length(target - new_position));
		const k3d::vector3 new_look_vector = k3d::to_vector(target - new_position);

		Viewport.set_view_matrix(k3d::view_matrix(new_look_vector, up_vector, right_vector, new_position));
*/

		const double direction = -1.0;
		const double sensitivity = k3d::pi();
		const double ysensitivity = -1.0;
		const double deltax = last_ndc[0] - current_ndc[0];
		const double deltay = last_ndc[1] - current_ndc[1];

		const k3d::matrix4 matrix =
			k3d::rotation3D(k3d::angle_axis(sensitivity * direction * deltax, Viewport.get_up_axis())) *
			k3d::rotation3D(k3d::angle_axis(ysensitivity * sensitivity * direction * deltay, right_vector));

		const k3d::vector3 new_look_vector = matrix * look_vector;
		const k3d::vector3 new_up_vector = matrix * up_vector;
		const k3d::vector3 new_right_vector = matrix * right_vector;
		const k3d::point3 new_position = target + (matrix * (position - target));

		const k3d::matrix4 new_view_matrix = k3d::view_matrix(new_look_vector, new_up_vector, new_position);

		Viewport.set_view_matrix(new_view_matrix);

		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("timestamp", m_timer.elapsed());
		arguments.append("new_view_matrix", new_view_matrix);
		m_command_signal.emit("orbit_motion", arguments);

		m_last_mouse = current_mouse;
		wrap_mouse_pointer(Viewport);
	}

	/// Called to aim the viewport (pan and tilt) in response to mouse motion
	void on_pan_tilt_motion(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
		const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
		const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
		const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
		const k3d::point3 position = k3d::position(view_matrix);
		const double target_distance = k3d::distance(k3d::position(Viewport.get_view_matrix()), Viewport.get_target());

		const k3d::point2 current_mouse = screen_pointer_coordinates(Viewport);
		const k3d::point2 current_ndc = ndc(Viewport, current_mouse);
		const k3d::point2 last_ndc = ndc(Viewport, m_last_mouse);

		const double theta = current_ndc[0] - last_ndc[0];
		const double phi = -(current_ndc[1] - last_ndc[1]);

		const k3d::matrix4 matrix = k3d::rotation3D(k3d::angle_axis(theta, up_vector)) * k3d::rotation3D(k3d::angle_axis(phi, right_vector));

		const k3d::vector3 new_look_vector = matrix * look_vector;
		const k3d::vector3 new_up_vector = matrix * up_vector;
		const k3d::vector3 new_right_vector = matrix * right_vector;

		const k3d::matrix4 new_view_matrix = k3d::view_matrix(new_look_vector, new_up_vector, position);
		const k3d::point3 new_target = position + (target_distance * k3d::normalize(new_look_vector));

		Viewport.set_view_matrix(new_view_matrix);
		Viewport.set_target(new_target);

		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("timestamp", m_timer.elapsed());
		arguments.append("new_view_matrix", new_view_matrix);
		arguments.append("new_target", new_target);
		m_command_signal.emit("pan_tilt_motion", arguments);

		m_last_mouse = current_mouse;
		wrap_mouse_pointer(Viewport);
	}

	/// Called to roll the viewport (dutch tilt) in response to mouse motion
	void on_roll_motion(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
		const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
		const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
		const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
		const k3d::point3 position = k3d::position(view_matrix);

		const k3d::point2 current_mouse = screen_pointer_coordinates(Viewport);
		const k3d::point2 current_ndc = ndc(Viewport, current_mouse);
		const k3d::point2 last_ndc = ndc(Viewport, m_last_mouse);
		const double theta = detail::angle(k3d::to_vector(current_ndc)) - detail::angle(k3d::to_vector(last_ndc));

		const k3d::matrix4 roll_matrix = k3d::rotation3D(k3d::angle_axis(-theta, look_vector));
		const k3d::vector3 new_up_vector = roll_matrix * up_vector;
		const k3d::vector3 new_right_vector = roll_matrix * right_vector;

		const k3d::matrix4 new_view_matrix = k3d::view_matrix(look_vector, new_up_vector, position);

		Viewport.set_view_matrix(new_view_matrix);

		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("timestamp", m_timer.elapsed());
		arguments.append("new_view_matrix", new_view_matrix);
		m_command_signal.emit("roll_motion", arguments);

		m_last_mouse = current_mouse;
	}

	/// Called to dolly the viewport (move forward / backward in screen space) in response to mouse-wheel motion
	void on_scroll(viewport::control& Viewport, const GdkEventScroll& Event)
	{
		return_if_fail(Viewport.camera());

		const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
		const k3d::vector3 look = k3d::look_vector(view_matrix);
		const k3d::vector3 up = k3d::up_vector(view_matrix);
		const k3d::vector3 right = k3d::right_vector(view_matrix);
		const k3d::point3 position = k3d::position(view_matrix);

		const double target_distance = k3d::distance(position, Viewport.get_target());
		if(!target_distance)
			m_dolly_sensitivity = 0.001;
		else
			m_dolly_sensitivity = target_distance * 0.1;

		double direction = 0;
		std::string direction_label;
		std::string change_set_label;
		switch(Event.direction)
		{
			case GDK_SCROLL_UP:
				direction = 1;
				direction_label = "forward";
				change_set_label = _("Dolly Forward");
				break;
			case GDK_SCROLL_DOWN:
				direction = -1;
				direction_label = "backward";
				change_set_label = _("Dolly Backward");
				break;
			default:
				break;
		}

		const k3d::matrix4 new_view_matrix = k3d::view_matrix(look, up, position + (m_dolly_sensitivity * direction * look));

		k3d::record_state_change_set change_set(m_document_state.document(), change_set_label, K3D_CHANGE_SET_CONTEXT);
		Viewport.set_view_matrix(new_view_matrix);

		command_arguments arguments;
		arguments.append_viewport(Viewport);
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		arguments.append("direction", direction_label);
		arguments.append("new_view_matrix", new_view_matrix);
		m_command_signal.emit("scroll_dolly", arguments);
	}

	void on_button1_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		return_if_fail(Viewport.camera());

		const k3d::selection::record selection = Viewport.pick_node(k3d::point2(Event.x, Event.y));
		if(!selection.empty())
		{
			// Get the new target position in world coordinates ...
			k3d::inode* const node = k3d::selection::get_node(selection);
			const k3d::point3 new_target = node ? k3d::world_position(*node) : k3d::point3(0, 0, 0);

			const k3d::matrix4 view_matrix = Viewport.get_view_matrix();
			const k3d::vector3 look_vector = k3d::look_vector(view_matrix);
			const k3d::vector3 up_vector = k3d::up_vector(view_matrix);
			const k3d::vector3 right_vector = k3d::right_vector(view_matrix);
			const k3d::point3 position = k3d::position(view_matrix);

			const k3d::vector3 new_look_vector = new_target - position;
			const k3d::vector3 new_right_vector = new_look_vector ^ Viewport.get_up_axis();
			const k3d::vector3 new_up_vector = new_right_vector ^ new_look_vector;

			const k3d::matrix4 new_view_matrix = k3d::view_matrix(new_look_vector, new_up_vector, position);

			command_arguments arguments;
			arguments.append_viewport_coordinates("mouse", Viewport, Event);
			arguments.append("new_view_matrix", new_view_matrix);
			arguments.append("new_target", new_target);
			m_command_signal.emit("pick_target", arguments);

			k3d::record_state_change_set change_set(m_document_state.document(), _("Pick Target"), K3D_CHANGE_SET_CONTEXT);
			Viewport.set_view_matrix(new_view_matrix);
			Viewport.set_target(new_target);
		}
		else
		{
			command_arguments arguments;
			arguments.append_viewport_coordinates("mouse", Viewport, Event);
			m_command_signal.emit("aim_selection", arguments);

			k3d::record_state_change_set change_set(m_document_state.document(), _("Aim Selection"), K3D_CHANGE_SET_CONTEXT);
			aim_selection(m_document_state, Viewport);
		}
	}

	void on_context_menu(viewport::control& Viewport, const GdkEventButton& Event)
	{
		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);
		m_command_signal.emit("context_menu", arguments);

		m_document_state.popup_context_menu();
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		try
		{
			if(Command == "start_track")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				m_timer.restart();
				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "track_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::matrix4 new_view_matrix = arguments.get_matrix4("new_view_matrix");
				const k3d::point3 new_target = arguments.get_point3("new_target");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				viewport.set_view_matrix(new_view_matrix);
				viewport.set_target(new_target);
				k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "finish_track")
			{
				k3d::finish_state_change_set(m_document_state.document(), _("Track Viewport"), K3D_CHANGE_SET_CONTEXT);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "start_zoom")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				m_timer.restart();
				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "zoom_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const double left = arguments.get_double("left");
				const double right = arguments.get_double("right");
				const double top = arguments.get_double("top");
				const double bottom = arguments.get_double("bottom");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				k3d::iprojection* const projection = viewport.camera() ? &viewport.camera()->projection() : 0;
				return_val_if_fail(projection, k3d::icommand_node::RESULT_ERROR);

				if(k3d::iperspective* const perspective = dynamic_cast<k3d::iperspective*>(projection))
				{
					k3d::property::set_internal_value(perspective->left(), left);
					k3d::property::set_internal_value(perspective->right(), right);
					k3d::property::set_internal_value(perspective->top(), top);
					k3d::property::set_internal_value(perspective->bottom(), bottom);
					k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

					return k3d::icommand_node::RESULT_CONTINUE;
				}

				if(k3d::iorthographic* const orthographic = dynamic_cast<k3d::iorthographic*>(projection))
				{
					k3d::property::set_internal_value(orthographic->left(), left);
					k3d::property::set_internal_value(orthographic->right(), right);
					k3d::property::set_internal_value(orthographic->top(), top);
					k3d::property::set_internal_value(orthographic->bottom(), bottom);
					k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

					return k3d::icommand_node::RESULT_CONTINUE;
				}

				k3d::log() << error << "Unknown projection type" << std::endl;
				return k3d::icommand_node::RESULT_ERROR;
			}
			else if(Command == "finish_zoom")
			{
				k3d::finish_state_change_set(m_document_state.document(), _("Zoom Viewport"), K3D_CHANGE_SET_CONTEXT);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "start_pan_tilt")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				m_timer.restart();
				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "pan_tilt_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::matrix4 new_view_matrix = arguments.get_matrix4("new_view_matrix");
				const k3d::point3 new_target = arguments.get_point3("new_target");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				viewport.set_view_matrix(new_view_matrix);
				viewport.set_target(new_target);
				k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "finish_pan_tilt")
			{
				k3d::finish_state_change_set(m_document_state.document(), _("Pan & Tilt Viewport"), K3D_CHANGE_SET_CONTEXT);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "start_dolly")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				m_timer.restart();
				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "dolly_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::matrix4 new_view_matrix = arguments.get_matrix4("new_view_matrix");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				viewport.set_view_matrix(new_view_matrix);
				k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "finish_dolly")
			{
				k3d::finish_state_change_set(m_document_state.document(), _("Dolly Viewport"), K3D_CHANGE_SET_CONTEXT);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "start_roll")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				m_timer.restart();
				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "roll_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::matrix4 new_view_matrix = arguments.get_matrix4("new_view_matrix");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				viewport.set_view_matrix(new_view_matrix);
				k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "finish_roll")
			{
				k3d::finish_state_change_set(m_document_state.document(), _("Roll Viewport"), K3D_CHANGE_SET_CONTEXT);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "start_orbit")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				m_timer.restart();
				interactive::move_pointer(viewport, mouse);

				k3d::start_state_change_set(m_document_state.document(), K3D_CHANGE_SET_CONTEXT);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "orbit_motion")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const double timestamp = arguments.get_double("timestamp");
				const k3d::matrix4 new_view_matrix = arguments.get_matrix4("new_view_matrix");

				interactive::warp_pointer(viewport, mouse, timestamp, m_timer);

				viewport.set_view_matrix(new_view_matrix);
				k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "finish_orbit")
			{
				k3d::finish_state_change_set(m_document_state.document(), _("Orbit Viewport"), K3D_CHANGE_SET_CONTEXT);
				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "context_menu")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, mouse);

				m_document_state.popup_context_menu(false);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "pick_target")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const k3d::matrix4 new_view_matrix = arguments.get_matrix4("new_view_matrix");
				const k3d::point3 new_target = arguments.get_point3("new_target");

				interactive::move_pointer(viewport, mouse);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Pick Target"), K3D_CHANGE_SET_CONTEXT);
				viewport.set_view_matrix(new_view_matrix);
				viewport.set_target(new_target);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "aim_selection")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");

				interactive::move_pointer(viewport, mouse);

				k3d::record_state_change_set change_set(m_document_state.document(), _("Aim Selection"), K3D_CHANGE_SET_CONTEXT);
				aim_selection(m_document_state, viewport);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
			else if(Command == "scroll_dolly")
			{
				command_arguments arguments(Arguments);
				viewport::control& viewport = arguments.get_viewport();
				const k3d::point2 mouse = arguments.get_viewport_point2("mouse");
				const std::string direction = arguments.get_string("direction");
				const k3d::matrix4 new_view_matrix = arguments.get_matrix4("new_view_matrix");

//				interactive::warp_pointer(viewport, mouse);
				
				std::string change_set_label;
				if(direction == "forward")
					change_set_label = _("Dolly Forward");
				else if(direction == "backward")
					change_set_label = _("Dolly Backward");

				k3d::record_state_change_set change_set(m_document_state.document(), change_set_label, K3D_CHANGE_SET_CONTEXT);
				viewport.set_view_matrix(new_view_matrix);

				k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::SYNCHRONOUS);

				non_blocking_sleep(0.2);

				return k3d::icommand_node::RESULT_CONTINUE;
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
			return k3d::icommand_node::RESULT_ERROR;
		}

		return k3d::icommand_node::RESULT_UNKNOWN_COMMAND;
	}

	void wrap_mouse_pointer(viewport::control& Viewport)
	{
		int x, y;
		Gdk::ModifierType modifiers;
		Gdk::Display::get_default()->get_pointer(x, y, modifiers);

		const int border = 5;
		const int width = Viewport.get_screen()->get_width();
		const int height = Viewport.get_screen()->get_height();

		if(x < border)
		{
			m_last_mouse = k3d::point2(width - (border + 1), y);
			interactive::warp_pointer(k3d::point2(width - (border + 1), y));
		}
		else if(width - x < border)
		{
			m_last_mouse = k3d::point2((border + 1), y);
			interactive::warp_pointer(k3d::point2((border + 1), y));
		}

		if(y < border)
		{
			m_last_mouse = k3d::point2(x, height - (border + 1));
			interactive::warp_pointer(k3d::point2(x, height - (border + 1)));
		}
		else if(height - y < border)
		{
			m_last_mouse = k3d::point2(x, (border + 1));
			interactive::warp_pointer(k3d::point2(x, (border + 1)));
		}
	}

	k3d::iunknown* interactive_target(viewport::control& Viewport)
	{
		return Viewport.camera() ? &Viewport.camera()->navigation_target() : 0;
	}

	k3d::iunknown* interactive_target(k3d::iunknown& Unknown)
	{
		viewport::control* const control = dynamic_cast<viewport::control*>(&Unknown);
		return_val_if_fail(control, 0);

		return control->camera() ? &control->camera()->navigation_target() : 0;
	}

	/// Converts screen coordinates into normalized device coordinates in the range [-0.5, 0.5]
	const k3d::point2 ndc(viewport::control& Viewport, const k3d::point2 ScreenCoords)
	{
		int x, y;
		Viewport.get_window()->get_origin(x, y);

		const double width = Viewport.get_width();
		const double height = Viewport.get_height();
		return_val_if_fail(width && height, k3d::point2());

		return k3d::point2(
			((ScreenCoords[0] - x) / width) - 0.5,
			0.5 - ((ScreenCoords[1] - y) / height));
	}

	/// Returns the current mouse position in screen coordinates
	const k3d::point2 screen_pointer_coordinates(viewport::control& Viewport)
	{
		int x, y;
		Gdk::ModifierType modifiers;
		Viewport.get_display()->get_pointer(x, y, modifiers);

		return k3d::point2(x, y);
	}

	/// Enumerates mouse motion types
	typedef enum
	{
		MOTION_TRACK,
		MOTION_DOLLY,
		MOTION_ZOOM,
		MOTION_PAN_TILT,
		MOTION_ORBIT,
		MOTION_ROLL,
	} motion_state_t;

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores the current mouse motion type
	motion_state_t motion_state;
	/// Stores the most recent mouse pointer coordinates
	k3d::point2 m_last_mouse;
	/// Controls the sensitivity of tracking movement while dragging
	double m_track_sensitivity;
	/// Controls the sensitivity of dollying movement while dragging
	double m_dolly_sensitivity;

	/// Context menu
	std::auto_ptr<Gtk::Menu> m_context_menu;

	/// Signal that will be emitted to record command-node commands
	sigc::signal<void, const std::string&, const std::string&> m_command_signal;

	k3d::timer m_timer;
};

/////////////////////////////////////////////////////////////////////////////
// navigation_input_model

navigation_input_model::navigation_input_model(document_state& DocumentState) :
	m_implementation(new implementation(DocumentState))
{
}

navigation_input_model::~navigation_input_model()
{
	delete m_implementation;
}

void navigation_input_model::on_button1_click(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_button1_click(Viewport, Event);
}

void navigation_input_model::on_button1_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
{
	m_implementation->on_button1_start_drag(Viewport, Event);
}

void navigation_input_model::on_button1_drag(viewport::control& Viewport, const GdkEventMotion& Event)
{
	m_implementation->on_drag(Viewport, Event);
}

void navigation_input_model::on_button1_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_end_drag(Viewport, Event);
}

void navigation_input_model::on_button2_click(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_context_menu(Viewport, Event);
}

void navigation_input_model::on_button2_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
{
	m_implementation->on_button2_start_drag(Viewport, Event);
}

void navigation_input_model::on_button2_drag(viewport::control& Viewport, const GdkEventMotion& Event)
{
	m_implementation->on_drag(Viewport, Event);
}

void navigation_input_model::on_button2_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
{
	m_implementation->on_end_drag(Viewport, Event);
}

void navigation_input_model::on_scroll(viewport::control& Viewport, const GdkEventScroll& Event)
{
	m_implementation->on_scroll(Viewport, Event);
}

sigc::connection navigation_input_model::connect_command_signal(const sigc::slot<void, const std::string&, const std::string&>& Slot)
{
	return m_implementation->m_command_signal.connect(Slot);
}

const k3d::icommand_node::result navigation_input_model::execute_command(const std::string& Command, const std::string& Arguments)
{
	return m_implementation->execute_command(Command, Arguments);
}

} // namespace libk3dngui

