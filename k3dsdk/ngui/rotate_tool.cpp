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
#include "rotate_tool.h"
#include "transform_tool.h"
#include "utility.h"
#include "viewport.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/geometric_operations.h>
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

class rotate_manipulators :
	public imanipulators<k3d::angle_axis>
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
			m_axis(0, 0, 0)
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

		void set_plane(const k3d::plane& Plane, const k3d::vector3& Axis)
		{
			m_plane = Plane;
			m_axis = Axis;
		}

		void begin_mouse_move(const k3d::point2& Mouse, const k3d::point3& Origin)
		{
			m_first_mouse = Mouse;
			m_rotation_origin = Origin;
		}

		k3d::angle_axis mouse_move(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::point3& Position, const k3d::matrix4& Orientation)
		{
			const k3d::point2 current_mouse(Coordinates);
			const k3d::point2 origin = Viewport.project(m_rotation_origin);

			const k3d::vector2 v1 = m_first_mouse - origin;
			const k3d::vector2 v2 = current_mouse - origin;

			// First or current mouse is on the origin
			if(!v1.length2() || !v2.length2())
				return k3d::angle_axis(0, m_axis);

			// Angle between v1 and v2
			const double dot_product = v1 * v2;
			double angle = acos(dot_product / v1.length() / v2.length());

			// Check for rotation manipulator direction
			const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*Viewport.camera());
			// Forward camera vector is +Z
			const k3d::vector3 camera_forward = screen_matrix * k3d::vector3(0, 0, 1);
			if(camera_forward * m_axis < 0)
				angle = -angle;

			// Test whether current_mouse point is below line defined by (m_first_mouse, origin) :
			if(m_first_mouse[0] == origin[0])
			{
				// Vertical line, test left or right instead
				if(current_mouse[0] > origin[0])
					angle = -angle;
			}
			else
			{
				// Check whether m_first_mouse was in the left or right quadrants
				if(m_first_mouse[0] > origin[0])
					angle = -angle;

				// Project current_mouse vertically on line defined by (m_first_mouse, origin)
				const double a = (current_mouse[0] - m_first_mouse[0]) / (origin[0] - m_first_mouse[0]);
				const double y = m_first_mouse[1] + a * (origin[1] - m_first_mouse[1]);
				if(y > current_mouse[1])
					angle = -angle;
			}

			return k3d::angle_axis(angle, m_axis);
		}

		const k3d::vector3 axis()
		{
			return m_axis;
		}

		const k3d::selection::token m_selection_token;
		const std::string m_label;
		const Glib::RefPtr<Gdk::Pixbuf> m_cursor;

		/// Stores the constraint plane in world coordinates
		k3d::plane m_plane;
		/// Stores the rotation axis in world coordinates
		k3d::vector3 m_axis;

		k3d::point2 m_first_mouse;
		k3d::point3 m_rotation_origin;
	};

