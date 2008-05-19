#ifndef NGUI_VIEWPORT_H
#define NGUI_VIEWPORT_H

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

#include <gtkmm/drawingarea.h>

#include "panel.h"
#include "ui_component.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/irender_camera_animation.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/line3.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/signal_system.h>

namespace k3d { namespace gl { class selection_state; } }

namespace libk3dngui
{

class document_state;

namespace viewport
{

/////////////////////////////////////////////////////////////////////////////
// control

class control :
        public Gtk::DrawingArea,
	public ui_component,
	public k3d::property_collection,
	public k3d::irender_camera_frame,
	public k3d::irender_camera_animation,
	public panel::control
{
	typedef Gtk::DrawingArea base;

public:
	control(document_state& DocumentState, k3d::icommand_node& Parent);

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent);
	const k3d::string_t panel_type();
	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot);

	/// Returns the owning document
	k3d::idocument& document();
	/// Returns the camera for this viewport
	k3d::icamera* const camera();
	/// Returns the OpenGL render engine for this viewport
	k3d::gl::irender_viewport* const gl_engine();
	/// Returns the preview render engine for this viewport
	k3d::irender_camera_preview* const camera_preview_engine();
	/// Returns the still render engine for this viewport
	k3d::irender_camera_frame* const camera_still_engine();
	/// Returns the animation render engine for this viewport
	k3d::irender_camera_animation* const camera_animation_engine();

	/// Sets the camera for this viewport
	void set_camera(k3d::icamera* const Camera);
	/// Sets the OpenGL render engine for this viewport
	void set_gl_engine(k3d::gl::irender_viewport* const Engine);
	/// Sets the current preview render engine for this viewport
	void set_camera_preview_engine(k3d::irender_camera_preview* const Engine);
	/// Sets the current still render engine for this viewport
	void set_camera_still_engine(k3d::irender_camera_frame* const Engine);
	/// Sets the current animation render engine for this viewport
	void set_camera_animation_engine(k3d::irender_camera_animation* const Engine);

	/// Returns the viewport view matrix
	const k3d::matrix4 get_view_matrix();
	/// Sets the viewport view matrix
	void set_view_matrix(const k3d::matrix4& Matrix);
	/// Returns the viewport "target", the point (in world coordinates) around which the viewport orbits
	const k3d::point3 get_target();
	/// Sets the viewport "target", the point (in world coordinates) around which the viewport orbits
	void set_target(const k3d::point3& Target);
	/// Returns the viewport "up" axis, the axis (in world coordinates) that defines "up" for modeling purposes
	const k3d::vector3 get_up_axis();

	/// Returns the most recent OpenGL viewport parameters
	void get_gl_viewport(GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4]);
	/// Projects a point in world coordinates into screen space, returning the 2D widget coordinates
	const k3d::point2 project(const k3d::point3& WorldCoords);

	bool render_camera_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const bool ViewCompletedImage);
	bool render_camera_animation(k3d::icamera& Camera, k3d::iproperty& Time, const k3d::frames& Frames, const bool ViewCompletedImages);

	/// Returns all points contained in the given rectangle in widget coordinates
	k3d::selection::records get_selectable_points(const k3d::rectangle& SelectionRegion, bool Backfacing);
	/// Returns all lines that intersect the given rectangle in widget coordinates
	k3d::selection::records get_selectable_lines(const k3d::rectangle& SelectionRegion, bool Backfacing);
	/// Returns all faces that intersect the given rectangle in widget coordinates
	k3d::selection::records get_selectable_faces(const k3d::rectangle& SelectionRegion, bool Backfacing);
	/// Returns all nodes that intersect the given rectangle in widget coordinates
	k3d::selection::records get_selectable_nodes(const k3d::rectangle& SelectionRegion);
	/// Returns all objects (points, lines, faces, or nodes, depending on selection mode) that intersect the given rectangle in widget coordinates
	k3d::selection::records get_selectable_objects(const k3d::rectangle& SelectionRegion, bool Backfacing);

	/// Returns the closest point at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_point(const k3d::point2& Coordinates, bool Backfacing);
	/// Returns the closest line at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_line(const k3d::point2& Coordinates, bool Backfacing);
	/// Returns the closest face at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_face(const k3d::point2& Coordinates, bool Backfacing);
	/// Returns the closest node at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_node(const k3d::point2& Coordinates);
	/// Returns the closest object (point, line, face, or node, depending on selection mode) at the given coordinates (may return an empty record)
	k3d::selection::record pick_object(const k3d::point2& Coordinates, bool Backfacing);

	/// Returns the closest point at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_point(const k3d::point2& Coordinates, k3d::selection::records& Records, bool Backfacing);
	/// Returns the closest line at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_line(const k3d::point2& Coordinates, k3d::selection::records& Records, bool Backfacing);
	/// Returns the closest face at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_face(const k3d::point2& Coordinates, k3d::selection::records& Records, bool Backfacing);
	/// Returns the closest node at the given widget coordinates (may return an empty record)
	k3d::selection::record pick_node(const k3d::point2& Coordinates, k3d::selection::records& Records);
	/// Returns the closest object (point, line, face, or node, depending on selection mode) at the given coordinates (may return an empty record)
	k3d::selection::record pick_object(const k3d::point2& Coordinates, k3d::selection::records& Records, bool Backfacing);

