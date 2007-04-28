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

#include "basic_input_model.h"
#include "command_arguments.h"
#include "document_state.h"
#include "modifiers.h"
#include "icons.h"
#include "interactive.h"
#include "keyboard.h"
#include "snap_tool.h"
#include "snap_tool_detail.h"
#include "viewport.h"

#include <k3dsdk/color.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/isnappable.h>
#include <k3dsdk/isnap_source.h>
#include <k3dsdk/isnap_target.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/property.h>
#include <k3dsdk/share.h>
#include <k3dsdk/xml.h>

#include <k3dsdk/fstream.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// snap_tool::implementation

struct snap_tool::implementation :
	public detail::abstract_tool,
	public snap_tool_detail
{
	typedef snap_tool_detail base;

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

		void begin_mouse_move(const k3d::point2& Mouse)
		{
			m_last_mouse = Mouse;
		}

		k3d::vector3 mouse_move(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::matrix4& Orientation)
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
			const k3d::vector3 delta = k3d::inverse(Orientation) * k3d::to_vector(current_intersection - last_intersection);

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

	implementation(k3d::idocument& Document, document_state& DocumentState, snap_tool& MoveTool) :
		base(Document, DocumentState, *this),
		m_snap_tool(MoveTool),
		m_quadric(gluNewQuadric()),
		m_current_constraint(&m_screen_xy_constraint),
		m_screen_xy_constraint(_("Move Screen XY"), load_icon("move_cursor_screen_xy", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 0)),
		m_x_constraint(_("Move X"), load_icon("move_cursor_x", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 1)),
		m_y_constraint(_("Move Y"), load_icon("move_cursor_y", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 2)),
		m_z_constraint(_("Move Z"), load_icon("move_cursor_z", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 3)),
		m_xy_constraint(_("Move XY"), load_icon("move_cursor_xy", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 4)),
		m_xz_constraint(_("Move XZ"), load_icon("move_cursor_xz", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 5)),
		m_yz_constraint(_("Move YZ"), load_icon("move_cursor_yz", Gtk::ICON_SIZE_BUTTON), k3d::selection::token(k3d::selection::USER1, 6)),
		m_transformation(init_owner(*this) + init_name("transformation") + init_label(_("Transformation")) + init_description(_("Transformation")) + init_value(k3d::identity3D())),
		m_target(init_owner(*this) + init_name("target") + init_label(_("Target")) + init_description(_("Target")) + init_value(static_cast<k3d::isnappable*>(0))),
		m_snap_target(init_owner(*this) + init_name("snap_target") + init_label(_("Snap Target")) + init_description(_("Snap Target")) + init_value(std::string("")) + init_values(m_snap_targets)),
		m_snap_distance(init_owner(*this) + init_name("snap_distance") + init_label(_("Snap Distance")) + init_description(_("Snap Distance")) + init_value(5.0)),
		m_snap_orientation(init_owner(*this) + init_name("snap_orientation") + init_label(_("Snap Orientation")) + init_description(_("Snap Orientation")) + init_value(true)),
		m_match_groups(init_owner(*this) + init_name("match_groups") + init_label(_("Match Groups")) + init_description(_("Match Groups")) + init_value(true))
	{
		m_transformation.connect_explicit_change_signal(sigc::mem_fun(*this, &implementation::on_move));

		m_input_model.connect_lbutton_down(sigc::mem_fun(*this, &implementation::on_lbutton_down));
		m_input_model.connect_lbutton_click(sigc::mem_fun(*this, &implementation::on_lbutton_click));
		m_input_model.connect_lbutton_start_drag(sigc::mem_fun(*this, &implementation::on_lbutton_start_drag));
		m_input_model.connect_lbutton_drag(sigc::mem_fun(*this, &implementation::on_lbutton_drag));
		m_input_model.connect_lbutton_end_drag(sigc::mem_fun(*this, &implementation::on_lbutton_end_drag));
		m_input_model.connect_mbutton_click(sigc::mem_fun(*this, &implementation::on_mbutton_click));
		m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &implementation::on_rbutton_click));
		m_input_model.connect_mouse_move(sigc::mem_fun(*this, &implementation::on_mouse_move));

		on_target_changed(0);
		m_target.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_target_changed));
	}

	~implementation()
	{
		gluDeleteQuadric(m_quadric);
	}

	void on_target_changed(k3d::iunknown*)
	{
		m_snap_targets.clear();
		m_snap_targets.push_back(k3d::ienumeration_property::enumeration_value_t("-- Automatic --", "", "-- Automatic --"));

		if(k3d::isnappable* const snappable = m_target.value())
		{
			const k3d::isnappable::snap_targets_t targets = snappable->snap_targets();

			unsigned long count = 0;
			for(k3d::isnappable::snap_targets_t::const_iterator target = targets.begin(); target != targets.end(); ++target)
			{
				m_snap_targets.push_back(
					k3d::ienumeration_property::enumeration_value_t(
						(*target)->label(), k3d::string_cast(count++), (*target)->label()));
			}
		}

		m_snap_target.notify_enumeration_values_changed();
	}

	void on_activate()
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

		m_visible_manipulators.set_value(true);
		set_constraint(&m_screen_xy_constraint);
		on_document_selection_changed();

		redraw_all();
	}

	void on_deactivate()
	{
		if(MOTION_NONE != m_current_motion)
			cancel_mouse_move();

		clear_targets();

		m_document_state.clear_cursor_signal().emit();
		redraw_all();
	}

	void on_document_selection_changed()
	{
		get_current_selection();
		reset();
	}

	void record_command(viewport::control& Viewport, const GdkEventButton& Event, const bool Move)
	{
		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);

		if(Move)
			m_snap_tool.record_command("mouse_move", arguments);

		m_snap_tool.record_command(m_tutorial_action, arguments);
		m_tutorial_action = "";
	}

	void record_transform(viewport::control& Viewport, const GdkEventMotion& Event, const k3d::vector3& Move)
	{
		command_arguments arguments;
		arguments.append_viewport_coordinates("mouse", Viewport, Event);

		m_snap_tool.record_command("mouse_warp", arguments);

		arguments.append("move", Move);
		m_snap_tool.record_command(m_tutorial_action, arguments);
		m_tutorial_action = "";
	}

	void on_lbutton_down(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);
		const k3d::key_modifiers modifiers = convert(Event.state);

		lbutton_down(Viewport, coordinates, modifiers);

		// Record command for tutorials
		record_command(Viewport, Event, true);
	}

	void on_lbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_click(Viewport, coordinates);

		// Record command for tutorials
		record_command(Viewport, Event, false);
	}

	void on_lbutton_start_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_start_drag(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, k3d::vector3(0, 0, 0));
	}

	void on_lbutton_drag(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		k3d::point2 coordinates(Event.x, Event.y);

		// Off-screen wrap
		off_screen_warp(Viewport, coordinates);

		const k3d::vector3 move = lbutton_drag(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, move);
	}

	void on_lbutton_end_drag(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_end_drag(Viewport, coordinates);

		// Record command for tutorials
		record_command(Viewport, Event, false);
	}

	void on_mbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);
		const k3d::key_modifiers modifiers = convert(Event.state);

		mbutton_click(Viewport, coordinates, modifiers);

		// Record command for tutorials
		record_command(Viewport, Event, true);
	}

	void on_rbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		rbutton_click(Viewport, coordinates);

		// Record command for tutorials
		record_command(Viewport, Event, true);
	}

	void on_mouse_move(viewport::control& Viewport, const GdkEventMotion& Event)
	{
		if(MOTION_CLICK_DRAG != m_current_motion)
			return;

		k3d::point2 coordinates(Event.x, Event.y);

		// Off-screen wrap
		off_screen_warp(Viewport, coordinates);

		const k3d::vector3 move = mouse_move_action(Viewport, coordinates);

		// Record command for tutorials
		record_transform(Viewport, Event, move);
	}

	void on_redraw(viewport::control& Viewport)
	{
		// Sanity checks
		return_if_fail(Viewport.gl_engine());
		return_if_fail(Viewport.camera());

		// Update target list
		update_targets();

		// Update constraints
		const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*Viewport.camera());
		const k3d::vector3 screen_normal = screen_matrix * k3d::vector3(0, 0, 1);

		const k3d::point3 origin = m_transformation.value() * k3d::point3();
		const k3d::matrix4 orientation = k3d::identity3D();

		// Update the screen xy constraint so it always aligns with the camera direction vector in world coordinates
		m_screen_xy_constraint.set_plane(k3d::plane(screen_normal, origin), 1, 1, 1);

		// Update axis constraints so their planes are always as perpendicular to the screen as-possible
		m_x_constraint.set_plane(k3d::plane((orientation * k3d::vector3(1, 0, 0)) ^ ((orientation * k3d::vector3(1, 0, 0)) ^ screen_normal), origin), 1, 0, 0);
		m_y_constraint.set_plane(k3d::plane((orientation * k3d::vector3(0, 1, 0)) ^ ((orientation * k3d::vector3(0, 1, 0)) ^ screen_normal), origin), 0, 1, 0);
		m_z_constraint.set_plane(k3d::plane((orientation * k3d::vector3(0, 0, 1)) ^ ((orientation * k3d::vector3(0, 0, 1)) ^ screen_normal), origin), 0, 0, 1);

		// Update plane constraints so they have the correct positions in world coordinates
		m_xy_constraint.set_plane(k3d::plane(orientation * k3d::vector3(0, 0, 1), origin), 1, 1, 0);
		m_xz_constraint.set_plane(k3d::plane(orientation * k3d::vector3(0, 1, 0), origin), 1, 0, 1);
		m_yz_constraint.set_plane(k3d::plane(orientation * k3d::vector3(1, 0, 0), origin), 0, 1, 1);

		// Draw manipulators
		if(!m_visible_manipulators.value() || !target_number())
			return;

		k3d::gl::store_attributes attributes;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translation3D(origin));
		k3d::gl::push_matrix(orientation);

		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators to show them at constant size (except on mouse drag)
		if(m_current_motion == MOTION_NONE)
			update_manipulators_scale(Viewport, origin);

		glScaled(m_manipulators_scale, m_manipulators_scale, m_manipulators_scale);

		// Draw manipulators
		k3d::vector3 px;
		k3d::vector3 py;
		k3d::vector3 pz;

		const bool motion = (MOTION_DRAG == m_current_motion) || (MOTION_CLICK_DRAG == m_current_motion);

		if(!motion || &m_x_constraint == m_current_constraint || &m_xy_constraint == m_current_constraint || &m_xz_constraint == m_current_constraint)
		{
			if(front_facing(Viewport, k3d::vector3(1, 0, 0), origin))
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
			if(front_facing(Viewport, k3d::vector3(0, -1, 0), origin))
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
			if(front_facing(Viewport, k3d::vector3(0, 0, 1), origin))
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

		if(!motion || &m_screen_xy_constraint == m_current_constraint)
			draw_screen_xy(Viewport, m_current_constraint == &m_screen_xy_constraint ? m_current_color : m_screen_xy_color);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void on_select(viewport::control& Viewport)
	{
		if(!m_visible_manipulators.value() || !target_number())
			return;

		const k3d::point3 origin = m_transformation.value() * k3d::point3();
		const k3d::matrix4 orientation = k3d::identity3D();

		k3d::gl::store_attributes attributes;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translation3D(origin));
		k3d::gl::push_matrix(orientation);

		glDisable(GL_LIGHTING);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators
		glScaled(m_manipulators_scale, m_manipulators_scale, m_manipulators_scale);

		// Draw manipulators
		k3d::vector3 px;
		k3d::vector3 py;
		k3d::vector3 pz;

		if(front_facing(Viewport, k3d::vector3(1, 0, 0), origin))
		{
			px = k3d::vector3(1, 0, 0);
			select_axis(m_x_constraint, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(0, 1, 0)));
		}
		else
		{
			px = k3d::vector3(-1, 0, 0);
			select_axis(m_x_constraint, k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(0, 1, 0)));
		}

		if(front_facing(Viewport, k3d::vector3(0, -1, 0), origin))
		{
			py = k3d::vector3(0, -1, 0);
			select_axis(m_y_constraint, k3d::rotation3D(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
		}
		else
		{
			py = k3d::vector3(0, 1, 0);
			select_axis(m_y_constraint, k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(1, 0, 0)));
		}

		if(front_facing(Viewport, k3d::vector3(0, 0, 1), origin))
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

		select_screen_xy(m_screen_xy_constraint);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
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
				// TODO : show SHIFT key for tutorials
				lmb_down_add();
			}
			else if(Command == "lmb_down_subtract")
			{
				// TODO : show CONTROL key for tutorials
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
				move_selection(arguments.get_vector3("move"));
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
				move_selection(arguments.get_vector3("move"));
				k3d::gl::redraw_all(m_document, k3d::gl::irender_engine::SYNCHRONOUS);
			}
			else
			{
				return RESULT_UNKNOWN_COMMAND;
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

private:
	// abstract_tool implementation
	virtual std::string manipulator_name(const k3d::selection::id ID)
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

	virtual std::string get_manipulator(const manipulators_t& Manipulators)
	{
		if(!Manipulators.size())
			return std::string("");

		// Give priority to the middle sphere since the invisible planes intersect it
		if(std::find(Manipulators.begin(), Manipulators.end(), std::string("screen_xy"))
			!= Manipulators.end())
			return std::string("screen_xy");

		return *Manipulators.begin();
	}

	// Set manipulator constraint
	virtual void set_manipulator(const std::string ManipulatorName)
	{
		if(ManipulatorName == "screen_xy")
			set_constraint(&m_screen_xy_constraint);
		else if(ManipulatorName == "x_axis")
			set_constraint(&m_x_constraint);
		else if(ManipulatorName == "y_axis")
			set_constraint(&m_y_constraint);
		else if(ManipulatorName == "z_axis")
			set_constraint(&m_z_constraint);
		else if(ManipulatorName == "xy_plane")
			set_constraint(&m_xy_constraint);
		else if(ManipulatorName == "xz_plane")
			set_constraint(&m_xz_constraint);
		else if(ManipulatorName == "yz_plane")
			set_constraint(&m_yz_constraint);
		else
			assert_not_reached();
	}

	virtual std::string get_constraint_name()
	{
		return_val_if_fail(m_current_constraint, "");

		return m_current_constraint->label();
	}

	virtual void begin_mouse_move(const k3d::point2& Coordinates)
	{
		return_if_fail(m_current_constraint);

		start_transform();
		m_current_constraint->begin_mouse_move(Coordinates);
	}

	void set_constraint(constraint* const Constraint)
	{
		return_if_fail(Constraint);

		m_current_constraint = Constraint;
		m_document_state.set_cursor_signal().emit(m_current_constraint->cursor());

		redraw_all();
	}

	constraint* cycle_constraint(constraint* Constraint)
	{
		if(Constraint == &m_x_constraint)
			return &m_y_constraint;

		if(Constraint == &m_y_constraint)
			return &m_z_constraint;

		if(Constraint == &m_z_constraint)
			return &m_x_constraint;

		return 0;
	}

	virtual void update_constraint(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		const k3d::point3 origin = m_transformation.value() * k3d::point3();

		// Cycle through X - Screen - Y - Screen - Z - Screen - X - etc
		if(m_current_constraint == &m_x_constraint || m_current_constraint == &m_y_constraint || m_current_constraint == &m_z_constraint)
		{
			// Save current constraint
			m_previous_constraint = m_current_constraint;

			// Go back to screeen mode
			set_constraint(&m_screen_xy_constraint);
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
			const k3d::point2 coords = Viewport.project(origin);
			const k3d::matrix4 orientation = k3d::identity3D();

			std::map<double, constraint*> constraints;
			constraints.insert(std::make_pair(k3d::distance(mouse, k3d::line2(coords, Viewport.project(origin + (orientation * k3d::vector3(1, 0, 0))))), &m_x_constraint));
			constraints.insert(std::make_pair(k3d::distance(mouse, k3d::line2(coords, Viewport.project(origin + (orientation * k3d::vector3(0, 1, 0))))), &m_y_constraint));
			constraints.insert(std::make_pair(k3d::distance(mouse, k3d::line2(coords, Viewport.project(origin + (orientation * k3d::vector3(0, 0, 1))))), &m_z_constraint));

			set_constraint(constraints.begin()->second);
		}

		begin_mouse_move(Coordinates);
	}

	virtual void reset()
	{
		m_transformation.set_value(k3d::identity3D());
	}

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
		k3d::gl::push_matrix(k3d::translation3D(k3d::point3(0, 0, m_axis_end - (0.5 * m_axis_arrow_length))));
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
		k3d::gl::push_matrix(k3d::translation3D(k3d::point3(0, 0, m_axis_end - (0.5 * m_axis_arrow_length))));
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

	// LMB drag actions
	k3d::vector3 mouse_move_to_3d(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		return_val_if_fail(m_current_constraint, k3d::vector3(0, 0, 0));

		// Transform mouse move to a world move
		const k3d::matrix4 orientation = k3d::identity3D();
		const k3d::vector3 delta = m_current_constraint->mouse_move(Viewport, Coordinates, orientation);

		return delta;
	}

	k3d::vector3 lbutton_drag(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		if(MOTION_DRAG == m_current_motion)
		{
			m_tutorial_action = "lmb_drag_move";

			const k3d::vector3 delta = mouse_move_to_3d(Viewport, Coordinates);
			move_selection(delta);

			return delta;
		}

		if(MOTION_BOX_SELECT == m_current_motion)
			lmb_drag_box_select(Viewport, Coordinates);

		return k3d::vector3(0, 0, 0);
	}

	k3d::vector3 mouse_move_action(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		if(MOTION_CLICK_DRAG == m_current_motion)
		{
			m_tutorial_action = "mouse_drag_move";

			const k3d::vector3 delta = mouse_move_to_3d(Viewport, Coordinates);
			move_selection(delta);

			return delta;
		}

		return k3d::vector3(0, 0, 0);
	}

	void move_selection(const k3d::vector3& Delta)
	{
		if(Delta == k3d::vector3(0, 0, 0))
			return;

		m_transformation.set_value(k3d::translation3D(Delta) * m_transformation.value());
	}

	void on_move(k3d::iunknown*)
	{
		k3d::isnap_target* snap_target = 0;

		if(k3d::isnappable* const snappable = m_target.value())
		{
			const std::string snap_target_string = m_snap_target.value();
			if(!snap_target_string.empty())
			{
				const unsigned long snap_target_id = k3d::from_string<unsigned long>(snap_target_string, 0);
				const k3d::isnappable::snap_targets_t targets = snappable->snap_targets();

				if(snap_target_id < targets.size())
					snap_target = targets[snap_target_id];
			}
		}

		transform_targets(m_target.value(), snap_target, m_snap_distance.value(), m_snap_orientation.value(), m_match_groups.value(), m_transformation.value());
		redraw_all();
	}

	/// Stores a back-pointer to our parent
	snap_tool& m_snap_tool;
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

	k3d_data(k3d::matrix4, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_transformation;
	k3d_data(k3d::isnappable*, immutable_name, change_signal, no_undo, node_storage, no_constraint, node_property, no_serialization) m_target;
	k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_snap_target;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_snap_distance;
	k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_snap_orientation;
	k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_match_groups;

	k3d::ienumeration_property::enumeration_values_t m_snap_sources;
	k3d::ienumeration_property::enumeration_values_t m_snap_targets;
};

/////////////////////////////////////////////////////////////////////////////
// snap_tool

snap_tool::snap_tool(document_state& DocumentState, const std::string& Name) :
	base(DocumentState, Name),
	m_implementation(new implementation(DocumentState.document(), DocumentState, *this))
{
	m_implementation->navigation_model().connect_command_signal(sigc::mem_fun(*this, &snap_tool::record_command));
}

snap_tool::~snap_tool()
{
	delete m_implementation;
}

void snap_tool::on_activate()
{
	m_implementation->on_activate();
}

void snap_tool::on_deactivate()
{
	m_implementation->on_deactivate();
}

void snap_tool::on_document_selection_changed()
{
	m_implementation->on_document_selection_changed();
}

const k3d::icommand_node::result snap_tool::execute_command(const std::string& Command, const std::string& Arguments)
{
	k3d::icommand_node::result result = m_implementation->navigation_model().execute_command(Command, Arguments);
	if(result != RESULT_UNKNOWN_COMMAND)
		return result;

	result = m_implementation->execute_command(Command, Arguments);
	if(result != RESULT_UNKNOWN_COMMAND)
		return result;

	return base::execute_command(Command, Arguments);
}

void snap_tool::on_redraw(viewport::control& Viewport)
{
	m_implementation->on_redraw(Viewport);
}

void snap_tool::on_select(viewport::control& Viewport)
{
	m_implementation->on_select(Viewport);
}

k3d::iproperty_collection* snap_tool::get_property_collection()
{
	return dynamic_cast<k3d::iproperty_collection*>(m_implementation);
}

iuser_input_model& snap_tool::get_input_model()
{
	return m_implementation->input_model();
}

} // namespace libk3dngui