public:
	rotate_manipulators() :
		m_current_constraint(&m_screen_z_constraint),
		m_screen_z_constraint(_("Rotate Screen Z"), load_icon("move_cursor_screen_xy", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 0)),
		m_x_constraint(_("Rotate X"), load_icon("move_cursor_x", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 1)),
		m_y_constraint(_("Rotate Y"), load_icon("move_cursor_y", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 2)),
		m_z_constraint(_("Rotate Z"), load_icon("move_cursor_z", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 3))
	{
	}

	void activate()
	{
		try
		{
			k3d::filesystem::ifstream layout_stream(k3d::share_path() / k3d::filesystem::generic_path("ngui/tool_layout.k3d"));
			k3d::xml::element layout_xml;
			layout_stream >> layout_xml;
			k3d::xml::element& xml_rotate = layout_xml.safe_element("application").safe_element("user_interface").safe_element("tools").safe_element("rotate");

			m_manipulators_size = k3d::xml::attribute_value<double>(xml_rotate, "size", 10);
			m_current_color = k3d::xml::attribute_value<k3d::color>(xml_rotate, "current_color", k3d::color(1, 1, 0));
			m_x_color = k3d::xml::attribute_value<k3d::color>(xml_rotate, "x_color", k3d::color(1, 0, 0));
			m_y_color = k3d::xml::attribute_value<k3d::color>(xml_rotate, "y_color", k3d::color(0, 1, 0));
			m_z_color = k3d::xml::attribute_value<k3d::color>(xml_rotate, "z_color", k3d::color(0, 0, 1));
			m_screen_z_color = k3d::xml::attribute_value<k3d::color>(xml_rotate, "screen_z_color", k3d::color(0.9, 0.9, 0.9));
			m_handle_size = k3d::xml::attribute_value<double>(xml_rotate, "handle_size", 5);
			m_handle_major_radius = k3d::xml::attribute_value<double>(xml_rotate, "handle_major_radius", 1.0);
			m_handle_minor_radius = k3d::xml::attribute_value<double>(xml_rotate, "handle_minor_radius", 0.03);
			m_handle_u_segments = k3d::xml::attribute_value<unsigned long>(xml_rotate, "handle_u_segments", 16);
			m_handle_v_segments = k3d::xml::attribute_value<unsigned long>(xml_rotate, "handle_v_segments", 4);
			m_screen_z_handle_size = k3d::xml::attribute_value<double>(xml_rotate, "screen_z_size", 8);
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}

		set_constraint("screen_z");
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
		m_screen_z_constraint.set_plane(k3d::plane(screen_normal, Position), screen_normal);

		// Update axis constraints so their planes are always as perpendicular to the screen as-possible
		m_x_constraint.set_plane(k3d::plane((Orientation * k3d::vector3(1, 0, 0)) ^ ((Orientation * k3d::vector3(1, 0, 0)) ^ screen_normal), Position), k3d::vector3(1, 0, 0));
		m_y_constraint.set_plane(k3d::plane((Orientation * k3d::vector3(0, 1, 0)) ^ ((Orientation * k3d::vector3(0, 1, 0)) ^ screen_normal), Position), k3d::vector3(0, 1, 0));
		m_z_constraint.set_plane(k3d::plane((Orientation * k3d::vector3(0, 0, 1)) ^ ((Orientation * k3d::vector3(0, 0, 1)) ^ screen_normal), Position), k3d::vector3(0, 0, 1));

		// Draw manipulators
		if(!Visible)
			return;

		k3d::gl::store_attributes attributes;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translation3D(Position));

		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators to show them at constant size (except on mouse drag)
		if(tool_selection::MOTION_NONE == Motion)
			m_scale = detail::manipulators_scale(Viewport, Position, m_manipulators_size);

		glScaled(m_scale, m_scale, m_scale);

		// Draw manipulators
		const bool motion = (tool_selection::MOTION_DRAG == Motion) || (tool_selection::MOTION_CLICK_DRAG == Motion);

		glPushMatrix();
		k3d::gl::push_matrix(Orientation);
		if(!motion || &m_x_constraint == m_current_constraint)
			draw_handle(m_current_constraint == &m_x_constraint ? m_current_color : m_x_color, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(0, 1, 0)), m_handle_size);
		if(!motion || &m_y_constraint == m_current_constraint)
			draw_handle(m_current_constraint == &m_y_constraint ? m_current_color : m_y_color, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(1, 0, 0)), m_handle_size);
		if(!motion || &m_z_constraint == m_current_constraint)
			draw_handle(m_current_constraint == &m_z_constraint ? m_current_color : m_z_color, k3d::rotation3D(k3d::radians(0.0), k3d::vector3(1, 0, 0)), m_handle_size);
		glPopMatrix();

		if(!motion || &m_screen_z_constraint == m_current_constraint)
		{
			k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*Viewport.camera());
			// Zero translation
			screen_matrix[0][3] = screen_matrix[1][3] = screen_matrix[2][3] = 0;

			draw_handle(m_current_constraint == &m_screen_z_constraint ? m_current_color : m_screen_z_color, screen_matrix, m_screen_z_handle_size);
		}

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void select(viewport::control& Viewport, const k3d::point3& Position, const k3d::matrix4& Orientation)
	{
		k3d::gl::store_attributes attributes;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translation3D(Position));

		glDisable(GL_LIGHTING);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators
		glScaled(m_scale, m_scale, m_scale);

		// Draw manipulators
		glPushMatrix();
		k3d::gl::push_matrix(Orientation);
		select_handle(m_x_constraint, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(0, 1, 0)), m_handle_size);
		select_handle(m_y_constraint, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(1, 0, 0)), m_handle_size);
		select_handle(m_z_constraint, k3d::rotation3D(k3d::radians(0.0), k3d::vector3(1, 0, 0)), m_handle_size);
		glPopMatrix();

		k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*Viewport.camera());
		screen_matrix[0][3] = screen_matrix[1][3] = screen_matrix[2][3] = 0;
		select_handle(m_screen_z_constraint, screen_matrix, m_screen_z_handle_size);

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
		if(Name == "screen_z")
			m_current_constraint = &m_screen_z_constraint;
		else if(Name == "x_axis")
			m_current_constraint = &m_x_constraint;
		else if(Name == "y_axis")
			m_current_constraint = &m_y_constraint;
		else if(Name == "z_axis")
			m_current_constraint = &m_z_constraint;
		else
		{
			k3d::log() << error << "Unknown constraint : '" << Name << "'" << std::endl;
			assert_not_reached();
		}

		return m_current_constraint->cursor();
	}

	std::string constraint_name(const k3d::selection::id ID)
	{
		if(ID == m_screen_z_constraint.m_selection_token.id)
			return "screen_z";
		else if(ID == m_x_constraint.m_selection_token.id)
			return "x_axis";
		else if(ID == m_y_constraint.m_selection_token.id)
			return "y_axis";
		else if(ID == m_z_constraint.m_selection_token.id)
			return "z_axis";

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
			set_constraint("screen_z");
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

	void begin_mouse_move(const k3d::point2& Coords, const k3d::point3& Origin)
	{
		m_current_constraint->begin_mouse_move(Coords, Origin);
	}

	k3d::angle_axis mouse_move(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::point3& Position, const k3d::matrix4& Orientation)
	{
		return m_current_constraint->mouse_move(Viewport, Coordinates, Position, Orientation);
	}

private:
	// Manipulator drawing/selection functions
	void draw_handle(const k3d::color& Color, const k3d::matrix4& Matrix, const double Size)
	{
		k3d::gl::store_attributes attributes;
		k3d::gl::color3d(Color);

		glEnable(GL_LIGHTING);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, Color);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));
		draw_gl_handle(Matrix, Size);
	}

	void select_handle(constraint& Constraint, const k3d::matrix4& Matrix, const double Size)
	{
		k3d::gl::store_attributes attributes;
		k3d::gl::push_selection_token(Constraint.m_selection_token);

		glDisable(GL_LIGHTING);
		draw_gl_handle(Matrix, Size);

		k3d::gl::pop_selection_token();
	}

	void draw_gl_handle(const k3d::matrix4& Matrix, const double Size)
	{
		k3d::gl::store_attributes attributes;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(Matrix);

		double major_step = k3d::pi_times_2() / static_cast<double>(m_handle_u_segments);
		double minor_step = k3d::pi_times_2() / static_cast<double>(m_handle_v_segments);

		for(unsigned long i = 0; i < m_handle_u_segments; ++i)
		{
			double phi = static_cast<double>(i) * major_step;
			double phi2 = phi + major_step;
			double x0 = cos(phi) * Size;
			double y0 = sin(phi) * Size;
			double x1 = cos(phi2) * Size;
			double y1 = sin(phi2) * Size;

			glBegin(GL_TRIANGLE_STRIP);
			for(unsigned long j = 0; j <= m_handle_v_segments; ++j)
			{
				double b = static_cast<double>(j) * minor_step;
				double c = cos(b);
				double r = m_handle_minor_radius * c + m_handle_major_radius;
				double z = sin(b) * Size;

				glNormal3d(x0*c, y0*c, z);
				glVertex3d(x0*r, y0*r, z * m_handle_minor_radius);

				glNormal3d(x1*c, y1*c, z);
				glVertex3d(x1*r, y1*r, z * m_handle_minor_radius);
			}
			glEnd();
		}

		glPopMatrix();
	}

	/// Stores the constraint that was in effect when rotation began (if any)
	constraint* m_current_constraint;
	/// Stores the constraint that was in effect before m_current_constraint
	constraint* m_previous_constraint;

	// These are placeholders, used to identify hotspots in the manipulators
	constraint m_screen_z_constraint;
	constraint m_x_constraint;
	constraint m_y_constraint;
	constraint m_z_constraint;

	// Parameters that control the on-screen appearance of the tool
	double m_manipulators_size;
	double m_scale;
	k3d::color m_current_color;
	k3d::color m_x_color;
	k3d::color m_y_color;
	k3d::color m_z_color;
	k3d::color m_screen_z_color;
	double m_handle_size;
	double m_handle_major_radius;
	double m_handle_minor_radius;
	unsigned long m_handle_u_segments;
	unsigned long m_handle_v_segments;
	double m_screen_z_handle_size;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// rotate_tool