private:
	void on_camera_changed(k3d::iunknown*);
	void on_gl_engine_changed(k3d::iunknown*);

	void on_redraw_request(k3d::gl::irender_viewport::redraw_type_t RedrawType);
	void on_style_changed(const Glib::RefPtr<Gtk::Style>& previous_style);
	bool on_redraw();
	const GLint select(const k3d::gl::selection_state& SelectState, const k3d::rectangle& SelectionRegion);
	const GLint select(const k3d::gl::selection_state& SelectState, const k3d::rectangle& SelectionRegion, GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4]);

	/// Returns an OpenGL selection as a collection of records
	const k3d::selection::records get_selection(const k3d::gl::selection_state& SelectionState, const k3d::rectangle& SelectionRegion);
	/// Returns an OpenGL selection as a collection of records
	const k3d::selection::records get_selection(const k3d::gl::selection_state& SelectionState, const k3d::rectangle& SelectionRegion, GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4]);

	/// Renders the current view to disk
	bool save_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const bool ViewCompletedImage);
	/// Caches the current font as an OpenGL display list
	void create_font();

	/// Called when a mouse button is pressed
	bool on_button_press_event(GdkEventButton* Event);
	/// Called when a mouse button is released
	bool on_button_release_event(GdkEventButton* Event);
	/// Called when the mouse wheel is scrolled
	bool on_scroll_event(GdkEventScroll* Event);
	/// Called when the mouse is moved
	bool on_motion_notify_event(GdkEventMotion* Event);
	/// Called when a key is pressed
	bool on_key_press_event(GdkEventKey* Event);
	/// Called when a key is released
	bool on_key_release_event(GdkEventKey* Event);

	class implementation;
	implementation* const m_implementation;
};

} // namespace viewport

/////////////////////////////////////////////////////////////////////////////
// widget_to_ndc

/// Converts widget coordinates to normalized camera coordinates - note: results may be outside the range [0, 1] because the viewport and camera aspect ratios may not match
const k3d::point2 widget_to_ndc(viewport::control& Viewport, const k3d::point2& WidgetCoords);

/////////////////////////////////////////////////////////////////////////////
// ndc_to_widget

/// Converts normalized camera coordinates to widget coordinates
const k3d::point2 ndc_to_widget(viewport::control& Viewport, const k3d::point2& NDC);

/////////////////////////////////////////////////////////////////////////////
// mouse_to_world

/// Converts widget coordinates to a line in world coordinates
const k3d::line3 mouse_to_world(viewport::control& Viewport, const k3d::point2& WidgetCoords);

} // namespace libk3dngui

#endif // NGUI_VIEWPORT_H

