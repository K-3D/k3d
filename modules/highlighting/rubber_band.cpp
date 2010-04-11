// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irenderable_gl.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/render_state_gl.h>
#include <k3dsdk/utility_gl.h>

namespace module
{

namespace highlighting
{

/////////////////////////////////////////////////////////////////////////////
// rubber_band

class rubber_band :
	public k3d::node,
	public k3d::gl::irenderable
{
	typedef k3d::node base;

public:
	rubber_band(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_camera(init_owner(*this) + init_name("camera") + init_label(_("Camera")) + init_description(_("Camera")) + init_value<k3d::icamera*>(0)),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(1, 0, 0))),
		m_opacity(init_owner(*this) + init_name("opacity") + init_label(_("Opacity")) + init_description(_("Opacity")) + init_value(0.1) + init_units(typeid(k3d::measurement::scalar)) + init_step_increment(0.1)),
		m_border_color(init_owner(*this) + init_name("border_color") + init_label(_("Border Color")) + init_description(_("Border Color")) + init_value(k3d::color(1, 0, 0))),
		m_border_opacity(init_owner(*this) + init_name("border_opacity") + init_label(_("Border Opacity")) + init_description(_("Border Opacity")) + init_value(0.5) + init_units(typeid(k3d::measurement::scalar)) + init_step_increment(0.1)),
		m_rectangle(init_owner(*this) + init_name("rectangle") + init_label(_("Rectangle")) + init_description(_("Rectangle")) + init_value(k3d::rectangle(0, 0, 0, 0)))
	{
		m_camera.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_opacity.changed_signal().connect(make_async_redraw_slot());
		m_border_color.changed_signal().connect(make_async_redraw_slot());
		m_border_opacity.changed_signal().connect(make_async_redraw_slot());
		m_rectangle.changed_signal().connect(make_async_redraw_slot());
	}

	sigc::slot<void, k3d::ihint*> make_async_redraw_slot()
	{
		return sigc::mem_fun(*this, &rubber_band::async_redraw);
	}

	void async_redraw(k3d::ihint*)
	{
		k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	k3d::uint_t gl_layer()
	{
		return 4096;
	}

	void gl_draw(const k3d::gl::render_state& State)
	{
		if(&State.camera != m_camera.pipeline_value())
			return;

		k3d::gl::store_attributes attributes;

		draw(State);
	}

	void gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		/** \note Selection is disabled for this plugin, so we don't interfere with
		 * selection of "normal" geometry */
	}

	void draw(const k3d::gl::render_state& State)
	{
		const k3d::color color = m_color.pipeline_value();
		const k3d::double_t opacity = m_opacity.pipeline_value();
		const k3d::color border_color = m_border_color.pipeline_value();
		const k3d::double_t border_opacity = m_border_opacity.pipeline_value();

		const k3d::rectangle rectangle = m_rectangle.pipeline_value();
		const k3d::double_t depth = 0;

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(State.gl_viewport[0], State.gl_viewport[0] + State.gl_viewport[2], State.gl_viewport[1], State.gl_viewport[1] + State.gl_viewport[3], -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);

		glColor4d(color.red, color.green, color.blue, opacity);
		glBegin(GL_QUADS);
			glVertex3d(rectangle.x1, rectangle.y1, depth);
			glVertex3d(rectangle.x2, rectangle.y1, depth);
			glVertex3d(rectangle.x2, rectangle.y2, depth);
			glVertex3d(rectangle.x1, rectangle.y2, depth);
		glEnd();

		glColor4d(border_color.red, border_color.green, border_color.blue, border_opacity);
		glBegin(GL_LINE_LOOP);
			glVertex3d(rectangle.x1, rectangle.y1, depth);
			glVertex3d(rectangle.x2, rectangle.y1, depth);
			glVertex3d(rectangle.x2, rectangle.y2, depth);
			glVertex3d(rectangle.x1, rectangle.y2, depth);
		glEnd();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<rubber_band>factory(
			k3d::uuid(0x72d36e00, 0x0a4621f4, 0xfdc8e69d, 0x621eadc8),
			"RubberBand",
			_("Displays a rubber-band box in the viewport."),
			"Annotation",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::icamera*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_camera;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_opacity;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_border_color;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_border_opacity;
	k3d_data(k3d::rectangle, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_rectangle;
};

/////////////////////////////////////////////////////////////////////////////
// rubber_band_factory

k3d::iplugin_factory& rubber_band_factory()
{
	return rubber_band::get_factory();
}

} // namespace highlighting

} // namespace module