rotate_tool::rotate_tool(document_state& DocumentState, const std::string& Name) :
	transform_tool_base(DocumentState.document(), DocumentState, Name),
	m_mutex(false),
	m_rotation(init_owner(*this) + init_name("rotation") + init_label(_("Rotation")) + init_description(_("Rotation")) + init_value(k3d::angle_axis(0, 1, 0, 0))),
	m_center(init_owner(*this) + init_name("center") + init_label(_("Center")) + init_description(_("Center")) + init_value(k3d::point3(0, 0, 0)))
{
	//m_implementation->navigation_model().connect_command_signal(sigc::mem_fun(*this, &rotate_tool::record_command));
	m_rotation.connect_explicit_change_signal(sigc::mem_fun(*this, &rotate_tool::on_rotate));

	m_input_model.connect_lbutton_down(sigc::mem_fun(*this, &rotate_tool::on_lbutton_down));
	m_input_model.connect_lbutton_click(sigc::mem_fun(*this, &rotate_tool::on_lbutton_click));
	m_input_model.connect_lbutton_start_drag(sigc::mem_fun(*this, &rotate_tool::on_lbutton_start_drag));
	m_input_model.connect_lbutton_drag(sigc::mem_fun(*this, &rotate_tool::on_lbutton_drag));
	m_input_model.connect_lbutton_end_drag(sigc::mem_fun(*this, &rotate_tool::on_lbutton_end_drag));
	m_input_model.connect_mbutton_click(sigc::mem_fun(*this, &rotate_tool::on_mbutton_click));
	m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &rotate_tool::on_rbutton_click));
	m_input_model.connect_mouse_move(sigc::mem_fun(*this, &rotate_tool::on_mouse_move));

	m_manipulators = new detail::rotate_manipulators();
}

