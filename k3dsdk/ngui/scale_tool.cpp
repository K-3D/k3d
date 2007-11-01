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

#include "command_arguments.h"
#include "document_state.h"
#include "modifiers.h"
#include "icons.h"
#include "interactive.h"
#include "keyboard.h"
#include "scale_tool.h"
#include "transform_tool.h"
#include "utility.h"
#include "viewport.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/geometric_operations.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/line2.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/plane.h>
#include <k3dsdk/property.h>
#include <k3dsdk/share.h>
#include <k3dsdk/xml.h>

#include <k3dsdk/fstream.h>

namespace libk3dngui
{

namespace detail
{

class scale_manipulators :
	public imanipulators<k3d::point3>
{
private:
	struct constraint :
		public k3d::selectable
	{
		constraint(const std::string& Label, Glib::RefPtr<Gdk::Pixbuf> Cursor, const k3d::selection::token& SelectionToken, const bool RelativeMove = false) :
			m_selection_token(SelectionToken),
			m_label(Label),
			m_cursor(Cursor),
			m_relative_move(RelativeMove),
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
			m_first_mouse = Mouse;
			m_scaling_origin = Origin;

			m_last_mouse = Mouse;
			m_current_scale = 1;
		}

		k3d::point3 mouse_move(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::point3& Position, const k3d::matrix4& Orientation)
		{
			const k3d::point2 current_mouse(Coordinates);
			const k3d::point2 origin = Viewport.project(m_scaling_origin);
			const double sign = k3d::sign((m_first_mouse - origin) * (current_mouse - origin));

			if(m_relative_move)
			{
				// Relative move
				const k3d::vector2 difference = current_mouse - m_last_mouse;
				m_last_mouse = current_mouse;
				double delta = 0;

				const double horizontal_length = difference[0];
				const double vertical_length = difference[1];
				if(std::abs(horizontal_length) > std::abs(vertical_length))
				{
					// Dragging mostly horizontally : 1/10th unit increase
					delta = 0.1 * horizontal_length;
				}
				else
				{
					// Dragging mostly vertically : one unit increase
					delta = - vertical_length;
				}

				delta = 1 + 0.02 * delta;
				m_current_scale *= delta;

				return k3d::point3(m_x_sensitivity ? m_current_scale * m_x_sensitivity : 1.0, m_y_sensitivity ? m_current_scale * m_y_sensitivity : 1.0, m_z_sensitivity ? m_current_scale * m_z_sensitivity : 1.0);
			}

			// Absolute move

			// Turn mouse coordinates into lines in world coordinates ...
			k3d::line3 first_line = mouse_to_world(Viewport, m_first_mouse);
			k3d::line3 current_line = mouse_to_world(Viewport, current_mouse);

			// Calculate intersections with our plane in world coordinates ...
			k3d::point3 first_intersection;
			return_val_if_fail(k3d::intersect(m_plane, first_line, first_intersection), k3d::point3(1, 1, 1));

			double factor = k3d::distance(first_intersection, Position);
			if(!factor)
				factor = 1.0;

			k3d::point3 current_intersection;
			return_val_if_fail(k3d::intersect(m_plane, current_line, current_intersection), k3d::point3(1, 1, 1));

			// Calculate scaling from distance to origin
			const double delta = sign * k3d::distance(current_intersection, Position) / factor;

			return k3d::point3(m_x_sensitivity ? delta * m_x_sensitivity : 1.0, m_y_sensitivity ? delta * m_y_sensitivity : 1.0, m_z_sensitivity ? delta * m_z_sensitivity : 1.0);
		}

		const k3d::selection::token m_selection_token;
		const std::string m_label;
		const Glib::RefPtr<Gdk::Pixbuf> m_cursor;
		const bool m_relative_move;

		/// Stores the constraint plane in world coordinates
		k3d::plane m_plane;

		double m_x_sensitivity;
		double m_y_sensitivity;
		double m_z_sensitivity;
		k3d::point2 m_last_mouse;

		k3d::point2 m_first_mouse;
		k3d::point3 m_scaling_origin;
		double m_current_scale;
	};

public:
	scale_manipulators() :
		m_current_constraint(&m_xyz_constraint),
		m_xyz_constraint(_("Scale XYZ"), load_icon("move_cursor_screen_xy", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 0), true),
		m_x_constraint(_("Scale X"), load_icon("move_cursor_x", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 1)),
		m_y_constraint(_("Scale Y"), load_icon("move_cursor_y", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 2)),
		m_z_constraint(_("Scale Z"), load_icon("move_cursor_z", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 3)),
		m_xy_constraint(_("Scale XY"), load_icon("move_cursor_xy", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 4)),
		m_xz_constraint(_("Scale XZ"), load_icon("move_cursor_xz", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 5)),
		m_yz_constraint(_("Scale YZ"), load_icon("move_cursor_yz", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 6))
	{
	}

