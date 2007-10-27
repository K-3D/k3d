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
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/vectors.h>

namespace libk3dannotation
{

/////////////////////////////////////////////////////////////////////////////
// annotate_normal

class annotate_normal :
	public k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > >
{
	typedef k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > > base;

public:
	annotate_normal(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_normal(init_owner(*this) + init_name("normal") + init_label(_("Normal")) + init_description(_("Normal")) + init_value(k3d::vector3(0, 0, 1))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(1, 1, 1)))
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
		m_normal.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_input_matrix.changed_signal().connect(make_async_redraw_slot());
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::color3d(get_selection_weight() ? k3d::color(1, 1, 1) : m_color.pipeline_value());
		draw(State);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);
		draw(State);
		k3d::gl::pop_selection_token();
	}

	void draw(const k3d::gl::render_state& State)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		const k3d::vector3 normal = m_normal.pipeline_value();

		glBegin(GL_LINES);
		k3d::gl::vertex3d(k3d::point3(0, 0, 0));
		k3d::gl::vertex3d(k3d::point3(0, 0, 0) + normal);
		glEnd();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<annotate_normal> factory(
			k3d::uuid(0x4a17ae9f, 0x237c4c81, 0x88dffdaf, 0x96b6d095),
			"AnnotateNormal",
			_("Displays a normal"),
			"Annotation",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_normal;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
};

/////////////////////////////////////////////////////////////////////////////
// annotate_normal

k3d::iplugin_factory& annotate_normal_factory()
{
	return annotate_normal::get_factory();
}

} // namespace libk3dannotation

