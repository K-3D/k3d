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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <gdkmm/cursor.h>
#include <gtkmm/widget.h>

#include "document_state.h"
#include "modifiers.h"
#include "icons.h"
#include "keyboard.h"
#include "move_tool.h"
#include "transform_tool.h"
#include "utility.h"
#include "viewport.h"

#include <k3d-i18n-config.h>

#include <k3dsdk/color.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/geometric_operations.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/line2.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/plane.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/share.h>
#include <k3dsdk/xml.h>

namespace k3d
{

namespace ngui
{

namespace detail
{

class move_manipulators :
	public imanipulators<k3d::vector3>
{
private:
	struct constraint :
		public k3d::selectable
	{
		constraint(const std::string& Label, Glib::RefPtr<Gdk::Pixbuf> Cursor, const k3d::selection::token& SelectionToken) :
			m_selection_token(SelectionToken),
			m_label(Label),
			m_cursor(Cursor),
			m_plane(k3d::vector3(0, 0, 1), 0),
			m_x_sensitivity(0),
			m_y_sensitivity(0),
			m_z_sensitivity(0)
		{
			assert_warning(!m_label.empty());
		}

		const std::string label()
		{
			return m_label;
		}

		Glib::RefPtr<Gdk::Pixbuf> cursor()
		{
			return m_cursor;
		}

		void set_plane(const k3d::plane& Plane, const double XSensitivity, const double YSensitivity, const double ZSensitivity)
		{
			m_plane = Plane;
			m_x_sensitivity = XSensitivity;
			m_y_sensitivity = YSensitivity;
			m_z_sensitivity = ZSensitivity;
		}

		void begin_mouse_move(const k3d::point2& Mouse, const k3d::point3& Origin)
		{
			m_last_mouse = Mouse;
		}

		k3d::vector3 mouse_move(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::point3& Position, const k3d::matrix4& Orientation)
		{
			const k3d::point2 current_mouse(Coordinates);

			// Turn mouse coordinates into lines in world coordinates ...
			k3d::line3 last_line = mouse_to_world(Viewport, m_last_mouse);
			k3d::line3 current_line = mouse_to_world(Viewport, current_mouse);
			m_last_mouse = current_mouse;

			// Calculate intersections with our plane in world coordinates ...
			k3d::point3 last_intersection;
			return_val_if_fail(k3d::intersect(m_plane, last_line, last_intersection), k3d::vector3(0, 0, 0));

			k3d::point3 current_intersection;
			return_val_if_fail(k3d::intersect(m_plane, current_line, current_intersection), k3d::vector3(0, 0, 0));

			// Convert the change into world axis coordinates and constrain them (with sensitivity)
			const k3d::vector3 delta = k3d::inverse(Orientation) * (current_intersection - last_intersection);

			return k3d::vector3(delta[0] * m_x_sensitivity, delta[1] * m_y_sensitivity, delta[2] * m_z_sensitivity);
		}

		const k3d::selection::token m_selection_token;
		const std::string m_label;
		const Glib::RefPtr<Gdk::Pixbuf> m_cursor;

		/// Stores the constraint plane in world coordinates
		k3d::plane m_plane;

		double m_x_sensitivity;
		double m_y_sensitivity;
		double m_z_sensitivity;
		k3d::point2 m_last_mouse;
	};

public:
	move_manipulators() :
		m_quadric(gluNewQuadric()),
		m_current_constraint(&m_screen_xy_constraint),
		m_screen_xy_constraint(_("Move Screen XY"), load_icon("move_cursor_screen_xy", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 0)),
		m_x_constraint(_("Move X"), load_icon("move_cursor_x", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 1)),
		m_y_constraint(_("Move Y"), load_icon("move_cursor_y", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 2)),
		m_z_constraint(_("Move Z"), load_icon("move_cursor_z", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 3)),
		m_xy_constraint(_("Move XY"), load_icon("move_cursor_xy", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 4)),
		m_xz_constraint(_("Move XZ"), load_icon("move_cursor_xz", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 5)),
		m_yz_constraint(_("Move YZ"), load_icon("move_cursor_yz", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 6))
	{
	}

	~move_manipulators()
	{
		gluDeleteQuadric(m_quadric);
	}

	void activate()
	{
		// Load parameters from configuration file
		try
		{
			k3d::filesystem::ifstream layout_stream(k3d::share_path() / k3d::filesystem::generic_path("ngui/tool_layout.k3d"));
			k3d::xml::element layout_xml;
			layout_stream >> layout_xml;
			k3d::xml::element& xml_move = layout_xml.safe_element("application").safe_element("user_interface").safe_element("tools").safe_element("move");

			m_manipulators_size = k3d::xml::attribute_value<double>(xml_move, "size", 10);
			m_current_color = k3d::xml::attribute_value<k3d::color>(xml_move, "current_color", k3d::color(1, 1, 0));
			m_x_color = k3d::xml::attribute_value<k3d::color>(xml_move, "x_color", k3d::color(1, 0, 0));
			m_y_color = k3d::xml::attribute_value<k3d::color>(xml_move, "y_color", k3d::color(0, 1, 0));
			m_z_color = k3d::xml::attribute_value<k3d::color>(xml_move, "z_color", k3d::color(0, 0, 1));
			m_screen_xy_color = k3d::xml::attribute_value<k3d::color>(xml_move, "screen_xy_color", k3d::color(0.9, 0.9, 0.9));
			m_plane_offset = k3d::xml::attribute_value<double>(xml_move, "plane_offset", 5);
			m_plane_size = k3d::xml::attribute_value<double>(xml_move, "plane_size", 4.8);
			m_screen_xy_radius = k3d::xml::attribute_value<double>(xml_move, "screen_xy_radius", 0.5);
			m_screen_xy_slices = k3d::xml::attribute_value<unsigned long>(xml_move, "screen_xy_slices", 8);
			m_screen_xy_stacks = k3d::xml::attribute_value<unsigned long>(xml_move, "screen_xy_stacks", 8);
			m_axis_start = k3d::xml::attribute_value<double>(xml_move, "axis_start", 5);
			m_axis_end = k3d::xml::attribute_value<double>(xml_move, "axis_end", 10);
			m_axis_line_width = k3d::xml::attribute_value<double>(xml_move, "axis_line_width", 1);
			m_axis_arrow_radius = k3d::xml::attribute_value<double>(xml_move, "axis_arrow_radius", 0.5);
			m_axis_arrow_length = k3d::xml::attribute_value<double>(xml_move, "axis_arrow_length", 2.0);
			m_axis_arrow_slices = k3d::xml::attribute_value<unsigned long>(xml_move, "axis_arrow_slices", 8);
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}

		set_constraint("screen_xy");
	}

	void redraw(viewport::control& Viewport, const bool Visible, const k3d::point3& Position, const k3d::matrix4& Orientation, tool_selection::motion_t Motion, const k3d::point3& Scaling)
	{
		// Sanity checks
		return_if_fail(Viewport.gl_engine());
		return_if_fail(Viewport.camera());

		// Update constraints
		const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*Viewport.camera());
		const k3d::vector3 screen_normal = screen_matrix * k3d::vector3(0, 0, 1);

		// Update the screen xy constraint so it always aligns with the camera direction vector in world coordinates
		m_screen_xy_constraint.set_plane(k3d::plane(screen_normal, Position), 1, 1, 1);

		// Update axis constraints so their planes are always as perpendicular to the screen as-possible
		m_x_constraint.set_plane(k3d::plane((Orientation * k3d::vector3(1, 0, 0)) ^ ((Orientation * k3d::vector3(1, 0, 0)) ^ screen_normal), Position), 1, 0, 0);
		m_y_constraint.set_plane(k3d::plane((Orientation * k3d::vector3(0, 1, 0)) ^ ((Orientation * k3d::vector3(0, 1, 0)) ^ screen_normal), Position), 0, 1, 0);
		m_z_constraint.set_plane(k3d::plane((Orientation * k3d::vector3(0, 0, 1)) ^ ((Orientation * k3d::vector3(0, 0, 1)) ^ screen_normal), Position), 0, 0, 1);

		// Update plane constraints so they have the correct positions in world coordinates
		m_xy_constraint.set_plane(k3d::plane(Orientation * k3d::vector3(0, 0, 1), Position), 1, 1, 0);
		m_xz_constraint.set_plane(k3d::plane(Orientation * k3d::vector3(0, 1, 0), Position), 1, 0, 1);
		m_yz_constraint.set_plane(k3d::plane(Orientation * k3d::vector3(1, 0, 0), Position), 0, 1, 1);

		// Draw manipulators
		if(!Visible)
			return;

		k3d::gl::store_attributes attributes;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translate3(k3d::to_vector(Position)));
		k3d::gl::push_matrix(Orientation);

		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators to show them at constant size (except on mouse drag)
		if(tool_selection::MOTION_NONE == Motion)
			m_scale = detail::manipulators_scale(Viewport, Position, m_manipulators_size);

