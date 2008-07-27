#ifndef YAFRAY_LIGHTS_H
#define YAFRAY_LIGHTS_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/ilight_yafray.h>
#include <k3dsdk/node.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/vectors.h>

namespace module
{

namespace yafray
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
// renderable_light

template<typename base_t>
class renderable_light :
	public k3d::gl::renderable<base_t>
{
	typedef k3d::gl::renderable<base_t> base;

public:
	renderable_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		const bool emitting = base_t::m_emit.pipeline_value();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);

		k3d::gl::color3d(State.node_selection ? k3d::color(1, 1, 1) : emitting ? k3d::color(1, 1, 0) : k3d::color(0, 0, 0));
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
		m_photons(init_owner(*this) + init_name("photons") + init_label(_("photons")) + init_description(_("Photons")) + init_value(2000) + init_constraint(k3d::data::constraint::minimum<k3d::int32_t>(0))),
		m_depth(init_owner(*this) + init_name("depth") + init_label(_("depth")) + init_description(_("Depth")) + init_value(1) + init_constraint(k3d::data::constraint::minimum<k3d::int32_t>(0))),
		m_search(init_owner(*this) + init_name("search") + init_label(_("search")) + init_description(_("Search")) + init_value(100) + init_constraint(k3d::data::constraint::minimum<k3d::int32_t>(0)))
	{
	}

protected:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_photons;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_depth;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_search;
};

} // namespace yafray

} // namespace module

#endif // !YAFRAY_LIGHTS_H