	void activate()
	{
		// Load parameters from configuration file
		try
		{
			k3d::filesystem::ifstream layout_stream(k3d::share_path() / k3d::filesystem::generic_path("ngui/tool_layout.k3d"));
			k3d::xml::element layout_xml;
			layout_stream >> layout_xml;
			k3d::xml::element& xml_scale = layout_xml.safe_element("application").safe_element("user_interface").safe_element("tools").safe_element("scale");

			m_manipulators_size = k3d::xml::attribute_value<double>(xml_scale, "size", 10);
			m_current_color = k3d::xml::attribute_value<k3d::color>(xml_scale, "current_color", k3d::color(1, 1, 0));
			m_x_color = k3d::xml::attribute_value<k3d::color>(xml_scale, "x_color", k3d::color(1, 0, 0));
			m_y_color = k3d::xml::attribute_value<k3d::color>(xml_scale, "y_color", k3d::color(0, 1, 0));
			m_z_color = k3d::xml::attribute_value<k3d::color>(xml_scale, "z_color", k3d::color(0, 0, 1));
			m_screen_xy_color = k3d::xml::attribute_value<k3d::color>(xml_scale, "screen_xy_color", k3d::color(0.9, 0.9, 0.9));
			m_plane_offset = k3d::xml::attribute_value<double>(xml_scale, "plane_offset", 5);
			m_plane_size = k3d::xml::attribute_value<double>(xml_scale, "plane_size", 4.8);
			m_axis_start = k3d::xml::attribute_value<double>(xml_scale, "axis_start", 0);
			m_axis_end = k3d::xml::attribute_value<double>(xml_scale, "axis_end", 10);
			m_axis_line_width = k3d::xml::attribute_value<double>(xml_scale, "axis_line_width", 1);
			m_axis_box_size = k3d::xml::attribute_value<double>(xml_scale, "axis_box_size", 0.5);
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}

		set_constraint("xyz_constraint");
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
		m_xyz_constraint.set_plane(k3d::plane(screen_normal, Position), 1, 1, 1);

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

		const bool motion = (tool_selection::MOTION_DRAG == Motion) || (tool_selection::MOTION_CLICK_DRAG == Motion);
		if(motion)
			k3d::gl::push_matrix(k3d::translation3D(Position) * Orientation * k3d::scaling3D(Scaling));
		else
			k3d::gl::push_matrix(k3d::translation3D(Position) * Orientation);

		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators to show them at constant size (except on mouse drag)
		if(tool_selection::MOTION_NONE == Motion)
			m_scale = detail::manipulators_scale(Viewport, Position, m_manipulators_size);

		glScaled(m_scale, m_scale, m_scale);

		// Draw manipulators
		k3d::vector3 px;
		k3d::vector3 py;
		k3d::vector3 pz;

		if(!motion || &m_x_constraint == m_current_constraint || &m_xy_constraint == m_current_constraint || &m_xz_constraint == m_current_constraint)
		{
			if(is_front_facing(Viewport, k3d::vector3(1, 0, 0), Position, Orientation))
			{
				px = k3d::vector3(1, 0, 0);
				draw_axis(m_x_color, m_current_constraint == &m_x_constraint ? m_current_color : m_x_color, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(0, 1, 0)));
			}
			else
			{
				px = k3d::vector3(-1, 0, 0);
				draw_axis(m_x_color, m_current_constraint == &m_x_constraint ? m_current_color : m_x_color, k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(0, 1, 0)));
			}
		}

