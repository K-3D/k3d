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

#include <gtkmm/widget.h>

#include "document_state.h"
#include "iuser_input_model.h"
#include "modifiers.h"
#include "tool.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/basic_math.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/file_range.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/iselection_engine_gl.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/property.h>
#include <k3dsdk/selection_state_gl.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>

#include <gtk/gtkgl.h>
#include <gtk/gtkmain.h>

#include <k3dsdk/fstream.h>
#include <cassert>
#include <iomanip>
#include <sstream>

using namespace k3d::selection;

namespace libk3dngui
{

namespace viewport
{

namespace detail
{

/// Defines storage for a collection of OpenGL hit records
typedef std::vector<GLuint> gl_selection_buffer_t;

const k3d::gl::selection_state select_points()
{
	k3d::gl::selection_state result;

	result.exclude_unselected_nodes = true;
	result.select_points = true;
	result.select_point_groups = true;

	return result;
}

const k3d::gl::selection_state select_lines()
{
	k3d::gl::selection_state result;

	result.exclude_unselected_nodes = true;
	result.select_edges = true;
	result.select_linear_curves = true;
	result.select_cubic_curves = true;
	result.select_nurbs_curves = true;

	return result;
}

const k3d::gl::selection_state select_faces()
{
	k3d::gl::selection_state result;

	result.exclude_unselected_nodes = true;
	result.select_faces = true;
	result.select_bilinear_patches = true;
	result.select_bicubic_patches = true;
	result.select_nurbs_patches = true;

	return result;
}

const k3d::gl::selection_state select_nodes()
{
	k3d::gl::selection_state result;

	result.select_points = true;
	result.select_point_groups = true;
	result.select_edges = true;
	result.select_faces = true;
	result.select_linear_curves = true;
	result.select_cubic_curves = true;
	result.select_nurbs_curves = true;
	result.select_bilinear_patches = true;
	result.select_bicubic_patches = true;
	result.select_nurbs_patches = true;
	result.select_blobbies = true;

	return result;
}

struct sort_by_zmin
{
	bool operator()(const k3d::selection::record& LHS, const k3d::selection::record& RHS)
	{
		return LHS.zmin < RHS.zmin;
	}
};

/// Wrapper class for OpenGL hit records - designed to resemble an STL container
class hit_record
{
public:
	explicit hit_record(GLuint* const Storage) :
		m_storage(Storage)
	{
		assert(m_storage);
	}

	/// Returns the minimum Z depth of the hit
	GLuint zmin() const
	{
		return *(m_storage+1);
	}

	/// Returns the maximum Z depth of the hit
	GLuint zmax() const
	{
		return *(m_storage+2);
	}

	/// Returns true iff the hit was empty (i.e. doesn't contain any names)
	bool empty() const
	{
		return 0 == size();
	}

	/// Returns the number of names contained in the hit
	unsigned int size() const
	{
		return *m_storage;
	}

	/// Defines an iterator type for accessing hit names
	typedef GLuint* const_name_iterator;

	/// Returns an iterator designating the beginning of the range of hit names
	const_name_iterator name_begin() const
	{
		return m_storage+3;
	}

	/// Returns an iterator designating one-past-the-end of the range of hit names
	const_name_iterator name_end() const
	{
		return m_storage+3+size();
	}

	/// Defines a strict ordering for non-empty hit records based on minimum Z depth, since we typically want to find the "closest" hit
	friend bool operator<(const hit_record& LHS, const hit_record& RHS)
	{
		if(LHS.empty())
			return false;

		return LHS.zmin() < RHS.zmin();
	}

private:
	GLuint* const m_storage;
};

/// Input iterator that extracts objects of type hit_record from a flat buffer
class hit_iterator
{
public:
	hit_iterator() :
		m_current(0),
		m_remaining(0)
	{
	}

	hit_iterator(gl_selection_buffer_t& Buffer, const unsigned int HitCount) :
		m_current(HitCount ? &Buffer[0] : 0),
		m_remaining(HitCount)
	{
	}

	hit_record operator*() const
	{
		return hit_record(m_current);
	}

	hit_record operator->() const
	{
		return hit_record(m_current);
	}

	hit_iterator& operator++()
	{
		if(m_remaining)
		{
			if(0 == --m_remaining)
				m_current = 0;
			else
				m_current += (3 + hit_record(m_current).size());
		}

		return *this;
	}

	hit_iterator operator++(int)
	{
		hit_iterator temp(*this);
		this->operator++();

		return temp;
	}

	friend bool operator == (const hit_iterator& LHS, const hit_iterator& RHS)
	{
		return LHS.m_current == RHS.m_current;
	}

