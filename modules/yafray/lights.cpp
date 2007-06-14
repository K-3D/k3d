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

#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/drawable_gl.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/iyafray.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/vectors.h>

namespace libk3dyafray
{

/////////////////////////////////////////////////////////////////////////////
// light

template<typename base_t>
class light :
	public base_t,
	public k3d::yafray::ilight
{
public:
	light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base_t(Factory, Document),
		m_emit(init_owner(*this) + init_name("emit") + init_label(_("emit")) + init_description(_("Emit Light")) + init_value(true))
	{
	}

protected:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_emit;
};

/////////////////////////////////////////////////////////////////////////////
// colored_light

template<typename base_t>
class colored_light :
	public base_t
{
public:
	colored_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base_t(Factory, Document),
		m_color(init_owner(*this) + init_name("color") + init_label(_("color")) + init_description(_("Light color")) + init_value(k3d::color(1, 1, 1)))
	{
	}

protected:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

/////////////////////////////////////////////////////////////////////////////
// optional_shadows

template<typename base_t>
class optional_shadows :
	public base_t
{
public:
	optional_shadows(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base_t(Factory, Document),
		m_cast_shadows(init_owner(*this) + init_name("cast_shadows") + init_label(_("cast_shadows")) + init_description(_("Cast shadows")) + init_value(true))
	{
	}

protected:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cast_shadows;
};

/////////////////////////////////////////////////////////////////////////////
// drawable_light

template<typename base_t>
class drawable_light :
	public k3d::gl::drawable<base_t>
{
	typedef k3d::gl::drawable<base_t> base;

public:
	drawable_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		base::m_selection_weight.changed_signal().connect(base::make_async_redraw_slot());
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		const bool emitting = base_t::m_emit.value();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);

		k3d::gl::color3d(base_t::get_selection_weight() ? k3d::color(1, 1, 1) : emitting ? k3d::color(1, 1, 0) : k3d::color(0, 0, 0));
		glLineWidth(1.0f);
		glDisable(GL_LINE_STIPPLE);

		draw();
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);
		draw();
		k3d::gl::pop_selection_token();
	}

private:
	void draw()
	{
		glBegin(GL_LINES);

		k3d::point3 coords(0, 0, 0);

		glVertex3d(coords[0] + 1.0, coords[1], coords[2]);
		glVertex3d(coords[0] - 1.0, coords[1], coords[2]);
		glVertex3d(coords[0], coords[1] + 1.0, coords[2]);
		glVertex3d(coords[0], coords[1] - 1.0, coords[2]);
		glVertex3d(coords[0], coords[1], coords[2] + 1.0);
		glVertex3d(coords[0], coords[1], coords[2] - 1.0);

		glVertex3d(coords[0] + 0.4, coords[1] + 0.4, coords[2] + 0.4);
		glVertex3d(coords[0] - 0.4, coords[1] - 0.4, coords[2] - 0.4);
		glVertex3d(coords[0] - 0.4, coords[1] + 0.4, coords[2] + 0.4);
		glVertex3d(coords[0] + 0.4, coords[1] - 0.4, coords[2] - 0.4);
		glVertex3d(coords[0] + 0.4, coords[1] + 0.4, coords[2] - 0.4);
		glVertex3d(coords[0] - 0.4, coords[1] - 0.4, coords[2] + 0.4);
		glVertex3d(coords[0] - 0.4, coords[1] + 0.4, coords[2] - 0.4);
		glVertex3d(coords[0] + 0.4, coords[1] - 0.4, coords[2] + 0.4);

		glEnd();
	}
};

/////////////////////////////////////////////////////////////////////////////
// photons

