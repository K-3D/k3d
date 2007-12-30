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
#include <k3dsdk/axis.h>
#include <k3dsdk/bitmap.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace annotation
{

/////////////////////////////////////////////////////////////////////////////
// reference_image_3d

class reference_image_3d :
	public k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > >
{
	typedef k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > > base;

public:
	reference_image_3d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(1, 1, 1))),
		m_opacity(init_owner(*this) + init_name("opacity") + init_label(_("Opacity")) + init_description(_("Opacity")) + init_value(0.3)),
		m_bitmap(init_owner(*this) + init_name("bitmap") + init_label(_("Bitmap")) + init_description(_("Bitmap")) + init_value<k3d::bitmap*>(0)),
		m_size(init_owner(*this) + init_name("size") + init_label(_("Size")) + init_description(_("Size")) + init_value(10.0)),
		m_aspect_ratio_type(init_owner(*this) + init_name("aspect_ratio_type") + init_label(_("Aspect Ratio Type")) + init_description(_("Aspect Ratio Type")) + init_value(ASPECT_RATIO_IMAGE) + init_enumeration(aspect_ratio_type_values())),
		m_aspect_ratio(init_owner(*this) + init_name("aspect_ratio") + init_label(_("Aspect Ratio")) + init_description(_("Aspect Ratio")) + init_value(1.0)),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values())),
		m_draw_behind(init_owner(*this) + init_name("draw_behind") + init_label(_("Draw Behind")) + init_description(_("Draw the image so that always appears behind geometry")) + init_value(true)),
		m_texture_name(0)
	{
		m_selection_weight.changed_signal().connect(make_async_redraw_slot());
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_opacity.changed_signal().connect(make_async_redraw_slot());
		m_bitmap.changed_signal().connect(sigc::mem_fun(*this, &reference_image_3d::bitmap_changed));
		m_size.changed_signal().connect(make_async_redraw_slot());
		m_aspect_ratio_type.changed_signal().connect(make_async_redraw_slot());
		m_aspect_ratio.changed_signal().connect(make_async_redraw_slot());
		m_orientation.changed_signal().connect(make_async_redraw_slot());
		m_draw_behind.changed_signal().connect(make_async_redraw_slot());
	}

	~reference_image_3d()
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

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		draw(State);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
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
		const double size = m_size.pipeline_value();
		const aspect_ratio_type_t aspect_ratio_type = m_aspect_ratio_type.pipeline_value();
		const double aspect_ratio = m_aspect_ratio.pipeline_value();
		const bool draw_behind = m_draw_behind.pipeline_value();

		glColor4d(color.red, color.green, color.blue, opacity);

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if(draw_behind)
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

			glMatrixMode(GL_MODELVIEW);

			switch(m_orientation.pipeline_value())
			{
				case k3d::PX:
					glRotated(-90, 0, 0, 1);
					glRotated(-90, 1, 0, 0);
					break;
				case k3d::NX:
					glRotated(90, 0, 0, 1);
					glRotated(-90, 1, 0, 0);
					break;
				case k3d::PY:
					glRotated(-90, 1, 0, 0);
					break;
				case k3d::NY:
					glRotated(180, 0, 0, 1);
					glRotated(-90, 1, 0, 0);
					break;
				case k3d::PZ:
					break;
				case k3d::NZ:
					glRotated(180, 1, 0, 0);
					break;
			}

			glScaled(size, size, 1);

			switch(aspect_ratio_type)
			{
				case ASPECT_RATIO_IMAGE:
					glScaled(static_cast<double>(bitmap->width()) / static_cast<double>(bitmap->height()), 1, 1);
					break;
				case ASPECT_RATIO_ABSOLUTE:
					break;
			}

			glScaled(aspect_ratio, 1, 1);

			glBegin(GL_QUADS);
				glTexCoord2d(0, 0);
				glVertex3d(-1, -1, 0);
				glTexCoord2d(1, 0);
				glVertex3d(1, -1, 0);
				glTexCoord2d(1, 1);
				glVertex3d(1, 1, 0);
				glTexCoord2d(0, 1);
				glVertex3d(-1, 1, 0);
			glEnd();
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<reference_image_3d>factory(
			k3d::uuid(0x0f6fa200, 0xa0094bc2, 0xa05379eb, 0x948f244f),
			"ReferenceImage3D",
			_("Displays a bitmap image on a resizable plane within the viewport."),
			"Annotation",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		ASPECT_RATIO_IMAGE,
		ASPECT_RATIO_ABSOLUTE,
	} aspect_ratio_type_t;

	static const k3d::ienumeration_property::enumeration_values_t& aspect_ratio_type_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Image"), "image", _("Maintain the image aspect ratio")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Absolute"), "absolute", _("Draw the image with a fixed aspect ratio, ignoring the image aspect ratio")));
		}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const aspect_ratio_type_t& Value)
	{
		switch(Value)
		{
			case ASPECT_RATIO_IMAGE:
				Stream << "image";
				break;
			case ASPECT_RATIO_ABSOLUTE:
				Stream << "absolute";
				break;
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, aspect_ratio_type_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "image")
			Value = ASPECT_RATIO_IMAGE;
		else if(text == "absolute")
			Value = ASPECT_RATIO_ABSOLUTE;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_opacity;
	k3d_data(k3d::bitmap*, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_bitmap;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_size;
	k3d_data(aspect_ratio_type_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_aspect_ratio_type;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_aspect_ratio;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_draw_behind;

	GLuint m_texture_name;
};

/////////////////////////////////////////////////////////////////////////////
// reference_image_3d_factory

k3d::iplugin_factory& reference_image_3d_factory()
{
	return reference_image_3d::get_factory();
}

} // namespace annotation

} // namespace module