		glScaled(m_scale, m_scale, m_scale);

		// Draw manipulators
		k3d::vector3 px;
		k3d::vector3 py;
		k3d::vector3 pz;

		const bool motion = (tool_selection::MOTION_DRAG == Motion) || (tool_selection::MOTION_CLICK_DRAG == Motion);

		if(!motion || &m_x_constraint == m_current_constraint || &m_xy_constraint == m_current_constraint || &m_xz_constraint == m_current_constraint)
		{
			if(is_front_facing(Viewport, k3d::vector3(1, 0, 0), Position, Orientation))
			{
				px = k3d::vector3(1, 0, 0);
				draw_axis(m_x_color, m_current_constraint == &m_x_constraint ? m_current_color : m_x_color, k3d::rotate3(k3d::radians(90.0), k3d::vector3(0, 1, 0)));
			}
			else
			{
				px = k3d::vector3(-1, 0, 0);
				draw_axis(m_x_color, m_current_constraint == &m_x_constraint ? m_current_color : m_x_color, k3d::rotate3(k3d::radians(-90.0), k3d::vector3(0, 1, 0)));
			}
		}

		if(!motion || &m_y_constraint == m_current_constraint || &m_xy_constraint == m_current_constraint || &m_yz_constraint == m_current_constraint)
		{
			if(is_front_facing(Viewport, k3d::vector3(0, -1, 0), Position, Orientation))
			{
				py = k3d::vector3(0, -1, 0);
				draw_axis(m_y_color, m_current_constraint == &m_y_constraint ? m_current_color : m_y_color, k3d::rotate3(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
			}
			else
			{
				py = k3d::vector3(0, 1, 0);
				draw_axis(m_y_color, m_current_constraint == &m_y_constraint ? m_current_color : m_y_color, k3d::rotate3(k3d::radians(-90.0), k3d::vector3(1, 0, 0)));
			}
		}

		if(!motion || &m_z_constraint == m_current_constraint || &m_xz_constraint == m_current_constraint || &m_yz_constraint == m_current_constraint)
		{
			if(is_front_facing(Viewport, k3d::vector3(0, 0, 1), Position, Orientation))
			{
				pz = k3d::vector3(0, 0, 1);
				draw_axis(m_z_color, m_current_constraint == &m_z_constraint ? m_current_color : m_z_color, k3d::rotate3(k3d::radians(0.0), k3d::vector3(0, 1, 0)));
			}
			else
			{
				pz = k3d::vector3(0, 0, -1);
				draw_axis(m_z_color, m_current_constraint == &m_z_constraint ? m_current_color : m_z_color, k3d::rotate3(k3d::radians(180.0), k3d::vector3(0, 1, 0)));
			}
		}

		if(!motion || &m_yz_constraint == m_current_constraint)
			draw_plane(m_current_constraint == &m_yz_constraint ? m_current_color : m_x_color, py, py + pz, pz);
		if(!motion || &m_xz_constraint == m_current_constraint)
			draw_plane(m_current_constraint == &m_xz_constraint ? m_current_color : m_y_color, px, px + pz, pz);
		if(!motion || &m_xy_constraint == m_current_constraint)
			draw_plane(m_current_constraint == &m_xy_constraint ? m_current_color : m_z_color, px, px + py, py);

		if(!motion || &m_screen_xy_constraint == m_current_constraint)
			draw_screen_xy(Viewport, m_current_constraint == &m_screen_xy_constraint ? m_current_color : m_screen_xy_color);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void select(viewport::control& Viewport, const k3d::point3& Position, const k3d::matrix4& Orientation)
	{
		k3d::gl::store_attributes attributes;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translate3(k3d::to_vector(Position)));
		k3d::gl::push_matrix(Orientation);

		glDisable(GL_LIGHTING);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators
		glScaled(m_scale, m_scale, m_scale);

		// Draw manipulators
		k3d::vector3 px;
		k3d::vector3 py;
		k3d::vector3 pz;

		if(is_front_facing(Viewport, k3d::vector3(1, 0, 0), Position, Orientation))
		{
			px = k3d::vector3(1, 0, 0);
			select_axis(m_x_constraint, k3d::rotate3(k3d::radians(90.0), k3d::vector3(0, 1, 0)));
		}
		else
		{
			px = k3d::vector3(-1, 0, 0);
			select_axis(m_x_constraint, k3d::rotate3(k3d::radians(-90.0), k3d::vector3(0, 1, 0)));
		}

		if(is_front_facing(Viewport, k3d::vector3(0, -1, 0), Position, Orientation))
		{
			py = k3d::vector3(0, -1, 0);
			select_axis(m_y_constraint, k3d::rotate3(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
		}
		else
		{
			py = k3d::vector3(0, 1, 0);
			select_axis(m_y_constraint, k3d::rotate3(k3d::radians(-90.0), k3d::vector3(1, 0, 0)));
		}

		if(is_front_facing(Viewport, k3d::vector3(0, 0, 1), Position, Orientation))
		{
			pz = k3d::vector3(0, 0, 1);
			select_axis(m_z_constraint, k3d::rotate3(k3d::radians(0.0), k3d::vector3(0, 1, 0)));
		}
		else
		{
			pz = k3d::vector3(0, 0, -1);
			select_axis(m_z_constraint, k3d::rotate3(k3d::radians(180.0), k3d::vector3(0, 1, 0)));
		}

		select_plane(m_yz_constraint, py, py + pz, pz);
		select_plane(m_xz_constraint, px, px + pz, pz);
		select_plane(m_xy_constraint, px, px + py, py);

		select_screen_xy(m_screen_xy_constraint);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	std::string get_constraint_name()
	{
		return_val_if_fail(m_current_constraint, "");

		return m_current_constraint->label();
	}

	// Set manipulator constraint
	Glib::RefPtr<Gdk::Pixbuf> set_constraint(const std::string& Name)
	{
		if(Name == "screen_xy")
			m_current_constraint = &m_screen_xy_constraint;
		else if(Name == "x_axis")
			m_current_constraint = &m_x_constraint;
		else if(Name == "y_axis")
			m_current_constraint = &m_y_constraint;
		else if(Name == "z_axis")
			m_current_constraint = &m_z_constraint;
		else if(Name == "xy_plane")
			m_current_constraint = &m_xy_constraint;
		else if(Name == "xz_plane")
			m_current_constraint = &m_xz_constraint;
		else if(Name == "yz_plane")
			m_current_constraint = &m_yz_constraint;
		else
			assert_not_reached();

		return m_current_constraint->cursor();
	}

	std::string constraint_name(const k3d::selection::id ID)
	{
		if(ID == m_screen_xy_constraint.m_selection_token.id)
			return "screen_xy";
		else if(ID == m_x_constraint.m_selection_token.id)
			return "x_axis";
		else if(ID == m_y_constraint.m_selection_token.id)
			return "y_axis";
		else if(ID == m_z_constraint.m_selection_token.id)
			return "z_axis";
		else if(ID == m_xy_constraint.m_selection_token.id)
			return "xy_plane";
		else if(ID == m_xz_constraint.m_selection_token.id)
			return "xz_plane";
		else if(ID == m_yz_constraint.m_selection_token.id)
			return "yz_plane";

		return "";
	}

	std::string cycle_constraint(constraint* Constraint)
	{
		if(Constraint == &m_x_constraint)
			return "y_axis";

		if(Constraint == &m_y_constraint)
			return "z_axis";

		if(Constraint == &m_z_constraint)
			return "x_axis";

		return "";
	}

	void update_constraint(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::point3& Position, const k3d::matrix4& Orientation)
	{
		// Cycle through X - Screen - Y - Screen - Z - Screen - X - etc
		if(m_current_constraint == &m_x_constraint || m_current_constraint == &m_y_constraint || m_current_constraint == &m_z_constraint)
		{
			// Save current constraint
			m_previous_constraint = m_current_constraint;

			// Go back to screeen mode
			set_constraint("screen_xy");
		}
		else if(m_previous_constraint)
		{
			// An axis was previously chosen, cycle to next one
			set_constraint(cycle_constraint(m_previous_constraint));
		}
		else
		{
			// Constrain movement to the "nearest" axis
			const k3d::point2 mouse(Coordinates);
			const k3d::point2 coords = Viewport.project(Position);

			std::map<double, std::string> constraints;
			constraints.insert(std::make_pair(k3d::distance(mouse, k3d::line2(coords, Viewport.project(Position + (Orientation * k3d::vector3(1, 0, 0))))), "x_axis"));
			constraints.insert(std::make_pair(k3d::distance(mouse, k3d::line2(coords, Viewport.project(Position + (Orientation * k3d::vector3(0, 1, 0))))), "y_axis"));
			constraints.insert(std::make_pair(k3d::distance(mouse, k3d::line2(coords, Viewport.project(Position + (Orientation * k3d::vector3(0, 0, 1))))), "z_axis"));

			set_constraint(constraints.begin()->second);
		}

		begin_mouse_move(Coordinates, Position);
	}

	void begin_mouse_move(const k3d::point2& Coordinates, const k3d::point3& Origin)
	{
		m_current_constraint->begin_mouse_move(Coordinates, Origin);
	}

	k3d::vector3 mouse_move(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::point3& Position, const k3d::matrix4& Orientation)
	{
		return m_current_constraint->mouse_move(Viewport, Coordinates, Position, Orientation);
	}

private:
	// Manipulator drawing/selection functions
	void draw_screen_xy(viewport::control& Viewport, const k3d::color& Color)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::color3d(Color);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, Color);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glEnable(GL_LIGHTING);
		gluQuadricDrawStyle(m_quadric, GLU_FILL);
		gluQuadricNormals(m_quadric, GLU_SMOOTH);
		gluSphere(m_quadric, m_screen_xy_radius, m_screen_xy_slices, m_screen_xy_stacks);

		glPopMatrix();
	}

	void select_screen_xy(constraint& Constraint)
	{
		k3d::gl::store_attributes attributes;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		k3d::gl::push_selection_token(Constraint.m_selection_token);

		glDisable(GL_LIGHTING);
		gluQuadricDrawStyle(m_quadric, GLU_FILL);
		gluQuadricNormals(m_quadric, GLU_NONE);
		gluSphere(m_quadric, m_screen_xy_radius, m_screen_xy_slices, m_screen_xy_stacks);

		k3d::gl::pop_selection_token();

		glPopMatrix();
	}

	void draw_axis(const k3d::color& ArrowColor, const k3d::color& AxisColor, const k3d::matrix4& Matrix)
	{
		k3d::gl::store_attributes attributes;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(Matrix);

		glPushMatrix();
		k3d::gl::push_matrix(k3d::translate3(0, 0, m_axis_end - (0.5 * m_axis_arrow_length)));
		glEnable(GL_LIGHTING);

		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, AxisColor);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));
		gluQuadricDrawStyle(m_quadric, GLU_FILL);
		gluQuadricNormals(m_quadric, GLU_SMOOTH);
		gluCylinder(m_quadric, m_axis_arrow_radius, m_axis_arrow_radius * 0.001, m_axis_arrow_length, m_axis_arrow_slices, 1);
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(m_axis_line_width);
		k3d::gl::color3d(AxisColor);
		glBegin(GL_LINES);
			glVertex3d(0, 0, m_axis_start);
			glVertex3d(0, 0, m_axis_end);
		glEnd();