template<typename base_t>
class photons :
	public base_t
{
public:
	photons(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base_t(Factory, Document),
		m_photons(init_owner(*this) + init_name("photons") + init_label(_("photons")) + init_description(_("Photons")) + init_value(2000) + init_constraint(k3d::data::constraint::minimum(0L))),
		m_depth(init_owner(*this) + init_name("depth") + init_label(_("depth")) + init_description(_("Depth")) + init_value(1) + init_constraint(k3d::data::constraint::minimum(0L))),
		m_search(init_owner(*this) + init_name("search") + init_label(_("search")) + init_description(_("Search")) + init_value(100) + init_constraint(k3d::data::constraint::minimum(0L)))
	{
	}

protected:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_photons;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_depth;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_search;
};

/////////////////////////////////////////////////////////////////////////////
// point_light

class point_light :
	public drawable_light<optional_shadows<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > > >
{
	typedef drawable_light<optional_shadows<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > > > base;

public:
	point_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(400.0))
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
	}

	~point_light()
	{
		gluDeleteQuadric(m_quadric);
	}

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		const k3d::point3 from = k3d::world_position(*this);
		const k3d::color color = m_color.value();

		Stream << "<light type=\"pointlight\" name=\"" << name() << "\" power=\"" << m_power.value() << "\" cast_shadows=\"" << (m_cast_shadows.value() ? "yes" : "no") << "\">" << std::endl;
		Stream << "	<from x=\"" << std::fixed << -from[0] << "\" y=\"" << std::fixed << from[1] << "\" z=\"" << std::fixed << from[2] << "\"/>" << std::endl;
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "</light>" << std::endl;
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

		k3d::gl::push_selection_token(this);
		draw_geometry();
		k3d::gl::pop_selection_token();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<point_light,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::uuid(0xd693bb64, 0xd73943ce, 0x80852061, 0x24fd242e),
				"YafrayPointLight",
				_("Yafray Point Light"),
				"Yafray Lights",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void draw_geometry()
	{
		gluSphere(m_quadric, 0.5, 8, 8);
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
};

/////////////////////////////////////////////////////////////////////////////
// sun_light

class sun_light :
	public drawable_light<optional_shadows<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > > >
{
	typedef drawable_light<optional_shadows<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > > > base;

public:
	sun_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(40.0))
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
	}

	~sun_light()
	{
		gluDeleteQuadric(m_quadric);
	}

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		const k3d::point3 from = k3d::world_position(*this);
		const k3d::color color = m_color.value();

		Stream << "<light type=\"sunlight\" name=\"" << name() << "\" power=\"" << m_power.value() << "\" cast_shadows=\"" << (m_cast_shadows.value() ? "yes" : "no") << "\">" << std::endl;
		Stream << "	<from x=\"" << std::fixed << -from[0] << "\" y=\"" << std::fixed << from[1] << "\" z=\"" << std::fixed << from[2] << "\"/>" << std::endl;
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "</light>" << std::endl;
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, get_selection_weight() ? k3d::color(1, 1, 0) : k3d::color(0.4, 0.4, 0.4));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));

		glEnable(GL_LIGHTING);
		draw_geometry();
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::store_attributes attributes;

		glDisable(GL_LIGHTING);

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
		static k3d::document_plugin_factory<sun_light,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::uuid(0x9a48777d, 0xa68345b5, 0xacb9fc07, 0x8af3e7f6),
				"YafraySunLight",
				_("Yafray Sun Light"),
				"Yafray Lights",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void draw_geometry()
	{
		gluSphere(m_quadric, 0.2, 8, 8);

		const k3d::point3 from = m_input_matrix.value() * k3d::point3(0, 0, 0);
		const k3d::point3 to = k3d::point3(0, 0, 0);
		const k3d::vector3 spherical = k3d::spherical(k3d::to_vector(to - from));
		glPushMatrix();
		k3d::gl::push_matrix(k3d::inverse(k3d::extract_rotation(m_input_matrix.value())) * rotation3D(k3d::quaternion(k3d::euler_angles(0, -spherical[2], spherical[1], k3d::euler_angles::ZXYstatic))));

		gluQuadricDrawStyle(m_quadric, GLU_FILL);
		gluQuadricNormals(m_quadric, GLU_SMOOTH);
		gluCylinder(m_quadric, 0.1, 0.001, 1, 8, 1);
		glPushMatrix();
		k3d::gl::push_matrix(k3d::translation3D(k3d::point3(0, 0, 0.5)));
		gluCylinder(m_quadric, 0.3, 0.001, 0.5, 8, 1);
		glPopMatrix();

		glPopMatrix();
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
};

