// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Implements the Light K-3D object, which encapsulates a light source within the 3D workspace
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/drawable_gl.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/ilight_gl.h>
#include <k3dsdk/ilight_ri.h>
#include <k3dsdk/ilight_shader_ri.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/vectors.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// light

class light :
	public k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > >,
	public k3d::gl::ilight,
	public k3d::ri::ilight
{
	typedef k3d::gl::drawable<k3d::transformable<k3d::persistent<k3d::node> > > base;

public:
	light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_shader(init_owner(*this) + init_name("shader") + init_label(_("Shader")) + init_description(_("Light shader")) + init_value<k3d::ri::ilight_shader*>(0))
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
		m_input_matrix.changed_signal().connect(make_async_redraw_slot());
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);

		k3d::gl::color3d(get_selection_weight() ? k3d::color(1, 1, 1) : k3d::color(1, 1, 0));
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

	void setup_gl_light(const unsigned long Light)
	{
		// Setup our own transformation matrix ...
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		k3d::gl::push_matrix(matrix());

		// Offset our light index ...
		GLenum light = GLenum(Light + GL_LIGHT0);

		// Enable the light ...
		glEnable(light);

		// Setup intensity / attenuation ...
		double intensity = 1.0 / 1200.0;
		glLightf(light, GL_CONSTANT_ATTENUATION, 0.0f);
		glLightf(light, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(light, GL_QUADRATIC_ATTENUATION, GLfloat(intensity));

		// Setup color ...
		k3d::point3 color(1.0, 1.0, 1.0);

		GLfloat glcolor[4];
		glcolor[0] = (GLfloat)color[0];
		glcolor[1] = (GLfloat)color[1];
		glcolor[2] = (GLfloat)color[2];
		glcolor[3] = 1.0f;

		glLightfv(light, GL_AMBIENT, glcolor);
		glLightfv(light, GL_DIFFUSE, glcolor);
		glLightfv(light, GL_SPECULAR, glcolor);

		GLfloat gllightpos[4];
		gllightpos[0] = 0.0f;
		gllightpos[1] = 0.0f;
		gllightpos[2] = 0.0f;
		gllightpos[3] = 1.0f;
		glLightfv(light, GL_POSITION, gllightpos);

		glLightf(light, GL_SPOT_CUTOFF, 180.0f);

		glPopMatrix();
	}

	void setup_renderman_light(const k3d::ri::render_state& State)
	{
		// If this is a shadow pass, we're done ...
		if(k3d::ri::SHADOW_MAP == State.render_context)
			return;

		// We only generate RIB on the last sample ...
		if(!k3d::ri::last_sample(State))
			return;

		// If the user hasn't selected a shader, we're done ...
		k3d::ri::ilight_shader* const shader = m_shader.value();
		if(!shader)
			return;

		// Make it happen ...
		State.engine.RiTransformBegin();
		State.engine.RiTransform(k3d::ri::convert(matrix()));
		k3d::ri::set_attributes(*this, State.engine);
		shader->setup_renderman_light_shader(State);
		State.engine.RiTransformEnd();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<light,
			k3d::interface_list<k3d::gl::ilight,
			k3d::interface_list<k3d::ri::ilight,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > > > factory(
			k3d::classes::RenderManLight(),
			"RenderManLight",
			_("RenderMan light source"),
			"RenderMan Lights",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::ri::ilight_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_shader;
};

/////////////////////////////////////////////////////////////////////////////
// light_factory

k3d::iplugin_factory& light_factory()
{
	return light::get_factory();
}

} // namespace libk3drenderman