		glPopMatrix();
	}

	void select_axis(constraint& Constraint, const k3d::matrix4& Matrix)
	{
		k3d::gl::store_attributes attributes;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(Matrix);

		k3d::gl::push_selection_token(Constraint.m_selection_token);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translate3(0, 0, m_axis_end - (0.5 * m_axis_arrow_length)));
		gluQuadricDrawStyle(m_quadric, GLU_FILL);
		gluQuadricNormals(m_quadric, GLU_NONE);
		gluCylinder(m_quadric, m_axis_arrow_radius, m_axis_arrow_radius * 0.001, m_axis_arrow_length, m_axis_arrow_slices, 1);
		glPopMatrix();

		glBegin(GL_LINES);
			glVertex3d(0, 0, m_axis_start);
			glVertex3d(0, 0, m_axis_end);
		glEnd();
		k3d::gl::pop_selection_token();

		glPopMatrix();
	}

	void draw_plane(const k3d::color& Color, const k3d::vector3& P1, const k3d::vector3& P2, const k3d::vector3& P3)
	{
		k3d::gl::store_attributes attributes;
		k3d::gl::color3d(Color);

		glDisable(GL_LIGHTING);
		glLineWidth(m_axis_line_width);

		glBegin(GL_LINE_STRIP);
			k3d::gl::vertex3d(k3d::point3(0, 0, 0) + (P1 * m_plane_size));
			k3d::gl::vertex3d(k3d::point3(0, 0, 0) + (P2 * m_plane_size));
			k3d::gl::vertex3d(k3d::point3(0, 0, 0) + (P3 * m_plane_size));
		glEnd();
	}