/////////////////////////////////////////////////////////////////////////////
// soft_light

class soft_light :
	public drawable_light<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > >
{
	typedef drawable_light<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > > base;

public:
	soft_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(400.0)),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Softness")) + init_value(5.0)),
		m_resolution(init_owner(*this) + init_name("resolution") + init_label(_("resolution")) + init_description(_("Shadowmap resolution")) + init_value(256) + init_constraint(k3d::data::constraint::minimum(0L, k3d::data::constraint::maximum(4096L)))),
		m_bias(init_owner(*this) + init_name("bias") + init_label(_("bias")) + init_description(_("Shadowmap bias")) + init_value(0.01)),
		m_glow_intensity(init_owner(*this) + init_name("glow_intensity") + init_label(_("glow_intensity")) + init_description(_("Glow intensity")) + init_value(0)),
		m_glow_type(init_owner(*this) + init_name("glow_type") + init_label(_("glow_type")) + init_description(_("Glow type")) + init_value(0)),
		m_glow_offset(init_owner(*this) + init_name("glow_offset") + init_label(_("glow_offset")) + init_description(_("Glow offset")) + init_value(0))
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
	}

	~soft_light()
	{
		gluDeleteQuadric(m_quadric);
	}

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		const k3d::point3 from = k3d::world_position(*this);
		const k3d::color color = m_color.value();

		Stream << "<light type=\"softlight\" name=\"" << name() << "\" power=\"" << m_power.value() << "\" res=\"" << m_resolution.value()  << "\" radius=\"" << m_radius.value() << "\" bias=\"" << m_bias.value() << "\" glow_intensity=\"" << m_glow_intensity.value() << "\" glow_type=\"" << m_glow_type.value() << "\" glow_offset=\"" << m_glow_offset.value() << "\">" << std::endl;
		Stream << "	<from x=\"" << std::fixed << -from[0] << "\" y=\"" << std::fixed << from[1] << "\" z=\"" << std::fixed << from[2] << "\"/>" << std::endl;
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "</light>" << std::endl;
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, get_selection_weight() ? k3d::color(0, 1, 1) : k3d::color(0.4, 0.4, 0.4));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, k3d::color(0, 0, 0));
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, k3d::color(0, 0, 0));

		glEnable(GL_LIGHTING);
		draw_geometry();
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::store_attributes attributes;

		glDisable(GL_LIGHTING);

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
		static k3d::document_plugin_factory<soft_light,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::uuid(0x2fcaffb5, 0xed294a0d, 0x82133a8f, 0x48df4988),
				"YafraySoftLight",
				_("Yafray Soft Light"),
				"Yafray Lights",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void draw_geometry()
	{
		gluSphere(m_quadric, 0.5, 8, 8);
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_radius;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_resolution;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bias;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_glow_intensity;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_glow_type;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_glow_offset;
};

/////////////////////////////////////////////////////////////////////////////
// spot_light

