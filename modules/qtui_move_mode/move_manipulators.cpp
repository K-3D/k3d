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
#include <k3dsdk/measurement.h>
#include <k3dsdk/share.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>
#include <k3dsdk/vectors.h>

#include "move_manipulators.h"

namespace module
{

namespace qtui
{

namespace move
{

/////////////////////////////////////////////////////////////////////////////
// move_manipulators

move_manipulators::move_manipulators() :
		m_position(init_value(k3d::point3(0, 0, 0))),
		m_size(init_value(10.)),
		m_x_axis(init_value(k3d::vector3(1.,0.,0.))),
		m_y_axis(init_value(k3d::vector3(0.,1.,0.))),
		m_z_axis(init_value(k3d::vector3(0.,0.,1.))),
		m_x_color(init_value(k3d::color(1.,0.,0.))),
		m_y_color(init_value(k3d::color(0.,1.,0.))),
		m_z_color(init_value(k3d::color(0.,0.,1.))),
		m_selected_color(init_value(k3d::color(1.,1.,0.))),
		m_camera(init_value(static_cast<k3d::icamera*>(0))),
		m_quadric(gluNewQuadric()),
		m_axis_arrow_radius(0.5),
		m_axis_arrow_length(2.),
		m_axis_arrow_slices(8),
		m_axis_line_width(1.)
{
}

move_manipulators::~move_manipulators()
{
	gluDeleteQuadric(m_quadric);
}

void move_manipulators::draw(const constraints_t Constraints)
{
	k3d::gl::store_attributes attributes;
		
	const k3d::point3 position = m_position.internal_value();
	
	// Update constraints
	const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*m_camera.internal_value());
	const k3d::vector3 screen_normal = screen_matrix * k3d::vector3(0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	k3d::gl::push_matrix(k3d::translate3(k3d::to_vector(position)));

	glClear(GL_DEPTH_BUFFER_BIT);

	// Scale manipulators to show them at constant size (except on mouse drag)
	//if(tool_selection::MOTION_NONE == Motion)
	const k3d::double_t scale_factor = scale(position);

	glScaled(scale_factor, scale_factor, scale_factor);

	// Draw manipulators
	const k3d::vector3 px = k3d::normalize(m_x_axis.internal_value());
	const k3d::vector3 py = k3d::normalize(m_y_axis.internal_value());
	const k3d::vector3 pz = k3d::normalize(m_z_axis.internal_value());

	//const bool motion = (tool_selection::MOTION_DRAG == Motion) || (tool_selection::MOTION_CLICK_DRAG == Motion);
	const k3d::bool_t motion = false;
	
	if(!motion || Constraints == XX)
	{
		draw_axis(m_x_color.internal_value(), px, false);
	}

	if(!motion || Constraints == YY)
	{
		draw_axis(m_y_color.internal_value(), py, false);
	}

	if(!motion || Constraints == ZZ)
	{
		draw_axis(m_z_color.internal_value(), pz, false);
	}

// 		if(!motion || (m_y_constraint.internal_value() && m_z_constraint.internal_value()))
// 			draw_plane(m_current_constraint == &m_yz_constraint ? m_current_color : m_x_color, py, py + pz, pz);
// 		if(!motion || (m_x_constraint.internal_value() && m_z_constraint.internal_value()))
// 			draw_plane(m_current_constraint == &m_xz_constraint ? m_current_color : m_y_color, px, px + pz, pz);
// 		if(!motion || (m_x_constraint.internal_value() && m_y_constraint.internal_value()))
// 			draw_plane(m_current_constraint == &m_xy_constraint ? m_current_color : m_z_color, px, px + py, py);

	//if(!motion || &m_screen_xy_constraint == m_current_constraint)
	//	draw_screen_xy(Viewport, m_current_constraint == &m_screen_xy_constraint ? m_current_color : m_screen_xy_color);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void move_manipulators::set_camera(k3d::icamera* Camera)
{
	m_camera.set_value(Camera);
}


k3d::double_t move_manipulators::scale(const k3d::point3& Origin)
{
	// Project unit axis on screen space
	const k3d::matrix4 screen_matrix = k3d::node_to_world_matrix(*m_camera.internal_value());
	const k3d::vector3 screen_parallel = screen_matrix * k3d::vector3(1, 0, 0);
	const k3d::point3 position = k3d::gl::project(Origin);
	const k3d::point3 x_axis = k3d::gl::project(Origin + screen_parallel);
	const k3d::vector2 vec(x_axis[0] - position[0], x_axis[1] - position[1]);
	const k3d::double_t length = vec.length();

	return_val_if_fail(length, 0);

	return m_size.internal_value() / length;
}

void move_manipulators::draw_axis(const k3d::color& Color, const k3d::vector3& Vector, const k3d::bool_t Selected)
{
	k3d::gl::store_attributes attributes;

	const k3d::vector3 axis_vector = Vector * m_size.internal_value();
	const k3d::point3 position = m_position.internal_value();

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

	glEnable(GL_LIGHTING);
	k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
	k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, Color);
	k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
	k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));
	gluQuadricDrawStyle(m_quadric, GLU_FILL);
	gluQuadricNormals(m_quadric, GLU_SMOOTH);
	
	gluCylinder(m_quadric, m_axis_arrow_radius, m_axis_arrow_radius * 0.001, m_axis_arrow_length, m_axis_arrow_slices, 1);
	
	glPopMatrix();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(m_axis_line_width);
		
	k3d::gl::color3d(Selected ? m_selected_color.internal_value() : Color);
	
	glBegin(GL_LINES);
		k3d::gl::vertex3d(position);
		k3d::gl::vertex3d(position + axis_vector);
	glEnd();
}

} // namespace move

} // namespace qtui

} // namespace module