		if(!motion || &m_y_constraint == m_current_constraint || &m_xy_constraint == m_current_constraint || &m_yz_constraint == m_current_constraint)
		{
			if(is_front_facing(Viewport, k3d::vector3(0, -1, 0), Position, Orientation))
			{
				py = k3d::vector3(0, -1, 0);
				draw_axis(m_y_color, m_current_constraint == &m_y_constraint ? m_current_color : m_y_color, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
			}
			else
			{
				py = k3d::vector3(0, 1, 0);
				draw_axis(m_y_color, m_current_constraint == &m_y_constraint ? m_current_color : m_y_color, k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(1, 0, 0)));
			}
		}

		if(!motion || &m_z_constraint == m_current_constraint || &m_xz_constraint == m_current_constraint || &m_yz_constraint == m_current_constraint)
		{
			if(is_front_facing(Viewport, k3d::vector3(0, 0, 1), Position, Orientation))
			{
				pz = k3d::vector3(0, 0, 1);
				draw_axis(m_z_color, m_current_constraint == &m_z_constraint ? m_current_color : m_z_color, k3d::rotation3D(k3d::radians(0.0), k3d::vector3(0, 1, 0)));
			}
			else
			{
				pz = k3d::vector3(0, 0, -1);
				draw_axis(m_z_color, m_current_constraint == &m_z_constraint ? m_current_color : m_z_color, k3d::rotation3D(k3d::radians(180.0), k3d::vector3(0, 1, 0)));
			}
		}

		if(!motion || &m_yz_constraint == m_current_constraint)
			draw_plane(m_current_constraint == &m_yz_constraint ? m_current_color : m_x_color, py, py + pz, pz);
		if(!motion || &m_xz_constraint == m_current_constraint)
			draw_plane(m_current_constraint == &m_xz_constraint ? m_current_color : m_y_color, px, px + pz, pz);
		if(!motion || &m_xy_constraint == m_current_constraint)
			draw_plane(m_current_constraint == &m_xy_constraint ? m_current_color : m_z_color, px, px + py, py);

		if(!motion || &m_xyz_constraint == m_current_constraint)
			draw_xyz(Viewport, m_current_constraint == &m_xyz_constraint ? m_current_color : m_screen_xy_color);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void select(viewport::control& Viewport, const k3d::point3& Position, const k3d::matrix4& Orientation)
	{
		k3d::gl::store_attributes attributes;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translation3D(Position) * Orientation);

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
			select_axis(m_x_constraint, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(0, 1, 0)));
		}
		else
		{
			px = k3d::vector3(-1, 0, 0);
			select_axis(m_x_constraint, k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(0, 1, 0)));
		}

		if(is_front_facing(Viewport, k3d::vector3(0, -1, 0), Position, Orientation))
		{
			py = k3d::vector3(0, -1, 0);
			select_axis(m_y_constraint, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
		}
		else
		{
			py = k3d::vector3(0, 1, 0);
			select_axis(m_y_constraint, k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(1, 0, 0)));
		}

		if(is_front_facing(Viewport, k3d::vector3(0, 0, 1), Position, Orientation))
		{
			pz = k3d::vector3(0, 0, 1);
			select_axis(m_z_constraint, k3d::rotation3D(k3d::radians(0.0), k3d::vector3(0, 1, 0)));
		}
		else
		{
			pz = k3d::vector3(0, 0, -1);
			select_axis(m_z_constraint, k3d::rotation3D(k3d::radians(180.0), k3d::vector3(0, 1, 0)));
		}

		select_plane(m_yz_constraint, py, py + pz, pz);
		select_plane(m_xz_constraint, px, px + pz, pz);
		select_plane(m_xy_constraint, px, px + py, py);

		select_xyz(m_xyz_constraint);

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
		if(Name == "xyz_constraint")
			m_current_constraint = &m_xyz_constraint;
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
		if(ID == m_xyz_constraint.m_selection_token.id)
			return "xyz_constraint";
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
			set_constraint("xyz_constraint");
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

	void begin_mouse_move(const k3d::point2& Coordinates, const k3d::point3& Position)
	{
		m_current_constraint->begin_mouse_move(Coordinates, Position);
	}

	k3d::point3 mouse_move(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::point3& Position, const k3d::matrix4& Orientation)
	{
		return m_current_constraint->mouse_move(Viewport, Coordinates, Position, Orientation);
	}