class spot_light :
	public colored_light<light<k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > > > >
{
	typedef colored_light<light<k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > > > > base;

public:
	spot_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(400.0)),
		m_size(init_owner(*this) + init_name("size") + init_label(_("size")) + init_description(_("Angle")) + init_value(k3d::radians(30.0))),
		m_nblend(init_owner(*this) + init_name("nblend") + init_label(_("nblend")) + init_description(_("Edge softness")) + init_value(5.0)),
		m_beam_falloff(init_owner(*this) + init_name("beam_falloff") + init_label(_("beam_falloff")) + init_description(_("Beam falloff")) + init_value(10.0))
	{
	}

	~spot_light()
	{
		gluDeleteQuadric(m_quadric);
	}

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		const k3d::point3 from = k3d::world_position(*this);
		const k3d::point3 to = from + k3d::node_to_world_matrix(*this) * k3d::vector3(0, 0, 1);
		const k3d::color color = m_color.value();
		const double power = m_power.value();
		const double size = k3d::degrees(m_size.value());
		const double nblend = m_nblend.value();
		const double beam_falloff = m_beam_falloff.value();

		Stream << "<light type=\"spotlight\" name=\"" << name() << "\" power=\"" << power << "\" size=\"" << size << "\" nblend=\"" << nblend << "\" beam_falloff=\"" << beam_falloff << "\">" << std::endl;
		Stream << "	<from x=\"" << std::fixed << -from[0] << "\" y=\"" << std::fixed << from[1] << "\" z=\"" << std::fixed << from[2] << "\"/>" << std::endl;
		Stream << "	<to x=\"" << std::fixed << -to[0] << "\" y=\"" << std::fixed << to[1] << "\" z=\"" << std::fixed << to[2] << "\"/>" << std::endl;
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "</light>" << std::endl;
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
		static k3d::document_plugin_factory<spot_light,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::uuid(0x5e363371, 0xf8464895, 0x99f0ddf0, 0x4e26ee4a),
				"YafraySpotLight",
				_("Yafray Spot Light"),
				"Yafray Lights",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void draw_geometry()
	{
		gluQuadricDrawStyle(m_quadric, GLU_FILL);
		gluQuadricNormals(m_quadric, GLU_SMOOTH);
		gluCylinder(m_quadric, 0.001, 0.2, 1, 8, 1);
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_size;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_nblend;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_beam_falloff;
};

/////////////////////////////////////////////////////////////////////////////
// hemi_light

class hemi_light :
	public drawable_light<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > >
{
	typedef drawable_light<colored_light<light<k3d::transformable<k3d::persistent<k3d::node> > > > > base;

public:
	hemi_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(2.0)),
		m_samples(init_owner(*this) + init_name("samples") + init_label(_("samples")) + init_description(_("Samples")) + init_value(256) + init_constraint(k3d::data::constraint::minimum(0L, k3d::data::constraint::maximum(1024L)))),
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

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		const k3d::color color = m_color.value();

		Stream << "<light type=\"hemilight\" name=\"" << name() << "\" power=\"" << m_power.value() << "\" samples=\"" << m_samples.value() << "\" use_QMC=\"" << (m_use_QMC.value() ? "on" : "off") << "\" maxdistance=\"" << m_maxdistance.value() << "\">" << std::endl;
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "</light>" << std::endl;
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
			"Yafray Lights",
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
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_samples;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_use_QMC;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_maxdistance;
};

/////////////////////////////////////////////////////////////////////////////
// area_light

class area_light :
	public colored_light<light<k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > > > >
{
	typedef colored_light<light<k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > > > > base;

public:
	area_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_quadric(gluNewQuadric()),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(20.0)),
		m_samples(init_owner(*this) + init_name("samples") + init_label(_("samples")) + init_description(_("Samples")) + init_value(256) + init_constraint(k3d::data::constraint::minimum(0L, k3d::data::constraint::maximum(1024L)))),
		m_psamples(init_owner(*this) + init_name("psamples") + init_label(_("psamples")) + init_description(_("PSamples")) + init_value(256) + init_constraint(k3d::data::constraint::minimum(0L, k3d::data::constraint::maximum(1024L)))),
		m_dummy(init_owner(*this) + init_name("dummy") + init_label(_("dummy")) + init_description(_("Dummy (global photon)")) + init_value(false))
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
	}

	~area_light()
	{
		gluDeleteQuadric(m_quadric);
	}

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		const k3d::color color = m_color.value();

		const k3d::matrix4 transformation = k3d::node_to_world_matrix(*this);
		const k3d::point3 a = transformation * k3d::point3(-0.5, 0.5, 0);
		const k3d::point3 b = transformation * k3d::point3(0.5, 0.5, 0);
		const k3d::point3 c = transformation * k3d::point3(0.5, -0.5, 0);
		const k3d::point3 d = transformation * k3d::point3(-0.5, -0.5, 0);

		Stream << "<light type=\"arealight\" name=\"" << name() << "\" power=\"" << m_power.value() << "\" samples=\"" << m_samples.value() << "\" psamples=\"" << m_psamples.value() << "\" dummy=\"" << (m_dummy.value() ? "on" : "off") << "\">" << std::endl;
		Stream << "	<a  x =\"" << -a[0] << "\" y =\"" << a[1] << "\" z =\"" << a[2] << "\"/>" << std::endl;
		Stream << "	<b  x =\"" << -b[0] << "\" y =\"" << b[1] << "\" z =\"" << b[2] << "\"/>" << std::endl;
		Stream << "	<c  x =\"" << -c[0] << "\" y =\"" << c[1] << "\" z =\"" << c[2] << "\"/>" << std::endl;
		Stream << "	<d  x =\"" << -d[0] << "\" y =\"" << d[1] << "\" z =\"" << d[2] << "\"/>" << std::endl;
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "</light>" << std::endl;
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		const bool emitting = m_emit.value();

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
		draw_geometry(get_selection_weight() ? k3d::color(1, 0, 0) : k3d::color(0.4, 0.4, 0.4));
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
			"Yafray Lights",
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
		k3d::gl::push_matrix(k3d::translation3D(k3d::point3(0, 0, 0.5)));
		gluCylinder(m_quadric, 0.3, 0.001, 0.5, 8, 1);
		glPopMatrix();
	}

	/// Stores a GLU quadric object for drawing the manipulators
	GLUquadricObj* const m_quadric;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_samples;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_psamples;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_dummy;
};