	friend bool operator != (const hit_iterator& LHS, const hit_iterator& RHS)
	{
		return !(LHS == RHS);
	}

private:
	GLuint* m_current;
	unsigned int m_remaining;
};

/// Convenience function used to choose whichever point is closest to the given window coordinates
void select_nearest_point(const k3d::mesh::points_t& Points, const k3d::selection::id Point, const k3d::point2& Coordinates, const double ScreenHeight, const GLdouble ModelViewMatrix[16], const GLdouble ProjectionMatrix[16], const GLint Viewport[4], k3d::selection::id& OutputPoint, double& OutputDistance)
{
	k3d::point2 coords;
	double unused;
	gluProject(
		Points[Point][0],
		Points[Point][1],
		Points[Point][2],
		ModelViewMatrix,
		ProjectionMatrix,
		Viewport,
		&coords[0],
		&coords[1],
		&unused);
	coords[1] = ScreenHeight - coords[1];

	const double distance = k3d::to_vector(coords - Coordinates).length2();
	if(distance < OutputDistance)
	{
		OutputPoint = Point;
		OutputDistance = distance;
	}
}

/// Convenience function used to choose whichever edge is closest to the given window coordinates
void select_nearest_edge(const k3d::mesh::indices_t& EdgePoints, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::points_t& Points, const k3d::selection::id Edge, const k3d::point2& Coordinates, const double ScreenHeight, const GLdouble ModelViewMatrix[16], const GLdouble ProjectionMatrix[16], const GLint Viewport[4], k3d::selection::id& OutputEdge, double& OutputDistance)
{
	double x1, y1, x2, y2;
	double unused;

	// First edge end : S1
	gluProject(
		Points[EdgePoints[Edge]][0],
		Points[EdgePoints[Edge]][1],
		Points[EdgePoints[Edge]][2],
		ModelViewMatrix,
		ProjectionMatrix,
		Viewport,
		&x1,
		&y1,
		&unused);

	const k3d::point2 S1(x1, ScreenHeight - y1);

	// Second edge end : S2
	gluProject(
		Points[EdgePoints[ClockwiseEdges[Edge]]][0],
		Points[EdgePoints[ClockwiseEdges[Edge]]][1],
		Points[EdgePoints[ClockwiseEdges[Edge]]][2],
		ModelViewMatrix,
		ProjectionMatrix,
		Viewport,
		&x2,
		&y2,
		&unused);

	const k3d::point2 S2(x2, ScreenHeight - y2);

	// Coordinates to segment distance
	double distance = 0;

	const k3d::point2 edge = S2 - S1;
	const k3d::point2 w = Coordinates - S1;

	const double c1 = w * edge;
	if(c1 <= 0)
		distance = k3d::distance(Coordinates, S1);
	else
	{
		const double c2 = edge * edge;
		if(c2 <= c1)
			distance = k3d::distance(Coordinates, S2);
		else
		{
			const double b = c1 / c2;
			const k3d::point2 middlepoint = S1 + b * edge;
			distance = k3d::distance(Coordinates, middlepoint);
		}
	}

	if(distance < OutputDistance)
	{
		OutputEdge = Edge;
		OutputDistance = distance;
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation
{
public:
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_camera(init_value<k3d::icamera*>(0)),
		m_gl_engine(init_value<k3d::gl::irender_engine*>(0)),
		m_preview_engine(init_value<k3d::icamera_preview_render_engine*>(0)),
		m_still_engine(init_value<k3d::icamera_still_render_engine*>(0)),
		m_animation_engine(init_value<k3d::icamera_animation_render_engine*>(0)),
		m_font_begin(0),
		m_font_end(0),
		m_glew_context(0)
	{
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores a reference to the current camera
	k3d_data(k3d::icamera*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_camera;
	/// Stores a reference to the current OpenGL render engine
	k3d_data(k3d::gl::irender_engine*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_gl_engine;
	/// Stores a reference to the current preview render engine
	k3d_data(k3d::icamera_preview_render_engine*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_preview_engine;
	/// Stores a reference to the current still render engine
	k3d_data(k3d::icamera_still_render_engine*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_still_engine;
	/// Stores a reference to the current animation render engine
	k3d_data(k3d::icamera_animation_render_engine*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_animation_engine;

	/// Stores the current set of OpenGL font glyphs (generated from Pango by gtkglext)
	unsigned long m_font_begin;
	/// Stores the current set of OpenGL font glyphs (generated from Pango by gtkglext)
	unsigned long m_font_end;

	/// Stores a connection to the attached OpenGL render engine redraw request signal
	sigc::connection m_gl_engine_redraw_request_connection;

	/// Buffers OpenGL hit records
	typedef std::vector<GLuint> gl_selection_buffer_t;
	gl_selection_buffer_t m_selection_buffer;

	// Buffers parameters from the most-recent render
	GLdouble m_gl_view_matrix[16];
	GLdouble m_gl_projection_matrix[16];
	GLint m_gl_viewport[4];

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;
	
	/// Keep track of glew initialisation
	GLEWContext* m_glew_context;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(document_state& DocumentState, k3d::icommand_node& Parent) :
	ui_component("viewport", &Parent),
	m_implementation(new implementation(DocumentState))
{
	m_implementation->m_camera.changed_signal().connect(sigc::mem_fun(*this, &control::on_camera_changed));
	m_implementation->m_gl_engine.changed_signal().connect(sigc::mem_fun(*this, &control::on_gl_engine_changed));

	set_flags(Gtk::CAN_FOCUS);
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::KEY_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);

	signal_button_press_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);

	signal_expose_event().connect(sigc::hide(sigc::mem_fun(*this, &control::on_redraw)));
	set_double_buffered(false);

	GdkGLConfig* const config = gdk_gl_config_new_by_mode(
		static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
	return_if_fail(config);

	return_if_fail(
		gtk_widget_set_gl_capability(GTK_WIDGET(gobj()), config, m_implementation->m_document_state.gdkgl_share_list(), true, GDK_GL_RGBA_TYPE));

	show_all();
}

sigc::connection control::connect_focus_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->m_panel_grab_signal.connect(Slot);
}

k3d::idocument& control::document()
{
	return m_implementation->m_document_state.document();
}

k3d::icamera* const control::camera()
{
	return m_implementation->m_camera.value();
}

k3d::gl::irender_engine* const control::gl_engine()
{
	return m_implementation->m_gl_engine.value();
}

k3d::icamera_preview_render_engine* const control::camera_preview_engine()
{
	return m_implementation->m_preview_engine.value();
}

k3d::icamera_still_render_engine* const control::camera_still_engine()
{
	return m_implementation->m_still_engine.value();
}

k3d::icamera_animation_render_engine* const control::camera_animation_engine()
{
	return m_implementation->m_animation_engine.value();
}

void control::set_camera(k3d::icamera* const Camera)
{
	m_implementation->m_camera.set_value(Camera);
}

void control::set_gl_engine(k3d::gl::irender_engine* const Engine)
{
	m_implementation->m_gl_engine_redraw_request_connection.disconnect();
	if(Engine)
		m_implementation->m_gl_engine_redraw_request_connection = Engine->redraw_request_signal().connect(sigc::mem_fun(*this, &control::on_redraw_request));

	m_implementation->m_gl_engine.set_value(Engine);

	on_redraw_request(k3d::gl::irender_engine::SYNCHRONOUS);
}

void control::set_camera_preview_engine(k3d::icamera_preview_render_engine* const Engine)
{
	m_implementation->m_preview_engine.set_value(Engine);
}

void control::set_camera_still_engine(k3d::icamera_still_render_engine* const Engine)
{
	m_implementation->m_still_engine.set_value(Engine);
}

void control::set_camera_animation_engine(k3d::icamera_animation_render_engine* const Engine)
{
	m_implementation->m_animation_engine.set_value(Engine);
}

const k3d::matrix4 control::get_view_matrix()
{
	return_val_if_fail(camera(), k3d::identity3D());
	return k3d::node_to_world_matrix(camera()->navigation_target());
}

void control::set_view_matrix(const k3d::matrix4& Matrix)
{
	return_if_fail(camera());
	k3d::set_matrix(camera()->navigation_target(), Matrix);
}

const k3d::point3 control::get_target()
{
	return boost::any_cast<k3d::point3>(camera()->world_target().property_value());
}

void control::set_target(const k3d::point3& Target)
{
	k3d::set_value(*camera(), "world_target", Target);
}

const k3d::vector3 control::get_up_axis()
{
	return k3d::vector3(0, 0, 1);
}

void control::get_gl_viewport(GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4])
{
	std::copy(m_implementation->m_gl_view_matrix, m_implementation->m_gl_view_matrix + 16, ViewMatrix);
	std::copy(m_implementation->m_gl_projection_matrix, m_implementation->m_gl_projection_matrix + 16, ProjectionMatrix);
	std::copy(m_implementation->m_gl_viewport, m_implementation->m_gl_viewport + 4, Viewport);
}

const k3d::point2 control::project(const k3d::point3& WorldCoords)
{
	k3d::point2 coords;
	double unused;

	gluProject(
		WorldCoords[0],
		WorldCoords[1],
		WorldCoords[2],
		m_implementation->m_gl_view_matrix,
		m_implementation->m_gl_projection_matrix,
		m_implementation->m_gl_viewport,
		&coords[0],
		&coords[1],
		&unused);
	coords[1] = get_height() - coords[1];

	return coords;
}

bool control::render_camera_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const bool ViewCompletedImage)
{
	return save_frame(Camera, OutputImage, ViewCompletedImage);
}

bool control::render_camera_animation(k3d::icamera& Camera, const k3d::file_range& Files, const bool ViewCompletedImages)
{
	// Ensure that the document has animation capabilities, first ...
	k3d::iproperty* const start_time_property = k3d::get_start_time(m_implementation->m_document_state.document());
	k3d::iproperty* const end_time_property = k3d::get_end_time(m_implementation->m_document_state.document());
	k3d::iproperty* const frame_rate_property = k3d::get_frame_rate(m_implementation->m_document_state.document());
	k3d::iwritable_property* const time_property = dynamic_cast<k3d::iwritable_property*>(k3d::get_time(m_implementation->m_document_state.document()));
	return_val_if_fail(start_time_property && end_time_property && frame_rate_property && time_property, false);

	// Test the output images filepath to make sure it can hold all the frames we're going to generate ...
	const double start_time = boost::any_cast<double>(k3d::get_value(m_implementation->m_document_state.document().dag(), *start_time_property));
	const double end_time = boost::any_cast<double>(k3d::get_value(m_implementation->m_document_state.document().dag(), *end_time_property));
	const double frame_rate = boost::any_cast<double>(k3d::get_value(m_implementation->m_document_state.document().dag(), *frame_rate_property));

	const size_t start_frame = static_cast<size_t>(k3d::round(frame_rate * start_time));
	const size_t end_frame = static_cast<size_t>(k3d::round(frame_rate * end_time));

	return_val_if_fail(Files.max_file_count() > end_frame, false);

	// For each frame to be rendered ...
	for(size_t view_frame = start_frame; view_frame < end_frame; ++view_frame)
	{
		// Set the frame time ...
		time_property->property_set_value(view_frame / frame_rate);

		// Save that baby ...
		const k3d::filesystem::path destination = Files.file(view_frame);
		return_val_if_fail(save_frame(Camera, destination, ViewCompletedImages), false);
	}

	return true;
}

bool control::save_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const bool ViewCompletedImage)
{
	// Draw the image as we normally would ...
	const unsigned long width = get_width();
	const unsigned long height = get_height();
	return_val_if_fail(width && height, false);

	GdkGLDrawable* const drawable = gtk_widget_get_gl_drawable(GTK_WIDGET(gobj()));
	return_val_if_fail(drawable, true);

	GdkGLContext* const context = gtk_widget_get_gl_context(GTK_WIDGET(gobj()));
	return_val_if_fail(context, true);

	return_val_if_fail(gdk_gl_drawable_gl_begin(drawable, context), true);

	create_font();
	glViewport(0, 0, width, height);
	if(m_implementation->m_gl_engine.value())
	{
		m_implementation->m_gl_engine.value()->redraw(Camera, width, height, m_implementation->m_font_begin, m_implementation->m_gl_view_matrix, m_implementation->m_gl_projection_matrix, m_implementation->m_gl_viewport);
	}
	else
	{
		glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	glFlush();

	// Get the rendered image ...
	std::vector<unsigned char> image_buffer(width * height * 3, 0);
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
//	glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
	glPixelZoom(1.0, -1.0);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &image_buffer[0]);

	if(gdk_gl_drawable_is_double_buffered(drawable))
		gdk_gl_drawable_swap_buffers(drawable);

	gdk_gl_drawable_gl_end(drawable);

	// Save that bad-boy ...
	k3d::filesystem::ofstream stream(OutputImage);

	stream << "P6" << std::endl;
	stream << width << " " << height << std::endl;
	stream << "255" << std::endl;

	// Write data ...
	for(unsigned long y = 0; y != height; ++y)
		std::copy(&image_buffer[(height - 1 - y) * width * 3], &image_buffer[(height - y) * width * 3], std::ostreambuf_iterator<char>(stream));

	return true;
}

void control::on_camera_changed(k3d::iunknown*)
{
	on_redraw_request(k3d::gl::irender_engine::SYNCHRONOUS);
}

void control::on_gl_engine_changed(k3d::iunknown*)
{
	on_redraw_request(k3d::gl::irender_engine::SYNCHRONOUS);
}

void control::on_redraw_request(k3d::gl::irender_engine::redraw_type_t RedrawType)
{
	if(k3d::gl::irender_engine::ASYNCHRONOUS == RedrawType)
	{
		queue_draw();
		return;
	}

	on_redraw();
}

void control::on_style_changed(const Glib::RefPtr<Gtk::Style>& previous_style)
{
	base::on_style_changed(previous_style);
	if (m_implementation->m_font_begin != m_implementation->m_font_end)
		glDeleteLists(m_implementation->m_font_begin, m_implementation->m_font_end - m_implementation->m_font_begin);
}

void control::create_font()
{
	if(m_implementation->m_font_begin != m_implementation->m_font_end)
		return;

	const unsigned long begin_glyph = 0;
	const unsigned long end_glyph = 256;
	m_implementation->m_font_begin = glGenLists(end_glyph - begin_glyph);
	return_if_fail(m_implementation->m_font_begin);
	m_implementation->m_font_end = m_implementation->m_font_begin + end_glyph - begin_glyph;

	// Initialize font
	return_if_fail(get_pango_context());
	const Pango::FontDescription& font_description = get_pango_context()->get_font_description();
	return_if_fail(get_pango_context()->get_font_description().gobj());
	Glib::RefPtr<Pango::Font> font = Glib::wrap(gdk_gl_font_use_pango_font(font_description.gobj(), begin_glyph, end_glyph, m_implementation->m_font_begin));

	if(font)
		font->reference();
}

bool control::on_button_press_event(GdkEventButton* Event)
{
	m_implementation->m_document_state.active_tool().input_model().button_press_event(*this, Event);
	return true;
}

bool control::on_button_release_event(GdkEventButton* Event)
{
	m_implementation->m_document_state.active_tool().input_model().button_release_event(*this, Event);
	return true;
}

bool control::on_motion_notify_event(GdkEventMotion* Event)
{
	m_implementation->m_document_state.active_tool().input_model().motion_notify_event(*this, Event);
	return true;
}

bool control::on_scroll_event(GdkEventScroll* Event)
{
	m_implementation->m_document_state.active_tool().input_model().scroll_event(*this, Event);
	return true;
}

bool control::on_key_press_event(GdkEventKey* Event)
{
	m_implementation->m_document_state.active_tool().input_model().key_press_event(*this, Event);
	return true;
}

bool control::on_key_release_event(GdkEventKey* Event)
{
	m_implementation->m_document_state.active_tool().input_model().key_release_event(*this, Event);
	return true;
}

k3d::selection::records control::get_selectable_points(const k3d::rectangle& SelectionRegion)
{
	return get_selection(detail::select_points(), SelectionRegion);
}

k3d::selection::records control::get_selectable_lines(const k3d::rectangle& SelectionRegion)
{
	return get_selection(detail::select_lines(), SelectionRegion);
}

k3d::selection::records control::get_selectable_faces(const k3d::rectangle& SelectionRegion)
{
	return get_selection(detail::select_faces(), SelectionRegion);
}

k3d::selection::records control::get_selectable_nodes(const k3d::rectangle& SelectionRegion)
{
	return get_selection(detail::select_nodes(), SelectionRegion);
}

k3d::selection::records control::get_selectable_objects(const k3d::rectangle& SelectionRegion)
{
	switch(m_implementation->m_document_state.selection_mode().value())
	{
		case SELECT_NODES:
			return get_selectable_nodes(SelectionRegion);
			break;
		case SELECT_POINTS:
			return get_selectable_points(SelectionRegion);
			break;
		case SELECT_LINES:
			return get_selectable_lines(SelectionRegion);
			break;
		case SELECT_FACES:
			return get_selectable_faces(SelectionRegion);
			break;
	}

	assert_not_reached();
	return k3d::selection::records();
}

k3d::selection::record control::pick_point(const k3d::point2& Coordinates)
{
	k3d::selection::records records;
	return pick_point(Coordinates, records);
}

k3d::selection::record control::pick_line(const k3d::point2& Coordinates)
{
	k3d::selection::records records;
	return pick_line(Coordinates, records);
}

k3d::selection::record control::pick_face(const k3d::point2& Coordinates)
{
	k3d::selection::records records;
	return pick_face(Coordinates, records);
}

k3d::selection::record control::pick_node(const k3d::point2& Coordinates)
{
	k3d::selection::records records;
	return pick_node(Coordinates, records);
}

k3d::selection::record control::pick_object(const k3d::point2& Coordinates)
{
	k3d::selection::records records;
	return pick_object(Coordinates, records);
}

k3d::selection::record control::pick_point(const k3d::point2& Coordinates, k3d::selection::records& Records)
{
	// Draw everything (will find nearest point if another component is picked)
	k3d::gl::selection_state selection_state;
	selection_state.exclude_unselected_nodes = true;
	selection_state.select_points = true;
	selection_state.select_point_groups = true;
	selection_state.select_edges = true;
	selection_state.select_faces = true;
	selection_state.select_linear_curves = true;
	selection_state.select_cubic_curves = true;
	selection_state.select_nurbs_curves = true;
	selection_state.select_bilinear_patches = true;
	selection_state.select_bicubic_patches = true;
	selection_state.select_nurbs_patches = true;

	const double sensitivity = 5;
	const k3d::rectangle selection_region(
		Coordinates[0] - sensitivity,
		Coordinates[0] + sensitivity,
		Coordinates[1] - sensitivity,
		Coordinates[1] + sensitivity);

	GLdouble view_matrix[16];
	GLdouble projection_matrix[16];
	GLint viewport[4];

	Records = get_selection(selection_state, selection_region, view_matrix, projection_matrix, viewport);
	std::sort(Records.begin(), Records.end(), detail::sort_by_zmin());
	if(Records.empty())
		return k3d::selection::record::empty_record();

	const k3d::selection::record& record = Records.front();

k3d::log() << debug << K3D_CHANGE_SET_CONTEXT << " " << record << std::endl;

	k3d::inode* const node = k3d::selection::get_node(record);
	if(!node)
		return k3d::selection::record::empty_record();

	k3d::mesh* const mesh = k3d::selection::get_mesh(record);
	if(!mesh)
		return k3d::selection::record::empty_record();
	if(!mesh->points)
		return k3d::selection::record::empty_record();

	GLdouble model_view_matrix[16];
	k3d::transpose(k3d::gl::matrix(view_matrix) * k3d::node_to_world_matrix(*node)).CopyArray(model_view_matrix);

	std::map<k3d::selection::type, k3d::selection::id> tokens;
	for(k3d::selection::record::tokens_t::const_iterator token = record.tokens.begin(); token != record.tokens.end(); ++token)
		tokens.insert(std::make_pair(token->type, token->id));

	if(tokens.count(k3d::selection::ABSOLUTE_POINT))
	{
		return record;
	}

	double distance = std::numeric_limits<double>::max();
	k3d::selection::id selected_point;

	if(tokens.count(k3d::selection::ABSOLUTE_SPLIT_EDGE))
	{
		if(mesh->polyhedra && mesh->polyhedra->edge_points && mesh->polyhedra->clockwise_edges)
		{
			const k3d::selection::id edge = tokens[k3d::selection::ABSOLUTE_SPLIT_EDGE];

			detail::select_nearest_point(
				*mesh->points,
				(*mesh->polyhedra->edge_points)[edge],
				Coordinates,
				get_height(),
				model_view_matrix,
				projection_matrix,
				viewport,
				selected_point,
				distance);

			detail::select_nearest_point(
				*mesh->points,
				(*mesh->polyhedra->edge_points)[(*mesh->polyhedra->clockwise_edges)[edge]],
				Coordinates,
				get_height(),
				model_view_matrix,
				projection_matrix,
				viewport,
				selected_point,
				distance);
		}
	}
	else if(tokens.count(k3d::selection::ABSOLUTE_FACE))
	{
		if(mesh->polyhedra && mesh->polyhedra->face_first_loops && mesh->polyhedra->face_loop_counts && mesh->polyhedra->loop_first_edges && mesh->polyhedra->edge_points && mesh->polyhedra->clockwise_edges)
		{
			const k3d::selection::id face = tokens[k3d::selection::ABSOLUTE_FACE];

			const size_t face_loop_begin = (*mesh->polyhedra->face_first_loops)[face];
			const size_t face_loop_end = face_loop_begin + (*mesh->polyhedra->face_loop_counts)[face];
			for(size_t face_loop = face_loop_begin; face_loop != face_loop_end; ++face_loop)
			{
				const size_t first_edge = (*mesh->polyhedra->loop_first_edges)[face_loop];
				for(size_t edge = first_edge; ; )
				{
					detail::select_nearest_point(
						*mesh->points,
						(*mesh->polyhedra->edge_points)[edge],
						Coordinates,
						get_height(),
						model_view_matrix,
						projection_matrix,
						viewport,
						selected_point,
						distance);

					edge = (*mesh->polyhedra->clockwise_edges)[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	}
	else if(tokens.count(k3d::selection::ABSOLUTE_LINEAR_CURVE))
	{
		if(mesh->linear_curve_groups && mesh->linear_curve_groups->curve_first_points && mesh->linear_curve_groups->curve_point_counts && mesh->linear_curve_groups->curve_points)
		{
			const k3d::selection::id curve = tokens[k3d::selection::ABSOLUTE_LINEAR_CURVE];

			const size_t curve_points_begin = (*mesh->linear_curve_groups->curve_first_points)[curve];
			const size_t curve_points_end = curve_points_begin + (*mesh->linear_curve_groups->curve_point_counts)[curve];
			for(size_t curve_point = curve_points_begin; curve_point != curve_points_end; ++curve_point)
			{
				detail::select_nearest_point(
					*mesh->points,
					(*mesh->linear_curve_groups->curve_points)[curve_point],
					Coordinates,
					get_height(),
					model_view_matrix,
					projection_matrix,
					viewport,
					selected_point,
					distance);
			}
		}
	}
	else if(tokens.count(k3d::selection::ABSOLUTE_CUBIC_CURVE))
	{
		if(mesh->cubic_curve_groups && mesh->cubic_curve_groups->curve_first_points && mesh->cubic_curve_groups->curve_point_counts && mesh->cubic_curve_groups->curve_points)
		{
			const k3d::selection::id curve = tokens[k3d::selection::ABSOLUTE_CUBIC_CURVE];

			const size_t curve_points_begin = (*mesh->cubic_curve_groups->curve_first_points)[curve];
			const size_t curve_points_end = curve_points_begin + (*mesh->cubic_curve_groups->curve_point_counts)[curve];
			for(size_t curve_point = curve_points_begin; curve_point != curve_points_end; ++curve_point)
			{
				detail::select_nearest_point(
					*mesh->points,
					(*mesh->cubic_curve_groups->curve_points)[curve_point],
					Coordinates,
					get_height(),
					model_view_matrix,
					projection_matrix,
					viewport,
					selected_point,
					distance);
			}
		}
	}

	if(distance < std::numeric_limits<double>::max())
	{
		k3d::selection::record record = k3d::selection::record::empty_record();
		record.tokens.push_back(k3d::selection::token(NODE, tokens[NODE]));
		record.tokens.push_back(k3d::selection::token(MESH, tokens[MESH]));
		record.tokens.push_back(k3d::selection::token(ABSOLUTE_POINT, selected_point));
		return record;
	}

	assert_not_implemented();
/*
	else if(tokens.count(NUCURVE_GROUP) && tokens.count(NUCURVE))
	{
		k3d::legacy::nucurve* const curve = mesh->nucurve_groups[tokens[NUCURVE_GROUP]]->curves[tokens[NUCURVE]];
		for(k3d::legacy::nucurve::control_points_t::const_iterator control_point = curve->control_points.begin(); control_point != curve->control_points.end(); ++control_point)
			detail::select_nearest_point(control_point->position, Coordinates, get_height(), model_view_matrix, projection_matrix, viewport, point, distance);
	}
	else if(tokens.count(BILINEAR_PATCH))
	{
		k3d::legacy::bilinear_patch* const patch = mesh->bilinear_patches[tokens[BILINEAR_PATCH]];
		for(k3d::legacy::bilinear_patch::control_points_t::const_iterator control_point = patch->control_points.begin(); control_point != patch->control_points.end(); ++control_point)
			detail::select_nearest_point(*control_point, Coordinates, get_height(), model_view_matrix, projection_matrix, viewport, point, distance);
	}
	else if(tokens.count(BICUBIC_PATCH))
	{
		k3d::legacy::bicubic_patch* const patch = mesh->bicubic_patches[tokens[BICUBIC_PATCH]];
		for(k3d::legacy::bicubic_patch::control_points_t::const_iterator control_point = patch->control_points.begin(); control_point != patch->control_points.end(); ++control_point)
			detail::select_nearest_point(*control_point, Coordinates, get_height(), model_view_matrix, projection_matrix, viewport, point, distance);
	}
	else if(tokens.count(NUPATCH))
	{
		k3d::legacy::nupatch* const patch = mesh->nupatches[tokens[NUPATCH]];
		for(k3d::legacy::nupatch::control_points_t::const_iterator control_point = patch->control_points.begin(); control_point != patch->control_points.end(); ++control_point)
			detail::select_nearest_point(control_point->position, Coordinates, get_height(), model_view_matrix, projection_matrix, viewport, point, distance);
	}
*/

	return k3d::selection::record::empty_record();
}

k3d::selection::record control::pick_line(const k3d::point2& Coordinates, k3d::selection::records& Records)
{
	// Draw everything (will find nearest line if some other component type is picked)
	k3d::gl::selection_state selection_state;
	selection_state.exclude_unselected_nodes = true;
	selection_state.select_edges = true;
	selection_state.select_faces = true;
	selection_state.select_linear_curves = true;
	selection_state.select_cubic_curves = true;
	selection_state.select_nurbs_curves = true;

	const double sensitivity = 5;
	const k3d::rectangle selection_region(
		Coordinates[0] - sensitivity,
		Coordinates[0] + sensitivity,
		Coordinates[1] - sensitivity,
		Coordinates[1] + sensitivity);

	GLdouble view_matrix[16];
	GLdouble projection_matrix[16];
	GLint viewport[4];

	Records = get_selection(selection_state, selection_region, view_matrix, projection_matrix, viewport);
	std::sort(Records.begin(), Records.end(), detail::sort_by_zmin());
	if(Records.empty())
		return k3d::selection::record::empty_record();

	const k3d::selection::record& record = Records.front();

	k3d::inode* const node = k3d::selection::get_node(record);
	if(!node)
		return k3d::selection::record::empty_record();

	k3d::mesh* const mesh = k3d::selection::get_mesh(record);
	if(!mesh)
		return k3d::selection::record::empty_record();

	GLdouble model_view_matrix[16];
	k3d::transpose(k3d::gl::matrix(view_matrix) * k3d::node_to_world_matrix(*node)).CopyArray(model_view_matrix);

	std::map<k3d::selection::type, k3d::selection::id> tokens;
	for(k3d::selection::record::tokens_t::const_iterator token = record.tokens.begin(); token != record.tokens.end(); ++token)
		tokens.insert(std::make_pair(token->type, token->id));

	if(tokens.count(k3d::selection::ABSOLUTE_SPLIT_EDGE))
	{
		return record;
	}
	else if(tokens.count(k3d::selection::ABSOLUTE_LINEAR_CURVE))
	{
		return record;
	}
	else if(tokens.count(k3d::selection::ABSOLUTE_CUBIC_CURVE))
	{
		return record;
	}
	else if(tokens.count(k3d::selection::ABSOLUTE_NURBS_CURVE))
	{
		return record;
	}


	double distance = std::numeric_limits<double>::max();
	k3d::selection::id selected_edge;

	if(tokens.count(k3d::selection::ABSOLUTE_FACE))
	{
		if(mesh->polyhedra && mesh->polyhedra->face_first_loops && mesh->polyhedra->face_loop_counts && mesh->polyhedra->loop_first_edges && mesh->polyhedra->edge_points && mesh->polyhedra->clockwise_edges)
		{
			const k3d::selection::id face = tokens[k3d::selection::ABSOLUTE_FACE];

			const size_t face_loop_begin = (*mesh->polyhedra->face_first_loops)[face];
			const size_t face_loop_end = face_loop_begin + (*mesh->polyhedra->face_loop_counts)[face];
			for(size_t face_loop = face_loop_begin; face_loop != face_loop_end; ++face_loop)
			{
				const size_t first_edge = (*mesh->polyhedra->loop_first_edges)[face_loop];
				for(size_t edge = first_edge; ; )
				{
					detail::select_nearest_edge(
						*mesh->polyhedra->edge_points,
						*mesh->polyhedra->clockwise_edges,
						*mesh->points,
						edge,
						Coordinates,
						get_height(),
						model_view_matrix,
						projection_matrix,
						viewport,
						selected_edge,
						distance);

					edge = (*mesh->polyhedra->clockwise_edges)[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	}

	if(distance < std::numeric_limits<double>::max())
	{
		k3d::selection::record record = k3d::selection::record::empty_record();
		record.tokens.push_back(k3d::selection::token(k3d::selection::NODE, tokens[k3d::selection::NODE]));
		record.tokens.push_back(k3d::selection::token(k3d::selection::MESH, tokens[k3d::selection::MESH]));
		record.tokens.push_back(k3d::selection::token(k3d::selection::ABSOLUTE_SPLIT_EDGE, selected_edge));
		return record;
	}

	return k3d::selection::record::empty_record();
}

/// Retrieves the face (if any) under the given widget coordinates
k3d::selection::record control::pick_face(const k3d::point2& Coordinates, k3d::selection::records& Records)
{
	const double sensitivity = 3;
	const k3d::rectangle selection_region(
		Coordinates[0] - sensitivity,
		Coordinates[0] + sensitivity,
		Coordinates[1] - sensitivity,
		Coordinates[1] + sensitivity);

	Records = get_selection(detail::select_faces(), selection_region);
	std::sort(Records.begin(), Records.end(), detail::sort_by_zmin());

	for(k3d::selection::records::const_iterator record = Records.begin(); record != Records.end(); ++record)
	{
		for(k3d::selection::record::tokens_t::const_iterator token = record->tokens.begin(); token != record->tokens.end(); ++token)
		{
			if(token->type == k3d::selection::ABSOLUTE_FACE || token->type == k3d::selection::ABSOLUTE_BILINEAR_PATCH || token->type == k3d::selection::ABSOLUTE_BICUBIC_PATCH || token->type == k3d::selection::ABSOLUTE_NURBS_PATCH)
				return *record;
		}
	}

	return k3d::selection::record::empty_record();
}

k3d::selection::record control::pick_node(const k3d::point2& Coordinates, k3d::selection::records& Records)
{
	const double sensitivity = 3;

	const k3d::rectangle selection_region(
		Coordinates[0] - sensitivity,
		Coordinates[0] + sensitivity,
		Coordinates[1] - sensitivity,
		Coordinates[1] + sensitivity);

	Records = get_selection(detail::select_nodes(), selection_region);
	std::sort(Records.begin(), Records.end(), detail::sort_by_zmin());

	for(k3d::selection::records::iterator record = Records.begin(); record != Records.end(); ++record)
	{
		if(record->tokens.size() && record->tokens[0].type == k3d::selection::NODE)
			return *record;
	}

	return k3d::selection::record::empty_record();
}

k3d::selection::record control::pick_object(const k3d::point2& Coordinates, k3d::selection::records& Records)
{
	switch(m_implementation->m_document_state.selection_mode().value())
	{
		case SELECT_NODES:
			return pick_node(Coordinates, Records);
			break;
		case SELECT_POINTS:
			return pick_point(Coordinates, Records);
			break;
		case SELECT_LINES:
			return pick_line(Coordinates, Records);
			break;
		case SELECT_FACES:
			return pick_face(Coordinates, Records);
			break;
	}

	return k3d::selection::record::empty_record();
}

const k3d::selection::records control::get_selection(const k3d::gl::selection_state& SelectionState, const k3d::rectangle& SelectionRegion)
{
	GLdouble view_matrix[16];
	GLdouble projection_matrix[16];
	GLint viewport[4];

	return get_selection(SelectionState, SelectionRegion, view_matrix, projection_matrix, viewport);
}

const k3d::selection::records control::get_selection(const k3d::gl::selection_state& SelectionState, const k3d::rectangle& SelectionRegion, GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4])
{
	k3d::selection::records selection;

	const unsigned int hit_count = select(SelectionState, SelectionRegion, ViewMatrix, ProjectionMatrix, Viewport);

	for(detail::hit_iterator hit(m_implementation->m_selection_buffer, hit_count); hit != detail::hit_iterator(); ++hit)
	{
		k3d::selection::record record;
		record.zmin = (*hit).zmin();
		record.zmax = (*hit).zmax();

		for(detail::hit_record::const_name_iterator name = (*hit).name_begin(); name != (*hit).name_end(); )
		{
			k3d::selection::type type = k3d::selection::type(*name++);
			k3d::selection::id id = k3d::selection::id(*name++);

			record.tokens.push_back(k3d::selection::token(type, id));
		}

		selection.push_back(record);
	}

	return selection;
}

bool control::on_redraw()
{
	// If we're minimized, we're done ...
	const unsigned long width = get_width();
	const unsigned long height = get_height();
	if(!width || !height)
		return true;

	if(!is_realized())
		return true;

	GdkGLDrawable* const drawable = gtk_widget_get_gl_drawable(GTK_WIDGET(gobj()));
	return_val_if_fail(drawable, true);

	GdkGLContext* const context = gtk_widget_get_gl_context(GTK_WIDGET(gobj()));
	return_val_if_fail(context, true);

	return_val_if_fail(gdk_gl_drawable_gl_begin(drawable, context), true);
		
	if (!m_implementation->m_glew_context)
	{
		m_implementation->m_glew_context = new GLEWContext();
		glew_context::instance().set_context(m_implementation->m_glew_context);
		GLenum err = glewInit(); // needs to be called after context creation
		if (GLEW_OK != err)
		{
  		k3d::log() << error << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
  		assert_not_reached();
		}
		k3d::log() << debug << "glew init successful" << std::endl;
		if (GLEW_VERSION_1_5)
			k3d::log() << debug << "OpenGL 1.5 is supported!" << std::endl;
	}
	
	glew_context::instance().set_context(m_implementation->m_glew_context);
	
	create_font();
	glViewport(0, 0, width, height);
	if(m_implementation->m_gl_engine.value() && m_implementation->m_camera.value())
	{
		k3d::timer timer;

		m_implementation->m_gl_engine.value()->redraw(*m_implementation->m_camera.value(), width, height, m_implementation->m_font_begin, m_implementation->m_gl_view_matrix, m_implementation->m_gl_projection_matrix, m_implementation->m_gl_viewport);
		if(m_implementation->m_document_state.get_focus_viewport() == this)
			m_implementation->m_document_state.active_tool().redraw(*this);

		const double elapsed = timer.elapsed();
		if(elapsed)
		{
			std::stringstream buffer;
			buffer << std::fixed << std::setprecision(1) << 1.0 / elapsed << "fps";

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1, 1, -1, 1, -1, 1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_1D);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);

			glColor3d(0, 0, 0);

			glRasterPos3d(-0.95, -0.95, 0);
			glListBase(m_implementation->m_font_begin);
			glCallLists(buffer.str().size(), GL_UNSIGNED_BYTE, buffer.str().data());
		}
	}
	else
	{
		glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glColor3d(0, 0, 0);

		glRasterPos3d(-0.95, -0.95, 0);
		const std::string buffer(_("Unattached"));
		glListBase(m_implementation->m_font_begin);
		glCallLists(buffer.size(), GL_UNSIGNED_BYTE, buffer.data());
	}
	glFlush();

	if(gdk_gl_drawable_is_double_buffered(drawable))
		gdk_gl_drawable_swap_buffers(drawable);

	gdk_gl_drawable_gl_end(drawable);

	return true;
}

const GLint control::select(const k3d::gl::selection_state& SelectState, const k3d::rectangle& SelectionRegion)
{
	GLdouble view_matrix[16];
	GLdouble projection_matrix[16];
	GLint viewport[4];

	return select(SelectState, SelectionRegion, view_matrix, projection_matrix, viewport);
}

const GLint control::select(const k3d::gl::selection_state& SelectState, const k3d::rectangle& SelectionRegion, GLdouble ViewMatrix[16], GLdouble ProjectionMatrix[16], GLint Viewport[4])
{
	// If we don't have a camera, we're done ...
	if(!m_implementation->m_camera.value())
		return 0;

	if(!is_realized())
		return 0;

	// Viewport doesn't support selection, so we're done ...
	k3d::gl::iselection_engine* const selection_engine = dynamic_cast<k3d::gl::iselection_engine*>(m_implementation->m_gl_engine.value());
	if(!selection_engine)
		return 0;

	// If we're minimized, we're done ...
	const unsigned long width = get_width();
	const unsigned long height = get_height();
	if(!width || !height)
		return 0;

	// Set our selection buffer to a sensible minimum size ...
	if(m_implementation->m_selection_buffer.size() < 8096)
		m_implementation->m_selection_buffer.resize(8096);

	// Set an (arbitrary) upper-limit on how large we let the buffer grow ...
	while(m_implementation->m_selection_buffer.size() < 10000000)
	{
		// Draw the scene, recording hits ...
		GdkGLDrawable* const drawable = gtk_widget_get_gl_drawable(GTK_WIDGET(gobj()));
		return_val_if_fail(drawable, true);

		GdkGLContext* const context = gtk_widget_get_gl_context(GTK_WIDGET(gobj()));
		return_val_if_fail(context, true);

		return_val_if_fail(gdk_gl_drawable_gl_begin(drawable, context), true);

		create_font();
		glViewport(0, 0, width, height);

		glSelectBuffer(m_implementation->m_selection_buffer.size(), &m_implementation->m_selection_buffer[0]);
		glRenderMode(GL_SELECT);
		glInitNames();

		GLdouble projection_matrix[16];
		selection_engine->select(SelectState, *m_implementation->m_camera.value(), width, height, m_implementation->m_font_begin, k3d::normalize(SelectionRegion), m_implementation->m_gl_view_matrix, projection_matrix, m_implementation->m_gl_viewport);
		std::copy(m_implementation->m_gl_view_matrix, m_implementation->m_gl_view_matrix + 16, ViewMatrix);
		std::copy(projection_matrix, projection_matrix + 16, ProjectionMatrix);
		std::copy(m_implementation->m_gl_viewport, m_implementation->m_gl_viewport + 4, Viewport);

		if(m_implementation->m_document_state.get_focus_viewport() == this)
			m_implementation->m_document_state.active_tool().select(*this);
		const GLint hits = glRenderMode(GL_RENDER);
		glFlush();

		gdk_gl_drawable_gl_end(drawable);

		// If we got a positive number of hits, we're done ...
		if(hits >= 0)
			return hits;

		// A negative number means there was buffer overflow, so try again ...
		m_implementation->m_selection_buffer.resize(m_implementation->m_selection_buffer.size() * 2);
	}

	// Ran out of buffer space!
	k3d::log() << error << "Ran out of selection-buffer space" << std::endl;

	return 0;
}

} // namespace viewport

/////////////////////////////////////////////////////////////////////////////
// widget_to_ndc

const k3d::point2 widget_to_ndc(viewport::control& Viewport, const k3d::point2& WidgetCoords)
{
	return_val_if_fail(Viewport.gl_engine(), k3d::point2());
	return_val_if_fail(Viewport.camera(), k3d::point2());

	const unsigned long width = Viewport.get_width();
	const unsigned long height = Viewport.get_height();
	k3d::rectangle host_rect(0, 0, 0, 0);
	k3d::rectangle window_rect(0, 0, 0, 0);
	Viewport.gl_engine()->get_ndc(*Viewport.camera(), width, height, host_rect, window_rect);

	const double window_x = k3d::mix(window_rect.left, window_rect.right, WidgetCoords[0] / width);
	const double window_y = k3d::mix(window_rect.top, window_rect.bottom, WidgetCoords[1] / height);

	const double x = (window_x - host_rect.left) / (host_rect.right - host_rect.left);
	const double y = (window_y - host_rect.top) / (host_rect.bottom - host_rect.top);

	return k3d::point2(x, y);
}

/////////////////////////////////////////////////////////////////////////////
// ndc_to_widget

const k3d::point2 ndc_to_widget(viewport::control& Viewport, const k3d::point2& NDC)
{
	return_val_if_fail(Viewport.gl_engine(), k3d::point2());
	return_val_if_fail(Viewport.camera(), k3d::point2());

	const unsigned long width = Viewport.get_width();
	const unsigned long height = Viewport.get_height();
	k3d::rectangle host_rect(0, 0, 0, 0);
	k3d::rectangle window_rect(0, 0, 0, 0);
	Viewport.gl_engine()->get_ndc(*Viewport.camera(), width, height, host_rect, window_rect);

	const double host_x = k3d::mix(host_rect.left, host_rect.right, NDC[0]);
	const double host_y = k3d::mix(host_rect.top, host_rect.bottom, NDC[1]);

	const double x = (host_x - window_rect.left) / (window_rect.right - window_rect.left);
	const double y = (host_y - window_rect.top) / (window_rect.bottom - window_rect.top);

	return k3d::point2(width * x, height * y);
}

/////////////////////////////////////////////////////////////////////////////
// mouse_to_world

const k3d::line3 mouse_to_world(viewport::control& Viewport, const k3d::point2& WidgetCoords)
{
	return_val_if_fail(Viewport.gl_engine(), k3d::line3(k3d::vector3(), k3d::point3()));

	GLdouble gl_view_matrix[16];
	GLdouble gl_projection_matrix[16];
	GLint gl_viewport[4];
	Viewport.get_gl_viewport(gl_view_matrix, gl_projection_matrix, gl_viewport);

	k3d::point3 near_plane;
	gluUnProject(WidgetCoords[0], Viewport.get_height() - WidgetCoords[1], 0.0, gl_view_matrix, gl_projection_matrix, gl_viewport, &near_plane[0], &near_plane[1], &near_plane[2]);

	k3d::point3 far_plane;
	gluUnProject(WidgetCoords[0], Viewport.get_height() - WidgetCoords[1], 1.0, gl_view_matrix, gl_projection_matrix, gl_viewport, &far_plane[0], &far_plane[1], &far_plane[2]);

	return k3d::line3(to_vector(far_plane - near_plane), near_plane);
}

} // namespace libk3dngui