private:
	// Manipulator drawing/selection functions
	void draw_xyz(viewport::control& Viewport, const k3d::color& Color)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::color3d(Color);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, Color);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, Color * 0.2);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glEnable(GL_LIGHTING);

		k3d::gl::draw(k3d::bounding_box3(m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size));

		glPopMatrix();
	}

	void select_xyz(constraint& Constraint)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::push_selection_token(Constraint.m_selection_token);
		k3d::gl::draw(k3d::bounding_box3(m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size));
		k3d::gl::pop_selection_token();
	}

	void draw_axis(const k3d::color& ArrowColor, const k3d::color& AxisColor, const k3d::matrix4& Matrix)
	{
		k3d::gl::store_attributes attributes;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(Matrix);

		glPushMatrix();
		k3d::gl::push_matrix(k3d::translation3D(k3d::point3(0, 0, m_axis_end)));
		glEnable(GL_LIGHTING);

		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, AxisColor);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, AxisColor * 0.2);
		k3d::gl::draw(k3d::bounding_box3(m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size));
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
		k3d::gl::push_matrix(k3d::translation3D(k3d::point3(0, 0, m_axis_end)));
		k3d::gl::draw(k3d::bounding_box3(m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size, m_axis_box_size, -m_axis_box_size));
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

	/// Stores the constraint that was in effect when scaling began (if any)
	constraint* m_current_constraint;
	/// Stores the constraint that was in effect before m_current_constraint
	constraint* m_previous_constraint;

	// These are placeholders, used to identify hotspots in the manipulators
	constraint m_xyz_constraint;
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
	double m_axis_start;
	double m_axis_end;
	double m_axis_line_width;
	double m_axis_box_size;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// scale_tool

scale_tool::scale_tool(document_state& DocumentState, const std::string& Name) :
	transform_tool_base(DocumentState.document(), DocumentState, Name),
	m_scaling(init_owner(*this) + init_name("scaling") + init_label(_("Scaling")) + init_description(_("Scaling")) + init_value(k3d::point3(1, 1, 1))),
	m_center(init_owner(*this) + init_name("center") + init_label(_("Center")) + init_description(_("Center")) + init_value(k3d::point3(0, 0, 0)))
{
	//navigation_model().connect_command_signal(sigc::mem_fun(*this, &scale_tool::record_command));
	m_scaling.connect_explicit_change_signal(sigc::mem_fun(*this, &scale_tool::on_scale));

	m_input_model.connect_lbutton_down(sigc::mem_fun(*this, &scale_tool::on_lbutton_down));
	m_input_model.connect_lbutton_click(sigc::mem_fun(*this, &scale_tool::on_lbutton_click));
	m_input_model.connect_lbutton_start_drag(sigc::mem_fun(*this, &scale_tool::on_lbutton_start_drag));
	m_input_model.connect_lbutton_drag(sigc::mem_fun(*this, &scale_tool::on_lbutton_drag));
	m_input_model.connect_lbutton_end_drag(sigc::mem_fun(*this, &scale_tool::on_lbutton_end_drag));
	m_input_model.connect_mbutton_click(sigc::mem_fun(*this, &scale_tool::on_mbutton_click));
	m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &scale_tool::on_rbutton_click));
	m_input_model.connect_mouse_move(sigc::mem_fun(*this, &scale_tool::on_mouse_move));

	m_manipulators = new detail::scale_manipulators();
}

scale_tool::~scale_tool()
{
	delete m_manipulators;
}