/////////////////////////////////////////////////////////////////////////////
// global_photon_light

class global_photon_light :
	public photons<light<k3d::persistent<k3d::node> > >
{
	typedef photons<light<k3d::persistent<k3d::node> > > base;

public:
	global_photon_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(2.0))
	{
	}

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		Stream << "<light type=\"globalphotonlight\" name=\"" << name() << "\" photons=\"" << m_photons.value() << "\" radius=\"" << m_radius.value() << "\" depth=\"" << m_depth.value() << "\" search=\"" << m_search.value() << "\">" << std::endl;
		Stream << "</light>" << std::endl;
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<global_photon_light> factory(
			k3d::uuid(0x8c651262, 0x2e2d4fcc, 0xbb20f617, 0xf0c56f41),
			"YafrayGlobalPhotonLight",
			_("Yafray Global Photon Light"),
			"Yafray Lights",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_radius;
};

/////////////////////////////////////////////////////////////////////////////
// photon_light

class photon_light :
	public colored_light<photons<light<k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > > > > >
{
	typedef colored_light<photons<light<k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > > > > > base;

public:
	photon_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_power(init_owner(*this) + init_name("power") + init_label(_("power")) + init_description(_("Power")) + init_value(100.0)),
		m_mode(init_owner(*this) + init_name("mode") + init_label(_("mode")) + init_description(_("Mode")) + init_value(DIFFUSE) + init_enumeration(mode_values())),
		m_fixedradius(init_owner(*this) + init_name("fixedradius") + init_label(_("fixedradius")) + init_description(_("Fixed Radius")) + init_value(0.1)),
		m_cluster(init_owner(*this) + init_name("cluster") + init_label(_("cluster")) + init_description(_("Cluster")) + init_value(0.1)),
		m_use_QMC(init_owner(*this) + init_name("use_QMC") + init_label(_("use_QMC")) + init_description(_("Use QMC")) + init_value(false)),
		m_angle(init_owner(*this) + init_name("angle") + init_label(_("angle")) + init_description(_("Angle")) + init_value(k3d::radians(30.0))),
		m_mindepth(init_owner(*this) + init_name("mindepth") + init_label(_("mindepth")) + init_description(_("Min. depth")) + init_value(1)),
		m_bias(init_owner(*this) + init_name("bias") + init_label(_("bias")) + init_description(_("Bias")) + init_value(0.001))
	{
	}

	void setup_light(std::ostream& Stream)
	{
		if(!m_emit.value())
			return;

		const k3d::color color = m_color.value();

		const k3d::point3 from = k3d::world_position(*this);
		const k3d::point3 to = from + k3d::node_to_world_matrix(*this) * k3d::vector3(0, 0, 1);

		Stream << "<light type=\"photonlight\" name=\"" << name() << "\" power=\"" << m_power.value() << "\" photons=\"" << m_photons.value() << "\" depth=\"" << m_depth.value() << "\" mindepth=\"" << m_mindepth.value() << "\" bias=\"" << m_bias.value() << "\" search=\"" << m_search.value() << "\" angle=\"" << m_angle.value() << "\" mode=\"" << (m_mode.value() == DIFFUSE ? "diffuse" : "caustic") << "\" fixedradius=\"" << m_fixedradius.value() << "\" cluster=\"" << m_cluster.value() << "\" use_QMC=\"" << (m_use_QMC.value() ? "on" : "off") << "\">" << std::endl;
		Stream << "	<from  x =\"" << -from[0] << "\" y =\"" << from[1] << "\" z =\"" << from[2] << "\"/>" << std::endl;
		Stream << "	<to  x =\"" << -to[0] << "\" y =\"" << to[1] << "\" z =\"" << to[2] << "\"/>" << std::endl;
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "</light>" << std::endl;
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		const bool emitting = m_emit.value();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);

		if(emitting)
			glColor3d(1, 1, 1);
		else
			glColor3d(0, 0, 0);

		glLineWidth(1.0f);
		glDisable(GL_LINE_STIPPLE);

		draw_geometry();
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::store_attributes attributes;
		draw_geometry();
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<photon_light> factory(
			k3d::uuid(0x2b90ffa5, 0x9360411b, 0xbd982d69, 0xe3f45f31),
			"YafrayPhotonLight",
			_("Yafray Photon Light"),
			"Yafray Lights",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void draw_geometry()
	{
		glBegin(GL_LINES);

		k3d::point3 coords(0, 0, 0);

		glVertex3d(coords[0], coords[1], coords[2]);
		glVertex3d(coords[0] - 0.2, coords[1] + 0.2, coords[2] + 1.0);
		glVertex3d(coords[0], coords[1], coords[2]);
		glVertex3d(coords[0] + 0.2, coords[1] + 0.2, coords[2] + 1.0);
		glVertex3d(coords[0], coords[1], coords[2]);
		glVertex3d(coords[0] + 0.2, coords[1] - 0.2, coords[2] + 1.0);
		glVertex3d(coords[0], coords[1], coords[2]);
		glVertex3d(coords[0] - 0.2, coords[1] - 0.2, coords[2] + 1.0);

		glEnd();
	}

	typedef enum
	{
		DIFFUSE,
		CAUSTIC,
	} mode_t;

	friend std::ostream& operator << (std::ostream& Stream, const mode_t& Value)
	{
		switch(Value)
		{
			case DIFFUSE:
				Stream << "diffuse";
				break;
			case CAUSTIC:
				Stream << "caustic";
				break;
		}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, mode_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "diffuse")
			Value = DIFFUSE;
		else if(text == "caustic")
			Value = CAUSTIC;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& mode_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Diffuse", "diffuse", "Photons are reflected by diffuse surfaces in random directions (for radiosity or global illumination)"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Caustic", "caustic", "The light will draw reflected and transmitted photons causing caustic patterns"));
		}

		return values;
	}

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
	k3d_data(mode_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_mode;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_fixedradius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cluster;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_use_QMC;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_angle;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_mindepth;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bias;
};

k3d::iplugin_factory& point_light_factory()
{
	return point_light::get_factory();
}

k3d::iplugin_factory& sun_light_factory()
{
	return sun_light::get_factory();
}

k3d::iplugin_factory& soft_light_factory()
{
	return soft_light::get_factory();
}

k3d::iplugin_factory& spot_light_factory()
{
	return spot_light::get_factory();
}

k3d::iplugin_factory& area_light_factory()
{
	return area_light::get_factory();
}

k3d::iplugin_factory& hemi_light_factory()
{
	return hemi_light::get_factory();
}

k3d::iplugin_factory& global_photon_light_factory()
{
	return global_photon_light::get_factory();
}

k3d::iplugin_factory& photon_light_factory()
{
	return photon_light::get_factory();
}

} // namespace libk3dyafray

