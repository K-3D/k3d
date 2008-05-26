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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include "detail.h"

namespace module
{

namespace quadrics
{

/////////////////////////////////////////////////////////////////////////////
// hyperboloid

class hyperboloid :
	public quadric
{
	typedef quadric base;

public:
	hyperboloid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_x1(init_owner(*this) + init_name("x1") + init_label(_("X1")) + init_description(_("From RenderMan specification")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y1(init_owner(*this) + init_name("y1") + init_label(_("Y1")) + init_description(_("From RenderMan specification")) + init_value(-5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z1(init_owner(*this) + init_name("z1") + init_label(_("Z1")) + init_description(_("From RenderMan specification")) + init_value(-5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_x2(init_owner(*this) + init_name("x2") + init_label(_("X2")) + init_description(_("From RenderMan specification")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y2(init_owner(*this) + init_name("y2") + init_label(_("Y2")) + init_description(_("From RenderMan specification")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z2(init_owner(*this) + init_name("z2") + init_label(_("Z2")) + init_description(_("From RenderMan specification")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("Theta max")) + init_description(_("From RenderMan specification")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_x1.changed_signal().connect(sigc::mem_fun(*this, &hyperboloid::reset_geometry));
		m_y1.changed_signal().connect(sigc::mem_fun(*this, &hyperboloid::reset_geometry));
		m_z1.changed_signal().connect(sigc::mem_fun(*this, &hyperboloid::reset_geometry));
		m_x2.changed_signal().connect(sigc::mem_fun(*this, &hyperboloid::reset_geometry));
		m_y2.changed_signal().connect(sigc::mem_fun(*this, &hyperboloid::reset_geometry));
		m_z2.changed_signal().connect(sigc::mem_fun(*this, &hyperboloid::reset_geometry));
		m_thetamax.changed_signal().connect(sigc::mem_fun(*this, &hyperboloid::reset_geometry));

		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
		m_input_matrix.changed_signal().connect(make_async_redraw_slot());
		m_material.changed_signal().connect(make_async_redraw_slot());

		m_gl_v_knot_vector.insert(m_gl_v_knot_vector.end(), 2, 0);
		m_gl_v_knot_vector.insert(m_gl_v_knot_vector.end(), 2, 1);
	}

	void reset_geometry(k3d::iunknown*)
	{
		m_gl_control_points.clear();
		k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	const k3d::bounding_box3 extents()
	{
		k3d::bounding_box3 bbox;
		bbox.insert(k3d::point3(m_x1.pipeline_value(), m_y1.pipeline_value(), m_z1.pipeline_value()));
		bbox.insert(k3d::point3(-m_x1.pipeline_value(), -m_y1.pipeline_value(), m_z1.pipeline_value()));
		bbox.insert(k3d::point3(m_x2.pipeline_value(), m_y2.pipeline_value(), m_z2.pipeline_value()));
		bbox.insert(k3d::point3(-m_x2.pipeline_value(), -m_y2.pipeline_value(), m_z2.pipeline_value()));

		return bbox;
	}

	void draw(const nurbs_renderer_t Nurbs)
	{
		if(m_gl_control_points.empty())
		{
			const double x1 = m_x1.pipeline_value();
			const double y1 = m_y1.pipeline_value();
			const double z1 = m_z1.pipeline_value();
			const double x2 = m_x2.pipeline_value();
			const double y2 = m_y2.pipeline_value();
			const double z2 = m_z2.pipeline_value();
			const double thetamax = m_thetamax.pipeline_value();

			if(thetamax == 0.0)
				return;

			const double thetamin1 = atan2(y1, x1);
			const double thetamin2 = atan2(y2, x2);

			const double radius1 = sqrt(x1 * x1 + y1 * y1);
			const double radius2 = sqrt(x2 * x2 + y2 * y2);

			const k3d::point3 offset1 = z1 * k3d::point3(0, 0, 1);
			const k3d::point3 offset2 = z2 * k3d::point3(0, 0, 1);

			std::vector<double> weights1;
			std::vector<k3d::point3> arc_points1;
			k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), thetamin1, thetamin1 + thetamax, 4, m_gl_u_knot_vector, weights1, arc_points1);

			std::vector<double> weights2;
			std::vector<k3d::point3> arc_points2;
			k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), thetamin2, thetamin2 + thetamax, 4, m_gl_u_knot_vector, weights2, arc_points2);

			for(unsigned long j = 0; j != arc_points1.size(); ++j)
			{
				m_gl_control_points.push_back(weights1[j] * (radius1 * arc_points1[j][0] + offset1[0]));
				m_gl_control_points.push_back(weights1[j] * (radius1 * arc_points1[j][1] + offset1[1]));
				m_gl_control_points.push_back(weights1[j] * (radius1 * arc_points1[j][2] + offset1[2]));
				m_gl_control_points.push_back(weights1[j]);
			}

			for(unsigned long j = 0; j != arc_points2.size(); ++j)
			{
				m_gl_control_points.push_back(weights2[j] * (radius2 * arc_points2[j][0] + offset2[0]));
				m_gl_control_points.push_back(weights2[j] * (radius2 * arc_points2[j][1] + offset2[1]));
				m_gl_control_points.push_back(weights2[j] * (radius2 * arc_points2[j][2] + offset2[2]));
				m_gl_control_points.push_back(weights2[j]);
			}
		}

		gluBeginSurface(Nurbs);
		gluNurbsSurface(Nurbs, m_gl_u_knot_vector.size(), &m_gl_u_knot_vector[0], m_gl_v_knot_vector.size(), &m_gl_v_knot_vector[0], 4, 36, &m_gl_control_points[0], 3, 2, GL_MAP2_VERTEX_4);
		gluEndSurface(Nurbs);
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::setup_material(m_material.pipeline_value());

		const nurbs_renderer_t nurbs = nurbs_renderer(State);

		k3d::gl::color3d(get_selection_weight() ? k3d::color(1, 1, 1) : k3d::color(0, 0, 0));
		gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_OUTLINE_PATCH);
		glDisable(GL_LIGHTING);
		glDisable(GL_AUTO_NORMAL);
		draw(nurbs);

		if(!State.draw_two_sided)
			glEnable(GL_CULL_FACE);

		gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
		glEnable(GL_LIGHTING);
		glEnable(GL_AUTO_NORMAL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		draw(nurbs);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		const nurbs_renderer_t nurbs = nurbs_renderer(State);
		gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_AUTO_NORMAL);
		glDisable(GL_CULL_FACE);

		k3d::gl::push_selection_token(this);
		draw(nurbs);
		k3d::gl::pop_selection_token();
	}

	void on_renderman_render(const k3d::ri::render_state& State)
	{
		const k3d::ri::point p1(k3d::point3(m_x1.pipeline_value(), m_y1.pipeline_value(), m_z1.pipeline_value()));
		const k3d::ri::point p2(k3d::point3(m_x2.pipeline_value(), m_y2.pipeline_value(), m_z2.pipeline_value()));
		const double thetamax = k3d::degrees(m_thetamax.pipeline_value());

		k3d::ri::setup_material(m_material.pipeline_value(), State);
		State.stream.RiHyperboloidV(p1, p2, thetamax);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<hyperboloid,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_source > > > factory(
				k3d::classes::Hyperboloid(),
				"Hyperboloid",
				_("Hyperboloid primitive"),
				"Quadric",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;

	std::vector<GLfloat> m_gl_u_knot_vector;
	std::vector<GLfloat> m_gl_v_knot_vector;
	std::vector<GLfloat> m_gl_control_points;
};

/////////////////////////////////////////////////////////////////////////////
// hyperboloid_factory

k3d::iplugin_factory& hyperboloid_factory()
{
	return hyperboloid::get_factory();
}

} // namespace quadrics

} // namespace module

