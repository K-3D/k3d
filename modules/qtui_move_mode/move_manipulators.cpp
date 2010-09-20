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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/ibounded.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/share.h>
#include <k3dsdk/snap_target.h>
#include <k3dsdk/snappable.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/vectors.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace qtui
{

namespace move
{

/////////////////////////////////////////////////////////////////////////////
// move_manipulators

/// Provides a configurable set of move_manipulators to assist users in visualizing the 3D workspace
class move_manipulators :
	public k3d::gl::renderable<k3d::transformable<k3d::node> >
{
	typedef k3d::gl::renderable<k3d::transformable<k3d::node> > base;
	
	enum selection_id {XX, YY, ZZ, XY, XZ, YZ, SC};

public:
	move_manipulators(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_position(init_owner(*this) + init_name("position") + init_label(_("Position")) + init_description(_("Current position of the manipulator.")) + init_value(k3d::point3(0, 0, 0))),
		m_size(init_owner(*this) + init_name("size") + init_label(_("Size")) + init_description(_("Size of the manipulator.")) + init_value(10.)),
		m_x_constraint(init_owner(*this) + init_name("x_constraint") + init_label(_("X Constraint")) + init_description(_("Allow motion along the local X axis.")) + init_value(true)),
		m_y_constraint(init_owner(*this) + init_name("y_constraint") + init_label(_("Y Constraint")) + init_description(_("Allow motion along the local Y axis.")) + init_value(true)),
		m_z_constraint(init_owner(*this) + init_name("z_constraint") + init_label(_("Z Constraint")) + init_description(_("Allow motion along the local Z axis.")) + init_value(true)),
		m_x_axis(init_owner(*this) + init_name("x_axis") + init_label(_("X Axis")) + init_description(_("Direction of the local X axis.")) + init_value(k3d::vector3(1.,0.,0.))),
		m_y_axis(init_owner(*this) + init_name("y_axis") + init_label(_("Y Axis")) + init_description(_("Direction of the local Y axis.")) + init_value(k3d::vector3(0.,1.,0.))),
		m_z_axis(init_owner(*this) + init_name("z_axis") + init_label(_("Z Axis")) + init_description(_("Direction of the local Z axis.")) + init_value(k3d::vector3(0.,0.,1.))),
		m_x_color(init_owner(*this) + init_name("x_color") + init_label(_("X Color")) + init_description(_("Color for the X axis.")) + init_value(k3d::color(1.,0.,0.))),
		m_y_color(init_owner(*this) + init_name("y_color") + init_label(_("Y Color")) + init_description(_("Color for the Y axis.")) + init_value(k3d::color(0.,1.,0.))),
		m_z_color(init_owner(*this) + init_name("z_color") + init_label(_("Z Color")) + init_description(_("Color for the Z axis.")) + init_value(k3d::color(0.,0.,1.))),
		m_selected_color(init_owner(*this) + init_name("selected_color") + init_label(_("Selected Color")) + init_description(_("Color for the selected axis.")) + init_value(k3d::color(1.,1.,0.))),
		m_quadric(gluNewQuadric()),
		m_axis_arrow_radius(0.5),
		m_axis_arrow_length(2.),
		m_axis_arrow_slices(8),
		m_axis_line_width(1.)
	{
	}
	
  ~move_manipulators()
	{
		gluDeleteQuadric(m_quadric);
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		gl_draw_style style(m_quadric, m_axis_line_width);
		draw_manipulators(State, style);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::log() << debug << "selecting move manips" << std::endl;
		gl_select_style style(m_quadric, m_axis_line_width);
		draw_manipulators(State, style);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<move_manipulators,
			k3d::interface_list<k3d::imatrix_source,
			k3d::interface_list<k3d::imatrix_sink> > >factory(
			k3d::uuid(0xaae19408, 0x270d47ab, 0x8dcffa3f, 0xd0717608),
			"MoveManipulators",
			_("Configurable set of move_manipulators"),
			"Annotation",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	
	k3d::double_t scale(const k3d::gl::render_state& State, const k3d::point3& Origin)
	{
		// Project unit axis on screen space
		const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(State.camera);
		const k3d::vector3 screen_parallel = screen_matrix * k3d::vector3(1, 0, 0);
		const k3d::point3 position = State.project(Origin);
		const k3d::point3 x_axis = State.project(Origin + screen_parallel);
		const k3d::vector2 vec(x_axis[0] - position[0], x_axis[1] - position[1]);
		const k3d::double_t length = vec.length();

		return_val_if_fail(length, 0);

		return m_size.pipeline_value() / length;
	}
	
	template<typename gl_style_t>
	void draw_manipulators(const k3d::gl::render_state& State, gl_style_t& Style)
	{
		k3d::gl::store_attributes attributes;
		
		const k3d::point3 position = m_position.pipeline_value();
		
		// Update constraints
		const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(State.camera);
		const k3d::vector3 screen_normal = screen_matrix * k3d::vector3(0, 0, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translate3(k3d::to_vector(position)));

		glClear(GL_DEPTH_BUFFER_BIT);

		// Scale manipulators to show them at constant size (except on mouse drag)
		//if(tool_selection::MOTION_NONE == Motion)
		const k3d::double_t scale_factor = scale(State, position);

		glScaled(scale_factor, scale_factor, scale_factor);

		// Draw manipulators
		const k3d::vector3 px = k3d::normalize(m_x_axis.pipeline_value());
		const k3d::vector3 py = k3d::normalize(m_y_axis.pipeline_value());
		const k3d::vector3 pz = k3d::normalize(m_z_axis.pipeline_value());

		//const bool motion = (tool_selection::MOTION_DRAG == Motion) || (tool_selection::MOTION_CLICK_DRAG == Motion);
		const k3d::bool_t motion = false;
		
		if(!motion || m_x_constraint.pipeline_value())
		{
			Style.push_selection_token(XX);
			draw_axis(m_x_color.pipeline_value(), px, false, Style);
			Style.pop_selection_token();
		}

		if(!motion || m_y_constraint.pipeline_value())
		{
			Style.push_selection_token(YY);
			draw_axis(m_y_color.pipeline_value(), py, false, Style);
			Style.pop_selection_token();
		}

		if(!motion || m_z_constraint.pipeline_value())
		{
			Style.push_selection_token(ZZ);
			draw_axis(m_z_color.pipeline_value(), pz, false, Style);
			Style.pop_selection_token();
		}

// 		if(!motion || (m_y_constraint.pipeline_value() && m_z_constraint.pipeline_value()))
// 			draw_plane(m_current_constraint == &m_yz_constraint ? m_current_color : m_x_color, py, py + pz, pz);
// 		if(!motion || (m_x_constraint.pipeline_value() && m_z_constraint.pipeline_value()))
// 			draw_plane(m_current_constraint == &m_xz_constraint ? m_current_color : m_y_color, px, px + pz, pz);
// 		if(!motion || (m_x_constraint.pipeline_value() && m_y_constraint.pipeline_value()))
// 			draw_plane(m_current_constraint == &m_xy_constraint ? m_current_color : m_z_color, px, px + py, py);

		//if(!motion || &m_screen_xy_constraint == m_current_constraint)
		//	draw_screen_xy(Viewport, m_current_constraint == &m_screen_xy_constraint ? m_current_color : m_screen_xy_color);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	
	template<typename gl_style_t>
	void draw_axis(const k3d::color& Color, const k3d::vector3& Vector, const k3d::bool_t Selected, gl_style_t& Style)
	{
		k3d::gl::store_attributes attributes;
		
		const k3d::vector3 axis_vector = Vector * m_size.pipeline_value();
		const k3d::point3 position = m_position.pipeline_value();

		// Draw the arrow head
		glMatrixMode(GL_MODELVIEW);
		
		// Translation of the head
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translate3(axis_vector - (0.5 * m_axis_arrow_length * Vector)));
		
		// Orientation for the head
		const k3d::vector3 head_rotation_axis = (Vector[0] || Vector[1]) ? k3d::vector3(0., 0., 1.) ^ Vector : k3d::vector3(0., 0., 1.);
		const k3d::double_t head_rotation_angle = (Vector[0] || Vector[1]) ? asin(head_rotation_axis.length()) : 0.;
		glPushMatrix();
		k3d::gl::push_matrix(k3d::rotate3(head_rotation_angle, head_rotation_axis));

		Style.set_arrow_style(Color);
		gluCylinder(m_quadric, m_axis_arrow_radius, m_axis_arrow_radius * 0.001, m_axis_arrow_length, m_axis_arrow_slices, 1);
		
		glPopMatrix();
		glPopMatrix();

		Style.set_axis_style();
		k3d::gl::color3d(Selected ? m_selected_color.pipeline_value() : Color);
		
		glBegin(GL_LINES);
			k3d::gl::vertex3d(position);
			k3d::gl::vertex3d(position + axis_vector);
		glEnd();
	}
	
	/// Sets the style for drawing mode
	struct gl_draw_style
	{
		gl_draw_style(GLUquadric* Quadric, const k3d::double_t AxisLineWidth) : m_quadric(Quadric), m_axis_line_width(AxisLineWidth)
		{
		}
		
		/// Style for the arrow head
		void set_arrow_style(const k3d::color& Color)
		{
			glEnable(GL_LIGHTING);
			k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
			k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, Color);
			k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
			k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));
			gluQuadricDrawStyle(m_quadric, GLU_FILL);
			gluQuadricNormals(m_quadric, GLU_SMOOTH);
		}
		
		/// Style for the axis
		void set_axis_style()
		{
			glDisable(GL_LIGHTING);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(m_axis_line_width);
		}
		
		/// Handle selection, if applicable
		void push_selection_token(const GLuint ID) {}
		void pop_selection_token() {}
		
		GLUquadric* m_quadric;
		const k3d::double_t m_axis_line_width;
	};
	
	/// Sets the style for drawing mode
	struct gl_select_style
	{
		gl_select_style(GLUquadric* Quadric, const k3d::double_t AxisLineWidth) : m_quadric(Quadric), m_axis_line_width(AxisLineWidth)
		{
		}
		
		/// Style for the arrow head
		void set_arrow_style(const k3d::color& Color)
		{
			glDisable(GL_LIGHTING);
			gluQuadricDrawStyle(m_quadric, GLU_FILL);
			gluQuadricNormals(m_quadric, GLU_NONE);
		}
		
		/// Style for the axis
		void set_axis_style()
		{
			glDisable(GL_LIGHTING);
			glLineWidth(m_axis_line_width);
		}
		
		/// Handle selection, if applicable
		void push_selection_token(const GLuint ID)
		{
			k3d::gl::push_selection_token(k3d::selection::USER1, ID);
		}
		void pop_selection_token()
		{
			k3d::gl::pop_selection_token();
		}
		
		GLUquadric* m_quadric;
		const k3d::double_t m_axis_line_width;
	};
	
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_position;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_size;
	
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_x_constraint;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_y_constraint;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_z_constraint;
	
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_x_axis;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_y_axis;
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_z_axis;
	
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_x_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_y_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_z_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_selected_color;
	
	GLUquadric* m_quadric;
	
	// TODO: These should be properties
	k3d::double_t m_axis_arrow_radius;
	k3d::double_t m_axis_arrow_length;
	k3d::int32_t m_axis_arrow_slices;
	k3d::double_t m_axis_line_width;
};

/////////////////////////////////////////////////////////////////////////////
// move_manipulators_factory

k3d::iplugin_factory& move_manipulators_factory()
{
	return move_manipulators::get_factory();
}

} // namespace move

} // namespace qtui

} // namespace module

