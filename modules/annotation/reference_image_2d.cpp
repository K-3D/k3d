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

#include <k3dsdk/bitmap.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/gl.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idrawable_gl.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/render_state_gl.h>
#include <k3dsdk/utility_gl.h>

namespace libk3dannotation
{

/////////////////////////////////////////////////////////////////////////////
// reference_image_2d

class reference_image_2d :
	public k3d::persistent<k3d::node>,
	public k3d::gl::idrawable
{
	typedef k3d::persistent<k3d::node> base;

public:
	reference_image_2d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(1, 1, 1))),
		m_opacity(init_owner(*this) + init_name("opacity") + init_label(_("Opacity")) + init_description(_("Opacity")) + init_value(0.1)),
		m_camera(init_owner(*this) + init_name("camera") + init_label(_("Camera")) + init_description(_("Camera")) + init_value<k3d::icamera*>(0)),
		m_bitmap(init_owner(*this) + init_name("bitmap") + init_label(_("Bitmap")) + init_description(_("Bitmap")) + init_value<k3d::bitmap*>(0)),
		m_aspect_ratio_type(init_owner(*this) + init_name("aspect_ratio_type") + init_label(_("Aspect Ratio Type")) + init_description(_("Aspect Ratio Type")) + init_value(IMAGE) + init_enumeration(aspect_ratio_type_values())),
		m_aspect_ratio(init_owner(*this) + init_name("aspect_ratio") + init_label(_("Aspect Ratio")) + init_description(_("Aspect Ratio")) + init_value(1.0)),
		m_texture_name(0)
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_opacity.changed_signal().connect(make_async_redraw_slot());
		m_camera.changed_signal().connect(make_async_redraw_slot());
		m_bitmap.changed_signal().connect(sigc::mem_fun(*this, &reference_image_2d::bitmap_changed));
		m_aspect_ratio_type.changed_signal().connect(make_async_redraw_slot());
		m_aspect_ratio.changed_signal().connect(make_async_redraw_slot());
	}

	~reference_image_2d()
	{
		if(m_texture_name)
			glDeleteTextures(1, &m_texture_name);
	}

	void bitmap_changed(iunknown* Hint)
	{
		if(m_texture_name)
		{
			glDeleteTextures(1, &m_texture_name);
			m_texture_name = 0;
		}

		async_redraw(Hint);
	}

	sigc::slot<void, iunknown*> make_async_redraw_slot()
	{
		return sigc::mem_fun(*this, &reference_image_2d::async_redraw);
	}

	void async_redraw(iunknown*)
	{
		k3d::gl::redraw_all(document(), k3d::gl::irender_engine::ASYNCHRONOUS);
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
		k3d::bitmap* const bitmap = m_bitmap.pipeline_value();
		if(!(bitmap && bitmap->width() && bitmap->height()))
			return;

		const k3d::color color = m_color.pipeline_value();
		const double opacity = m_opacity.pipeline_value();
		const aspect_ratio_type_t aspect_ratio_type = m_aspect_ratio_type.pipeline_value();
		const double aspect_ratio = m_aspect_ratio.pipeline_value();

		glColor4d(color.red, color.green, color.blue, opacity);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glScaled(1, 1, -1);

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthMask(GL_FALSE);

		if(0 == m_texture_name)
		{
			glGenTextures(1, &m_texture_name);
			glBindTexture(GL_TEXTURE_2D, m_texture_name);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			k3d::gl::tex_image_2d(*bitmap);
		}

		if(m_texture_name)
		{
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, m_texture_name);

			double camera_left = 0;
			double camera_right = 0;
			double camera_top = 0;
			double camera_bottom = 0;
			double camera_far = 0;
			
			if(State.orthographic)
			{
				camera_left = State.gl_camera_frustum_left;
				camera_right = State.gl_camera_frustum_right;
				camera_top = State.gl_camera_frustum_top;
				camera_bottom = State.gl_camera_frustum_bottom;
				camera_far = State.gl_camera_frustum_far;
			}
			else
			{
				const double q = State.gl_camera_frustum_far / State.gl_camera_frustum_near;

				camera_left = State.gl_camera_frustum_left * q;
				camera_right = State.gl_camera_frustum_right * q;
				camera_top = State.gl_camera_frustum_top * q;
				camera_bottom = State.gl_camera_frustum_bottom * q;
				camera_far = State.gl_camera_frustum_far;
			}

			double draw_left = 0;
			double draw_right = 0;
			double draw_top = 0;
			double draw_bottom = 0;
			double draw_z = 0;
	
			const double camera_ratio = (camera_right - camera_left) / (camera_top - camera_bottom);
			double target_ratio = 0;

			switch(aspect_ratio_type)
			{
				case IMAGE:
					target_ratio = static_cast<double>(bitmap->width()) / static_cast<double>(bitmap->height());
					break;
				case CAMERA:
					target_ratio = camera_ratio;
					break;
			}

			target_ratio *= aspect_ratio;

			if(target_ratio > camera_ratio)
			{
				draw_left = camera_left;
				draw_right = camera_right;
				draw_top = camera_top * camera_ratio / target_ratio;
				draw_bottom = camera_bottom * camera_ratio / target_ratio;
				draw_z = camera_far * 0.99;
			}
			else
			{
				draw_left = camera_left * target_ratio / camera_ratio;
				draw_right = camera_right * target_ratio / camera_ratio;
				draw_top = camera_top;
				draw_bottom = camera_bottom;
				draw_z = camera_far * 0.99;
			}

			glBegin(GL_QUADS);
				glTexCoord2d(0, 0);
				glVertex3d(draw_left, draw_top, draw_z);
				glTexCoord2d(1, 0);
				glVertex3d(draw_right, draw_top, draw_z);
				glTexCoord2d(1, 1);
				glVertex3d(draw_right, draw_bottom, draw_z);
				glTexCoord2d(0, 1);
				glVertex3d(draw_left, draw_bottom, draw_z);
			glEnd();
		}
		
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<reference_image_2d>factory(
			k3d::uuid(0x77c55d01, 0x138e45bd, 0xbb4aefa2, 0xfe2911b2),
			"ReferenceImage2D",
			_("Displays a bitmap image in the background of the viewport."),
			"Annotation",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		IMAGE,
		CAMERA,
	} aspect_ratio_type_t;

	static const k3d::ienumeration_property::enumeration_values_t& aspect_ratio_type_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Image"), "image", _("Maintain the image aspect ratio")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Camera"), "camera", _("Stretch the image to fill the camera aspect ratio")));
		}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const aspect_ratio_type_t& Value)
	{
		switch(Value)
		{
			case IMAGE:
				Stream << "image";
				break;
			case CAMERA:
				Stream << "camera";
				break;
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, aspect_ratio_type_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "image")
			Value = IMAGE;
		else if(text == "camera")
			Value = CAMERA;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_opacity;
	k3d_data(k3d::icamera*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_camera;
	k3d_data(k3d::bitmap*, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_bitmap;
	k3d_data(aspect_ratio_type_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_aspect_ratio_type;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_aspect_ratio;

	GLuint m_texture_name;
};

/////////////////////////////////////////////////////////////////////////////
// reference_image_2d_factory

k3d::iplugin_factory& reference_image_2d_factory()
{
	return reference_image_2d::get_factory();
}

} // namespace libk3dannotation