	void select_plane(constraint& Constraint, const k3d::vector3& P1, const k3d::vector3& P2, const k3d::vector3& P3)
	{
		k3d::gl::store_attributes attributes;
		k3d::gl::push_selection_token(Constraint.m_selection_token);

		glDisable(GL_LIGHTING);

		glBegin(GL_QUADS);
			k3d::gl::vertex3d(k3d::point3(0, 0, 0));
			k3d::gl::vertex3d(k3d::point3(0, 0, 0) + (P1 * m_plane_size));
			k3d::gl::vertex3d(k3d::point3(0, 0, 0) + (P2 * m_plane_size));
			k3d::gl::vertex3d(k3d::point3(0, 0, 0) + (P3 * m_plane_size));
		glEnd();

		k3d::gl::pop_selection_token();
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	/// Stores the constraint that was in effect when movement began (if any)
	constraint* m_current_constraint;
	/// Stores the constraint that was in effect before m_current_constraint
	constraint* m_previous_constraint;

	// These are placeholders, used to identify hotspots in the manipulators
	constraint m_screen_xy_constraint;
	constraint m_x_constraint;
	constraint m_y_constraint;
	constraint m_z_constraint;
	constraint m_xy_constraint;
	constraint m_xz_constraint;
	constraint m_yz_constraint;

	// Parameters that control the on-screen appearance of the tool
	double m_manipulators_size;
	double m_scale;
	k3d::color m_current_color;
	k3d::color m_x_color;
	k3d::color m_y_color;
	k3d::color m_z_color;
	k3d::color m_screen_xy_color;
	double m_plane_offset;
	double m_plane_size;
	double m_screen_xy_radius;
	unsigned long m_screen_xy_slices;
	unsigned long m_screen_xy_stacks;
	double m_axis_start;
	double m_axis_end;
	double m_axis_line_width;
	double m_axis_arrow_radius;
	double m_axis_arrow_length;
	unsigned long m_axis_arrow_slices;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// move_tool

move_tool::move_tool(document_state& DocumentState, const std::string& Name) :
	transform_tool_base(DocumentState.document(), DocumentState, Name),
	m_mutex(false),
	m_move(init_owner(*this) + init_name("move") + init_label(_("Move")) + init_description(_("Move")) + init_value(k3d::point3(0, 0, 0))),
	m_world_position(init_owner(*this) + init_name("world_position") + init_label(_("World position")) + init_description(_("Manipulators' world position")) + init_value(k3d::point3(0, 0, 0)))
{
	m_world_position.set_update_slot(sigc::mem_fun(*this, &move_tool::get_world_position));

	m_move.connect_explicit_change_signal(sigc::mem_fun(*this, &move_tool::on_move));

	m_input_model.connect_lbutton_down(sigc::mem_fun(*this, &move_tool::on_lbutton_down));
	m_input_model.connect_lbutton_click(sigc::mem_fun(*this, &move_tool::on_lbutton_click));
	m_input_model.connect_lbutton_start_drag(sigc::mem_fun(*this, &move_tool::on_lbutton_start_drag));
	m_input_model.connect_lbutton_drag(sigc::mem_fun(*this, &move_tool::on_lbutton_drag));
	m_input_model.connect_lbutton_end_drag(sigc::mem_fun(*this, &move_tool::on_lbutton_end_drag));
	m_input_model.connect_mbutton_click(sigc::mem_fun(*this, &move_tool::on_mbutton_click));
	m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &move_tool::on_rbutton_click));
	m_input_model.connect_mouse_move(sigc::mem_fun(*this, &move_tool::on_mouse_move));

