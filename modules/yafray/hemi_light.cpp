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
// hemi_light

class hemi_light :
	public renderable_light<colored_light<light<k3d::transformable<k3d::node > > > >
{
	typedef renderable_light<colored_light<light<k3d::transformable<k3d::node > > > > base;

public:
	hemi_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(2.0)),
		m_samples(init_owner(*this) + init_name("samples") + init_label(_("samples")) + init_description(_("Samples")) + init_value(256) + init_constraint(k3d::data::constraint::minimum<k3d::int32_t>(0, k3d::data::constraint::maximum<k3d::int32_t>(1024)))),
		m_use_QMC(init_owner(*this) + init_name("use_QMC") + init_label(_("use_QMC")) + init_description(_("Use QMC")) + init_value(false)),
		m_maxdistance(init_owner(*this) + init_name("maxdistance") + init_label(_("maxdistance")) + init_description(_("Max. distance")) + init_value(-1.0))
	{
		// We override the default white color ...
		m_color.internal_value() = k3d::color(0.9, 0.9, 1.0);
	}

	~hemi_light()
	{
		gluDeleteQuadric(m_quadric);
	}

	void setup_light(const k3d::string_t& Name, std::ostream& Stream)
	{
		if(!m_emit.pipeline_value())
			return;

		const k3d::color color = m_color.pipeline_value();

		Stream << "<!-- K-3D plugin: " << factory().name() << " name: " << name() << " -->\n";
		Stream << "<light type=\"hemilight\" name=\"" << Name << "\" power=\"" << m_power.pipeline_value() << "\" samples=\"" << m_samples.pipeline_value() << "\" use_QMC=\"" << (m_use_QMC.pipeline_value() ? "on" : "off") << "\" maxdistance=\"" << m_maxdistance.pipeline_value() << "\">\n";
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>\n";
		Stream << "</light>\n";
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, get_selection_weight() ? k3d::color(1, 1, 1) : k3d::color(0.4, 0.4, 0.4));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));

		glEnable(GL_LIGHTING);
		draw_geometry();
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::store_attributes attributes;

		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);

		k3d::gl::push_selection_token(this);
		draw_geometry();
		k3d::gl::pop_selection_token();
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<hemi_light > factory(
			k3d::uuid(0xa0661dc7, 0x52cd4990, 0x8e6a0aa8, 0x87bdd89d),
			"YafrayHemiLight",
			_("Yafray Hemi Light"),
			"Yafray Light",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void draw_geometry()
	{
		const GLdouble radius = 0.5;
		const GLint sides = 16;
		const double stacks = 4;

		const double angle = k3d::pi_over_2() / stacks;

		for(double n = 0; n < stacks; ++n)
		{
			const double angle1 = angle * n;
			const double angle2 = angle * (n + 1);
			const double z1 = sin(angle1) * radius;
			const double z2 = sin(angle2) * radius;
			const double r1 = cos(angle1) * radius;
			const double r2 = cos(angle2) * radius;

			glPushMatrix();
			glTranslated(0, 0, z1);
			gluCylinder(m_quadric, r1, r2 ? r2 : 0.001, z2 - z1, sides, 1);
			glPopMatrix();
		}
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_samples;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_use_QMC;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_maxdistance;
};

k3d::iplugin_factory& hemi_light_factory()
{
	return hemi_light::get_factory();
}

} // namespace yafray

} // namespace module