const k3d::icommand_node::result scale_tool::execute_command(const std::string& Command, const std::string& Arguments)
{
	const k3d::icommand_node::result result = navigation_model().execute_command(Command, Arguments);
	if(result != RESULT_UNKNOWN_COMMAND)
		return result;

	try
	{
		command_arguments arguments(Arguments);

		if(Command == "mouse_move")
		{
			interactive::move_pointer(arguments.get_viewport(), arguments.get_viewport_point2("mouse"));
		}
		else if(Command == "mouse_warp")
		{
			interactive::warp_pointer(arguments.get_viewport(), arguments.get_viewport_point2("mouse"));
		}
		else if(Command == "lmb_down_add")
		{
			lmb_down_add();
		}
		else if(Command == "lmb_down_subtract")
		{
			lmb_down_subtract();
		}
		else if(Command.substr(0, std::min(static_cast<size_t>(21), Command.size())) == "lmb_down_manipulator_")
		{
			lmb_down_manipulator(Command.substr(21));
		}
		else if(Command == "lmb_down_selected")
		{
			lmb_down_selected();
		}
		else if(Command == "lmb_down_deselected")
		{
			lmb_down_deselected();
		}
		else if(Command == "lmb_down_nothing")
		{
			lmb_down_nothing();
		}
		else if(Command == "lmb_click_add")
		{
			lmb_click_add();
		}
		else if(Command == "lmb_click_subtract")
		{
			lmb_click_subtract();
		}
		else if(Command == "lmb_click_start_motion")
		{
			lmb_click_start_motion(arguments.get_viewport_point2("mouse"));
		}
		else if(Command == "lmb_click_stop_motion")
		{
			lmb_click_stop_motion();
		}
		else if(Command == "lmb_click_deselect_all")
		{
			lmb_click_deselect_all();
		}
		else if(Command == "lmb_start_drag_start_motion")
		{
			lmb_start_drag_start_motion(arguments.get_viewport_point2("mouse"));
		}
		else if(Command == "lmb_start_drag_box_select")
		{
			lmb_start_drag_box_select(arguments.get_viewport(), arguments.get_viewport_point2("mouse"));
		}
		else if(Command == "lmb_drag_move")
		{
			scale_selection(arguments.get_point3("scaling"));
			k3d::gl::redraw_all(m_document, k3d::gl::irender_viewport::SYNCHRONOUS);
		}
		else if(Command == "lmb_drag_box_select")
		{
			lmb_drag_box_select(arguments.get_viewport(), arguments.get_viewport_point2("mouse"));
		}
		else if(Command == "lmb_end_drag_stop_motion")
		{
			lmb_end_drag_stop_motion();
		}
		else if(Command == "lmb_end_drag_box_select")
		{
			lmb_end_drag_box_select(arguments.get_viewport(), arguments.get_viewport_point2("mouse"));
		}
		else if(Command == "mmb_click_toggle_manipulators_visibility")
		{
			mmb_click_toggle_manipulators_visibility();
		}
		else if(Command == "mmb_click_manipulators_next_selection")
		{
			// TODO : show SHIFT key for tutorials
			mmb_click_manipulators_next_selection();
		}
		else if(Command == "mmb_click_switch_coordinate_system")
		{
			// TODO : show CONTROL key for tutorials
			mmb_click_switch_coordinate_system();
		}
		else if(Command == "mmb_click_next_constraint")
		{
			mmb_click_next_constraint(arguments.get_viewport(), arguments.get_viewport_point2());
		}
		else if(Command == "rmb_click_selection_tool")
		{
			rmb_click_selection_tool();
		}
		else if(Command == "rmb_click_cancel_move")
		{
			rmb_click_cancel_move();
		}
		else if(Command == "mouse_drag_move")
		{
			scale_selection(arguments.get_point3("scaling"));
			k3d::gl::redraw_all(m_document, k3d::gl::irender_viewport::SYNCHRONOUS);
		}
		else
		{
			return transform_tool::execute_command(Command, Arguments);
		}

		return RESULT_CONTINUE;
	}
	catch(std::exception& e)
	{
		k3d::log() << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
		return RESULT_ERROR;
	}

	return RESULT_UNKNOWN_COMMAND;
}