	m_manipulators = new detail::move_manipulators();
}

move_tool::~move_tool()
{
	delete m_manipulators;
}

void move_tool::on_activate()
{
	m_manipulators->activate();

	m_visible_manipulators.set_value(true);

	on_document_selection_changed();

	tool_selection::redraw_all();
}

void move_tool::on_deactivate()
{
	if(MOTION_NONE != m_current_motion)
		cancel_mouse_move();

	clear_targets();

	m_document_state.clear_cursor_signal().emit();
	tool_selection::redraw_all();
}

void move_tool::on_document_selection_changed()
{
	target_list_changed();
	update_targets();
	reset();

	// Update world position
	m_world_position.update();
}

void move_tool::on_redraw(viewport::control& Viewport)
{
	// Update target list
	update_targets();

	// Redraw manipulators
	m_manipulators->redraw(Viewport, (m_visible_manipulators.internal_value() && target_number()), world_position(), world_orientation(), m_current_motion, k3d::point3(1.0, 1.0, 1.0));
}

void move_tool::on_select(viewport::control& Viewport)
{
	if(m_visible_manipulators.internal_value() && target_number())
		m_manipulators->select(Viewport, world_position(), world_orientation());
}

k3d::iproperty_collection* move_tool::get_property_collection()
{
	return dynamic_cast<k3d::iproperty_collection*>(this);
}