rotate_tool::~rotate_tool()
{
	delete m_manipulators;
}

const k3d::icommand_node::result rotate_tool::execute_command(const std::string& Command, const std::string& Arguments)
{
	const k3d::icommand_node::result result = navigation_model().execute_command(Command, Arguments);
	if(result != RESULT_UNKNOWN_COMMAND)
		return result;

	try
	{
		command_arguments arguments(Arguments);

		if(Command == "mouse_move")
		{
			interactive::move_pointer(arguments.get_viewport(), arguments.get_viewport_point2());
		}
		else if(Command == "mouse_warp")
		{
			interactive::warp_pointer(arguments.get_viewport(), arguments.get_viewport_point2());
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
			lmb_click_start_motion(arguments.get_viewport_point2());
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
			lmb_start_drag_start_motion(arguments.get_viewport_point2());
		}
		else if(Command == "lmb_start_drag_box_select")
		{
			lmb_start_drag_box_select(arguments.get_viewport(), arguments.get_viewport_point2());
		}
		else if(Command == "lmb_drag_move")
		{
			rotate_selection(arguments.get_angle_axis("rotation"));
			k3d::gl::redraw_all(m_document, k3d::gl::irender_engine::SYNCHRONOUS);
		}
		else if(Command == "lmb_drag_box_select")
		{
			lmb_drag_box_select(arguments.get_viewport(), arguments.get_viewport_point2());
		}
		else if(Command == "lmb_end_drag_stop_motion")
		{
			lmb_end_drag_stop_motion();
		}
		else if(Command == "lmb_end_drag_box_select")
		{
			lmb_end_drag_box_select(arguments.get_viewport(), arguments.get_viewport_point2());
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
			rotate_selection(arguments.get_angle_axis("rotation"));
			k3d::gl::redraw_all(m_document, k3d::gl::irender_engine::SYNCHRONOUS);
		}
		else
		{
			return transform_tool::execute_command(Command, Arguments);;
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

void rotate_tool::on_activate()
{
	m_manipulators->activate();

	m_visible_manipulators.set_value(true);

	on_document_selection_changed();

	tool_selection::redraw_all();
}

void rotate_tool::on_deactivate()
{
	if(MOTION_NONE != m_current_motion)
		cancel_mouse_move();

	clear_targets();

	m_document_state.clear_cursor_signal().emit();
	tool_selection::redraw_all();
}

void rotate_tool::on_document_selection_changed()
{
	target_list_changed();
	reset();

	//set_center(world_position());
}

void rotate_tool::on_redraw(viewport::control& Viewport)
{
	// Update target list
	update_targets();

	// Redraw manipulators
	m_manipulators->redraw(Viewport, (m_visible_manipulators.internal_value() && target_number()), world_position(), world_orientation(), m_current_motion, k3d::point3(1.0, 1.0, 1.0));
}

void rotate_tool::on_select(viewport::control& Viewport)
{
	if(m_visible_manipulators.internal_value() && target_number())
		m_manipulators->select(Viewport, world_position(), world_orientation());
}

k3d::iproperty_collection* rotate_tool::get_property_collection()
{
	return dynamic_cast<k3d::iproperty_collection*>(this);
}

viewport_input_model& rotate_tool::get_input_model()
{
	return input_model();
}

void rotate_tool::record_command(viewport::control& Viewport, const GdkEventButton& Event, const bool Move)
{
	command_arguments arguments;
	arguments.append_viewport_coordinates("mouse", Viewport, Event);

	if(Move)
		transform_tool::record_command("mouse_move", arguments);

	transform_tool::record_command(m_tutorial_action, arguments);
	m_tutorial_action = "";
}

void rotate_tool::record_transform(viewport::control& Viewport, const GdkEventMotion& Event, const k3d::angle_axis& Rotation)
{
	command_arguments arguments;
	arguments.append_viewport_coordinates("mouse", Viewport, Event);

	transform_tool::record_command("mouse_warp", arguments);

	arguments.append("rotation", Rotation);
	transform_tool::record_command(m_tutorial_action, arguments);
	m_tutorial_action = "";
}

// LMB drag actions
k3d::angle_axis rotate_tool::mouse_move_to_3d(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	// Transform mouse move to a world rotation
	const k3d::point3 origin = world_position();
	const k3d::matrix4 orientation = world_orientation();
	const k3d::angle_axis rotation = m_manipulators->mouse_move(Viewport, Coordinates, origin, orientation);

	return rotation;
}

k3d::angle_axis rotate_tool::lbutton_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_DRAG == m_current_motion)
	{
		m_tutorial_action = "lmb_drag_move";

		const k3d::angle_axis rotation = mouse_move_to_3d(Viewport, Coordinates);
		rotate_selection(rotation);

		return rotation;
	}

	if(MOTION_BOX_SELECT == m_current_motion)
		lmb_drag_box_select(Viewport, Coordinates);

	return k3d::angle_axis(0, 1, 0, 0);
}

k3d::angle_axis rotate_tool::mouse_move_action(viewport::control& Viewport, const k3d::point2& Coordinates)
{
	if(MOTION_CLICK_DRAG == m_current_motion)
	{
		m_tutorial_action = "mouse_drag_move";

		const k3d::angle_axis rotation = mouse_move_to_3d(Viewport, Coordinates);
		rotate_selection(rotation);

		return rotation;
	}

	return k3d::angle_axis(0, 1, 0, 0);
}

void rotate_tool::rotate_selection(const k3d::angle_axis& Rotation)
{
	if(!Rotation.angle)
		return;

	m_rotation.set_value(Rotation);
}

k3d::point3 rotate_tool::get_center()
{
	return world_position();
}

void rotate_tool::on_rotate(k3d::iunknown*)
{
	if(m_mutex)
		return;

	rotate_targets(k3d::rotation3D(m_rotation.internal_value()));
}

} // namespace libk3dngui

