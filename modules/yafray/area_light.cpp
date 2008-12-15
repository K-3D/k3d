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
		\author Tim Shead <tshead@k-3d.com>
		\author Romain Behar <romainbehar@yahoo.com>
*/

#include "lights.h"

namespace module
{

namespace yafray
{

/////////////////////////////////////////////////////////////////////////////
// area_light

class area_light :
	public colored_light<light<k3d::gl::renderable<k3d::transformable<k3d::node > > > >
{
	typedef colored_light<light<k3d::gl::renderable<k3d::transformable<k3d::node > > > > base;

public:
	area_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(20.0)),
		m_samples(init_owner(*this) + init_name("samples") + init_label(_("samples")) + init_description(_("Samples")) + init_value(256) + init_constraint(k3d::data::constraint::minimum<k3d::int32_t>(0, k3d::data::constraint::maximum<k3d::int32_t>(1024)))),
		m_psamples(init_owner(*this) + init_name("psamples") + init_label(_("psamples")) + init_description(_("PSamples")) + init_value(256) + init_constraint(k3d::data::constraint::minimum<k3d::int32_t>(0, k3d::data::constraint::maximum<k3d::int32_t>(1024)))),
		m_dummy(init_owner(*this) + init_name("dummy") + init_label(_("dummy")) + init_description(_("Dummy (global photon)")) + init_value(false))
	{
	}

	~area_light()
	{
		gluDeleteQuadric(m_quadric);
	}

	void setup_light(const k3d::string_t& Name, std::ostream& Stream)
	{
		if(!m_emit.pipeline_value())
			return;

		const k3d::color color = m_color.pipeline_value();

		const k3d::matrix4 transformation = k3d::node_to_world_matrix(*this);
		const k3d::point3 a = transformation * k3d::point3(-0.5, 0.5, 0);
		const k3d::point3 b = transformation * k3d::point3(0.5, 0.5, 0);
		const k3d::point3 c = transformation * k3d::point3(0.5, -0.5, 0);
		const k3d::point3 d = transformation * k3d::point3(-0.5, -0.5, 0);

		Stream << "<!-- K-3D plugin: " << factory().name() << " name: " << name() << " -->\n";
		Stream << "<light type=\"arealight\" name=\"" << Name << "\" power=\"" << m_power.pipeline_value() << "\" samples=\"" << m_samples.pipeline_value() << "\" psamples=\"" << m_psamples.pipeline_value() << "\" dummy=\"" << (m_dummy.pipeline_value() ? "on" : "off") << "\">\n";
		Stream << "	<a  x =\"" << -a[0] << "\" y =\"" << a[1] << "\" z =\"" << a[2] << "\"/>\n";
		Stream << "	<b  x =\"" << -b[0] << "\" y =\"" << b[1] << "\" z =\"" << b[2] << "\"/>\n";
		Stream << "	<c  x =\"" << -c[0] << "\" y =\"" << c[1] << "\" z =\"" << c[2] << "\"/>\n";
		Stream << "	<d  x =\"" << -d[0] << "\" y =\"" << d[1] << "\" z =\"" << d[2] << "\"/>\n";
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>\n";
		Stream << "</light>\n";
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		const bool emitting = m_emit.pipeline_value();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);

		if(emitting)
			glColor3d(1, 1, 1);
		else
			glColor3d(0, 0, 0);

		glLineWidth(1.0f);
		glDisable(GL_LINE_STIPPLE);

		glEnable(GL_LIGHTING);
		draw_geometry(State.node_selection ? k3d::color(1, 0, 0) : k3d::color(0.4, 0.4, 0.4));
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::store_attributes attributes;

		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);

		k3d::gl::push_selection_token(this);
		draw_geometry(k3d::color(1, 1, 1));
		k3d::gl::pop_selection_token();
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<area_light> factory(
			k3d::uuid(0x882e29b9, 0xc62a49da, 0x858cd48c, 0x43815d16),
			"YafrayAreaLight",
			_("Yafray Area Light"),
			"Yafray Light",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void draw_geometry(const k3d::color ArrowColor)
	{
		glBegin(GL_LINES);

		k3d::point3 coords(0, 0, 0);

		glVertex3d(coords[0] - 0.5, coords[1] + 0.5, coords[2]);
		glVertex3d(coords[0] + 0.5, coords[1] + 0.5, coords[2]);

		glVertex3d(coords[0] + 0.5, coords[1] + 0.5, coords[2]);
		glVertex3d(coords[0] + 0.5, coords[1] - 0.5, coords[2]);

		glVertex3d(coords[0] + 0.5, coords[1] - 0.5, coords[2]);
		glVertex3d(coords[0] - 0.5, coords[1] - 0.5, coords[2]);

		glVertex3d(coords[0] - 0.5, coords[1] - 0.5, coords[2]);
		glVertex3d(coords[0] - 0.5, coords[1] + 0.5, coords[2]);

		glEnd();

		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, ArrowColor);
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));
		gluQuadricDrawStyle(m_quadric, GLU_FILL);
		gluQuadricNormals(m_quadric, GLU_SMOOTH);
		gluCylinder(m_quadric, 0.1, 0.001, 1, 8, 1);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translate3(0, 0, 0.5));
		gluCylinder(m_quadric, 0.3, 0.001, 0.5, 8, 1);
		glPopMatrix();
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_samples;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_psamples;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_dummy;
};

k3d::iplugin_factory& area_light_factory()
{
	return area_light::get_factory();
}

} // namespace yafray

} // namespace module

