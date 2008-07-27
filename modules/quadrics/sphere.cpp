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
*/

#include "detail.h"

namespace module
{

namespace quadrics
{

/////////////////////////////////////////////////////////////////////////////
// sphere

class sphere :
	public quadric
{
	typedef quadric base;

public:
	sphere(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Sphere radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmin(init_owner(*this) + init_name("zmin") + init_label(_("Z min")) + init_description(_("From RenderMan specification")) + init_value(-1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmax(init_owner(*this) + init_name("zmax") + init_label(_("Z max")) + init_description(_("From RenderMan specification")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("Theta max")) + init_description(_("From RenderMan specification")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_radius.changed_signal().connect(sigc::mem_fun(*this, &sphere::reset_geometry));
		m_zmin.changed_signal().connect(sigc::mem_fun(*this, &sphere::reset_geometry));
		m_zmax.changed_signal().connect(sigc::mem_fun(*this, &sphere::reset_geometry));
		m_thetamax.changed_signal().connect(sigc::mem_fun(*this, &sphere::reset_geometry));

		m_input_matrix.changed_signal().connect(make_async_redraw_slot());

		m_radius.changed_signal().connect(make_node_change_slot());
		m_input_matrix.changed_signal().connect(make_node_change_slot());
		m_material.changed_signal().connect(make_async_redraw_slot());

		add_snap_target(new k3d::snap_target(_("Surface"), sigc::mem_fun(*this, &sphere::surface_target_position), sigc::mem_fun(*this, &sphere::surface_target_orientation)));
	}

	bool surface_target_position(const k3d::point3& Position, k3d::point3& TargetPosition)
	{
		TargetPosition = k3d::to_point(k3d::normalize(k3d::to_vector(Position)) * m_radius.pipeline_value());
		return true;
	}

	bool surface_target_orientation(const k3d::point3& Position, k3d::vector3& Look, k3d::vector3& Up)
	{
		Look = k3d::to_vector(Position);
		Up = Look ^ (Look ^ k3d::vector3(0, 0, 1));
		return true;
	}

	const k3d::bounding_box3 extents()
	{
		const double radius = m_radius.pipeline_value();
		const double zmax = m_zmax.pipeline_value();
		const double zmin = m_zmin.pipeline_value();

		return k3d::bounding_box3(radius, -radius, radius, -radius, zmax * radius, zmin * radius);
	}

	void reset_geometry(k3d::iunknown*)
	{
		m_gl_control_points.clear();
		k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	void draw(const nurbs_renderer_t Nurbs)
	{
		if(m_gl_control_points.empty())
		{
			const double radius = m_radius.pipeline_value();
			const double zmin = m_zmin.pipeline_value();
			const double zmax = m_zmax.pipeline_value();
			const double thetamax = m_thetamax.pipeline_value();

			const double phimin = (zmin > -1) ? asin(zmin) : -k3d::pi_over_2();
			const double phimax = (zmax < 1) ? asin(zmax) : k3d::pi_over_2();

			if(thetamax == 0.0 || (zmin == zmax))
				return;

			std::vector<double> v_weights;
			std::vector<k3d::point3> v_arc_points;
			k3d::nurbs::circular_arc(k3d::point3(0, 1, 0), k3d::point3(0, 0, 1), phimin, phimax, 2, m_gl_v_knot_vector, v_weights, v_arc_points);

			std::vector<double> u_weights;
			std::vector<k3d::point3> u_arc_points;
			k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), 0, thetamax, 4, m_gl_u_knot_vector, u_weights, u_arc_points);

			for(unsigned long v = 0; v != v_arc_points.size(); ++v)
			{
				const k3d::point3 offset = radius * v_arc_points[v][2] * k3d::point3(0, 0, 1);
				const double radius2 = radius * v_arc_points[v][1];
				const double v_weight = v_weights[v];

				for(unsigned long u = 0; u != u_arc_points.size(); ++u)
				{
					m_gl_control_points.push_back(v_weight * u_weights[u] * (radius2 * u_arc_points[u][0] + offset[0]));
					m_gl_control_points.push_back(v_weight * u_weights[u] * (radius2 * u_arc_points[u][1] + offset[1]));
					m_gl_control_points.push_back(v_weight * u_weights[u] * (radius2 * u_arc_points[u][2] + offset[2]));
					m_gl_control_points.push_back(v_weight * u_weights[u]);
				}
			}
		}

		gluBeginSurface(Nurbs);
		gluNurbsSurface(Nurbs, m_gl_u_knot_vector.size(), &m_gl_u_knot_vector[0], m_gl_v_knot_vector.size(), &m_gl_v_knot_vector[0], 4, 36, &m_gl_control_points[0], 3, 3, GL_MAP2_VERTEX_4);
		gluEndSurface(Nurbs);
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::setup_material(m_material.pipeline_value());

		const nurbs_renderer_t nurbs = nurbs_renderer(State);

		k3d::gl::color3d(State.node_selection ? k3d::color(1, 1, 1) : k3d::color(0, 0, 0));
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
		const double radius = m_radius.pipeline_value();
		const double zmin = m_zmin.pipeline_value();
		const double zmax = m_zmax.pipeline_value();
		const double thetamax = k3d::degrees(m_thetamax.pipeline_value());

		k3d::ri::setup_material(m_material.pipeline_value(), State);
		State.stream.RiSphereV(radius, radius * zmin, radius * zmax, thetamax);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sphere,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::classes::Sphere(),
				"Sphere",
				_("Sphere primitive"),
				"Quadric",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_zmin;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_zmax;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;

	std::vector<GLfloat> m_gl_u_knot_vector;
	std::vector<GLfloat> m_gl_v_knot_vector;
	std::vector<GLfloat> m_gl_control_points;
};

/////////////////////////////////////////////////////////////////////////////
// sphere_factory

k3d::iplugin_factory& sphere_factory()
{
	return sphere::get_factory();
}

} // namespace quadrics

} // namespace module