void scale_tool::on_activate()
{
	m_manipulators->activate();

	m_visible_manipulators.set_value(true);

	on_document_selection_changed();

	tool_selection::redraw_all();
}

void scale_tool::on_deactivate()
{
	if(MOTION_NONE != m_current_motion)
		cancel_mouse_move();

	clear_targets();

	m_document_state.clear_cursor_signal().emit();
	tool_selection::redraw_all();
}

void scale_tool::on_document_selection_changed()
{
	target_list_changed();
	update_targets();
	m_scaling.set_value(k3d::point3(1, 1, 1));
	//set_center(world_position());

	m_original_scaling = k3d::point3(1, 1, 1);
}

void scale_tool::on_redraw(viewport::control& Viewport)
{
	// Update target list
	update_targets();

	// Redraw manipulators
	m_manipulators->redraw(Viewport, (m_visible_manipulators.internal_value() && target_number()), world_position(), world_orientation(), m_current_motion, get_scaling());
}

void scale_tool::on_select(viewport::control& Viewport)
{
	if(m_visible_manipulators.internal_value() && target_number())
		m_manipulators->select(Viewport, world_position(), world_orientation());
}

k3d::iproperty_collection* scale_tool::get_property_collection()
{
	return dynamic_cast<k3d::iproperty_collection*>(this);
}

viewport_input_model& scale_tool::get_input_model()
{
	return input_model();
}

void scale_tool::record_command(viewport::control& Viewport, const GdkEventButton& Event, const bool Move)
{
	command_arguments arguments;
	arguments.append_viewport_coordinates("mouse", Viewport, Event);

	if(Move)
		transform_tool::record_command("mouse_move", arguments);

	transform_tool::record_command(m_tutorial_action, arguments);
	m_tutorial_action = "";
}

void scale_tool::record_transform(viewport::control& Viewport, const GdkEventMotion& Event, const k3d::point3& Scaling)
{
	command_arguments arguments;
	arguments.append_viewport_coordinates("mouse", Viewport, Event);

	transform_tool::record_command("mouse_warp", arguments);

	arguments.append("scaling", Scaling);
	transform_tool::record_command(m_tutorial_action, arguments);
	m_tutorial_action = "";
}

// LMB drag/move actions
k3d::point3 scale_tool::mouse_move_to_3d(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	// Transform mouse move to a scaling
	const k3d::point3 origin = world_position();
	const k3d::matrix4 orientation = world_orientation();
	const k3d::point3 scaling = m_manipulators->mouse_move(Viewport, Coordinates, origin, orientation);

	return scaling;
}

k3d::point3 scale_tool::lbutton_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_DRAG == m_current_motion)
	{
		m_tutorial_action = "lmb_drag_move";

		const k3d::point3 scaling = mouse_move_to_3d(Viewport, Coordinates);
		scale_selection(scaling);

		return scaling;
	}

	if(MOTION_BOX_SELECT == m_current_motion)
		lmb_drag_box_select(Viewport, Coordinates);

	return k3d::point3(1, 1, 1);
}

k3d::point3 scale_tool::mouse_move_action(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_CLICK_DRAG == m_current_motion)
	{
		m_tutorial_action = "mouse_drag_move";

		const k3d::point3 scaling = mouse_move_to_3d(Viewport, Coordinates);
		scale_selection(scaling);

		return scaling;
	}

	return k3d::point3(1, 1, 1);
}

void scale_tool::scale_selection(const k3d::point3& Scaling)
{
	if(Scaling == k3d::point3(1, 1, 1))
		return;

	m_scaling.set_value(Scaling);
}

k3d::point3 scale_tool::get_scaling()
{
	return m_scaling.internal_value();
}

void scale_tool::on_scale(k3d::iunknown*)
{
	scale_targets(m_scaling.internal_value());
}

k3d::point3 scale_tool::get_center()
{
	return world_position();
}

} // namespace libk3dngui