viewport_input_model& move_tool::get_input_model()
{
	return input_model();
}

// LMB drag actions
k3d::vector3 move_tool::mouse_move_to_3d(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	// Transform mouse move to a world move
	const k3d::point3 origin = world_position();
	const k3d::matrix4 orientation = world_orientation();
	const k3d::vector3 delta = m_manipulators->mouse_move(Viewport, Coordinates, origin, orientation);

	return delta;
}

k3d::vector3 move_tool::lbutton_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_DRAG == m_current_motion)
	{
		const k3d::vector3 delta = mouse_move_to_3d(Viewport, Coordinates);
		move_selection(delta);

		return delta;
	}

	if(MOTION_BOX_SELECT == m_current_motion)
		lmb_drag_box_select(Viewport, Coordinates);

	return k3d::vector3(0, 0, 0);
}

k3d::vector3 move_tool::mouse_move_action(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_CLICK_DRAG == m_current_motion)
	{
		const k3d::vector3 delta = mouse_move_to_3d(Viewport, Coordinates);
		move_selection(delta);

		return delta;
	}

	return k3d::vector3(0, 0, 0);
}

void move_tool::move_selection(const k3d::vector3& Delta)
{
	if(Delta == k3d::vector3(0, 0, 0))
		return;

	const k3d::point3 new_position = m_move.internal_value() + Delta;
	m_move.set_value(new_position);
}

void move_tool::on_move(k3d::iunknown*)
{
	if(m_mutex)
		return;

	move_targets(k3d::to_vector(m_move.internal_value()));

	// Update world position
	m_world_position.update();
}

void move_tool::get_world_position(const std::vector<k3d::ihint*>& Hints, k3d::point3& Output)
{
	Output = world_position();
}

} // namespace ngui

} // namespace k3d

