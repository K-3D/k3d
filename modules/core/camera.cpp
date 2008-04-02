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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/aspect_ratios.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/icrop_window.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/parentable.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property_group_collection.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/transformable.h>

#ifdef	WIN32
#ifdef	near
#undef	near
#endif	//near
#ifdef	far
#undef	far
#endif	//far
#endif	//WIN32

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// camera

class camera :
	public k3d::gl::renderable<k3d::parentable<k3d::transformable<k3d::persistent<k3d::node> > > >,
	public k3d::icamera,
	public k3d::icrop_window,
	public k3d::property_group_collection
{
	typedef k3d::gl::renderable<k3d::parentable<k3d::transformable<k3d::persistent<k3d::node> > > > base;

public:
	camera(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_navigation_target(init_owner(*this) + init_name("navigation_target") + init_label(_("Navigation Target")) + init_description(_("Navigation Target")) + init_value<k3d::itransform_source*>(this)),
		m_world_target(init_owner(*this) + init_name("world_target") + init_label(_("World target")) + init_description(_("World target")) + init_value(k3d::point3(0, -5, 0))),
		m_target_distance(init_owner(*this) + init_name("target_distance") + init_label(_("Target distance")) + init_description(_("Target distance from camera position")) + init_value(0) + init_units(typeid(k3d::measurement::distance)) + init_slot(sigc::mem_fun(*this, &camera::get_target_distance))),
		m_aspect_ratio(init_owner(*this) + init_name("aspect_ratio") + init_label(_("Aspect Ratio")) + init_description(_("Choose a predefined aspect ratio")) + init_enumeration(k3d::aspect_ratio_values()) + init_value(std::string(""))),
		m_show_projection(init_owner(*this) + init_name("show_projection") + init_label(_("Show Projection")) + init_description(_("Show viewing projection")) + init_value(false)),
		m_orthographic(init_owner(*this) + init_name("orthographic") + init_label(_("Orthographic")) + init_description(_("Orthographic projection")) + init_value(false)),
		m_left(init_owner(*this) + init_name("left") + init_label(_("Left")) + init_description(_("Left")) + init_value(-2.0/3.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_right(init_owner(*this) + init_name("right") + init_label(_("Right")) + init_description(_("Right")) + init_value(2.0/3.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_top(init_owner(*this) + init_name("top") + init_label(_("Top")) + init_description(_("Top")) + init_value(0.5) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_bottom(init_owner(*this) + init_name("bottom") + init_label(_("Bottom")) + init_description(_("Bottom")) + init_value(-0.5) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_near(init_owner(*this) + init_name("near") + init_label(_("Near")) + init_description(_("Near Plane Distance")) + init_value(1.0) + init_constraint(constraint::minimum<double>(0.0)) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_far(init_owner(*this) + init_name("far") + init_label(_("Far")) + init_description(_("Far Plane Distance")) + init_value(1000.0) + init_constraint(constraint::minimum<double>(0.0)) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_crop_window_left(init_owner(*this) + init_name("crop_window_left") + init_label(_("Crop Window Left")) + init_description(_("Crop window left")) + init_value(0.0) + init_constraint(constraint::minimum<double>(0.0, constraint::maximum<double>(1.0))) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_crop_window_right(init_owner(*this) + init_name("crop_window_right") + init_label(_("Crop Window Right")) + init_description(_("Crop Window Right")) + init_value(1.0) + init_constraint(constraint::minimum<double>(0.0, constraint::maximum<double>(1.0)))  + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_crop_window_top(init_owner(*this) + init_name("crop_window_top") + init_label(_("Crop Window Top")) + init_description(_("Crop Window Top")) + init_value(0.0) + init_constraint(constraint::minimum<double>(0.0, constraint::maximum<double>(1.0)))  + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_crop_window_bottom(init_owner(*this) + init_name("crop_window_bottom") + init_label(_("Crop Window Bottom")) + init_description(_("Crop window bottom")) + init_value(1.0) + init_constraint(constraint::minimum<double>(0.0, constraint::maximum<double>(1.0)))  + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_show_reference_plane(init_owner(*this) + init_name("show_reference_plane") + init_label(_("Show Reference Plane")) + init_description(_("Show Reference Plane")) + init_value(false)),
		m_reference_plane(init_owner(*this) + init_name("reference_plane") + init_label(_("Reference Plane")) + init_description(_("Reference Plane Distance")) + init_value(0.0) + init_constraint(constraint::minimum<double>(0.0))  + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_reference_plane_color(init_owner(*this) + init_name("reference_plane_color") + init_label(_("Reference Plane Color")) + init_description(_("Reference Plane Color")) + init_value(k3d::color(0, 0, 0.7))),
		m_perspective_projection(m_left, m_right, m_top, m_bottom, m_near, m_far),
		m_orthographic_projection(m_left, m_right, m_top, m_bottom, m_near, m_far)
	{
		k3d::iproperty_group_collection::group projection_group("Projection");
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_navigation_target));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_aspect_ratio));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_orthographic));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_left));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_right));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_top));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_bottom));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_near));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_far));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_show_reference_plane));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_reference_plane));
		projection_group.properties.push_back(&static_cast<k3d::iproperty&>(m_reference_plane_color));

		k3d::iproperty_group_collection::group output_group("Output");
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_crop_window_left));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_crop_window_right));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_crop_window_top));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_crop_window_bottom));

		register_property_group(projection_group);
		register_property_group(output_group);

		m_aspect_ratio.changed_signal().connect(sigc::mem_fun(*this, &camera::on_aspect_ratio_changed));

		m_show_projection.changed_signal().connect(make_async_redraw_slot());
		m_orthographic.changed_signal().connect(make_async_redraw_slot());
		m_left.changed_signal().connect(make_async_redraw_slot());
		m_right.changed_signal().connect(make_async_redraw_slot());
		m_top.changed_signal().connect(make_async_redraw_slot());
		m_bottom.changed_signal().connect(make_async_redraw_slot());
		m_near.changed_signal().connect(make_async_redraw_slot());
		m_far.changed_signal().connect(make_async_redraw_slot());
		m_show_reference_plane.changed_signal().connect(make_async_redraw_slot());
		m_reference_plane.changed_signal().connect(make_async_redraw_slot());
		m_reference_plane_color.changed_signal().connect(make_async_redraw_slot());
		m_crop_window_left.changed_signal().connect(make_async_redraw_slot());
		m_crop_window_right.changed_signal().connect(make_async_redraw_slot());
		m_crop_window_top.changed_signal().connect(make_async_redraw_slot());
		m_crop_window_bottom.changed_signal().connect(make_async_redraw_slot());
		m_input_matrix.changed_signal().connect(make_async_redraw_slot());

		m_input_matrix.changed_signal().connect(sigc::mem_fun(*this, &camera::on_position_changed));
		m_world_target.changed_signal().connect(sigc::mem_fun(*this, &camera::on_position_changed));
	}

	void set_selection_weight(const double Weight)
	{
		if(Weight != get_selection_weight())
			k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::ASYNCHRONOUS);

		base::set_selection_weight(Weight);
	}

	k3d::itransform_source& transformation()
	{
		return *this;
	}

	k3d::iprojection& projection()
	{
		const bool orthographic = m_orthographic.pipeline_value();

		if(orthographic)
			return m_orthographic_projection;

		return m_perspective_projection;
	}

	k3d::icrop_window& crop_window()
	{
		return *this;
	}

	k3d::itransform_source& navigation_target()
	{
		k3d::itransform_source* target = m_navigation_target.pipeline_value();
		if(!target)
			target = this;

		return *target;
	}

	k3d::iproperty& world_target()
	{
		return m_world_target;
	}

	k3d::iproperty& crop_left()
	{
		return m_crop_window_left;
	}

	k3d::iproperty& crop_right()
	{
		return m_crop_window_right;
	}

	k3d::iproperty& crop_top()
	{
		return m_crop_window_top;
	}

	k3d::iproperty& crop_bottom()
	{
		return m_crop_window_bottom;
	}

	void on_aspect_ratio_changed(k3d::iunknown*)
	{
		const std::string new_ratio = m_aspect_ratio.pipeline_value();

		const k3d::aspect_ratios_t& ratios = k3d::aspect_ratios();
		for(k3d::aspect_ratios_t::const_iterator ratio = ratios.begin(); ratio != ratios.end(); ++ratio)
		{
			if(ratio->name != new_ratio)
				continue;

			m_left.set_value(-0.5 * ratio->value);
			m_right.set_value(0.5 * ratio->value);
			m_top.set_value(0.5);
			m_bottom.set_value(-0.5);

			return;
		}

		assert_not_reached();
	}

	void on_position_changed(k3d::iunknown* const Hint)
	{
		m_target_distance.reset(Hint);
	}

	double get_target_distance()
	{
		const k3d::point3 position = k3d::node_to_world_matrix(navigation_target()) * k3d::point3(0, 0, 0);
		return k3d::distance(position, m_world_target.pipeline_value());
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);

		k3d::gl::color3d(get_selection_weight() ? k3d::color(1, 1, 1) : k3d::color(0, 0, 0));
		glLineWidth(1.0f);
		glDisable(GL_LINE_STIPPLE);

		draw();

		if(m_show_projection.pipeline_value())
			draw_projection();

		if(m_show_reference_plane.pipeline_value())
			draw_reference_plane();
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);
		draw();
		k3d::gl::pop_selection_token();
	}

	void draw_projection()
	{
		const bool orthographic = m_orthographic.pipeline_value();
		const double left = m_left.pipeline_value();
		const double right = m_right.pipeline_value();
		const double top = m_top.pipeline_value();
		const double bottom = m_bottom.pipeline_value();
		const double near = m_near.pipeline_value();
		const double far = m_far.pipeline_value();

		const double reference_plane = m_reference_plane.pipeline_value();
		const k3d::color reference_plane_color = m_reference_plane_color.pipeline_value();

		if(orthographic)
		{
			// Draw the near plane ...
			glBegin(GL_LINE_LOOP);
				glVertex3d(left, top, near);
				glVertex3d(right, top, near);
				glVertex3d(right, bottom, near);
				glVertex3d(left, bottom, near);
			glEnd();

			// Draw the reference plane ...
			glBegin(GL_LINE_LOOP);
				glVertex3d(left, top, reference_plane);
				glVertex3d(right, top, reference_plane);
				glVertex3d(right, bottom, reference_plane);
				glVertex3d(left, bottom, reference_plane);
			glEnd();

			// Draw the far plane ...
			glBegin(GL_LINE_LOOP);
				glVertex3d(left, top, far);
				glVertex3d(right, top, far);
				glVertex3d(right, bottom, far);
				glVertex3d(left, bottom, far);
			glEnd();

			// Draw the frustum ...
			glBegin(GL_LINES);
				glVertex3d(left, top, near);
				glVertex3d(left, top, far);

				glVertex3d(right, top, near);
				glVertex3d(right, top, far);

				glVertex3d(right, bottom, near);
				glVertex3d(right, bottom, far);

				glVertex3d(left, bottom, near);
				glVertex3d(left, bottom, far);
			glEnd();
		}
		else
		{
			const double p = far / near;
			const double left2 = left * p;
			const double right2 = right * p;
			const double top2 = top * p;
			const double bottom2 = bottom * p;

			const double q = reference_plane / near;
			const double left3 = left * q;
			const double right3 = right * q;
			const double top3 = top * q;
			const double bottom3 = bottom * q;

			// Draw the near plane ...
			glBegin(GL_LINE_LOOP);
				glVertex3d(left, top, near);
				glVertex3d(right, top, near);
				glVertex3d(right, bottom, near);
				glVertex3d(left, bottom, near);
			glEnd();

			// Draw the reference plane ...
			glBegin(GL_LINE_LOOP);
				glVertex3d(left3, top3, reference_plane);
				glVertex3d(right3, top3, reference_plane);
				glVertex3d(right3, bottom3, reference_plane);
				glVertex3d(left3, bottom3, reference_plane);
			glEnd();

			// Draw the far plane ...
			glBegin(GL_LINE_LOOP);
				glVertex3d(left2, top2, far);
				glVertex3d(right2, top2, far);
				glVertex3d(right2, bottom2, far);
				glVertex3d(left2, bottom2, far);
			glEnd();

			// Draw the frustum ...
			glBegin(GL_LINES);
				glVertex3d(left, top, near);
				glVertex3d(left2, top2, far);

				glVertex3d(right, top, near);
				glVertex3d(right2, top2, far);

				glVertex3d(right, bottom, near);
				glVertex3d(right2, bottom2, far);

				glVertex3d(left, bottom, near);
				glVertex3d(left2, bottom2, far);
			glEnd();
		}
	}

	void draw_reference_plane()
	{
		const bool orthographic = m_orthographic.pipeline_value();
		const double left = m_left.pipeline_value();
		const double right = m_right.pipeline_value();
		const double top = m_top.pipeline_value();
		const double bottom = m_bottom.pipeline_value();
		const double near = m_near.pipeline_value();

		const double reference_plane = m_reference_plane.pipeline_value();
		const k3d::color reference_plane_color = m_reference_plane_color.pipeline_value();

		if(orthographic)
		{
			k3d::gl::color3d(reference_plane_color);
			glBegin(GL_LINE_LOOP);
				glVertex3d(left, top, reference_plane);
				glVertex3d(right, top, reference_plane);
				glVertex3d(right, bottom, reference_plane);
				glVertex3d(left, bottom, reference_plane);
			glEnd();
		}
		else
		{
			const double q = reference_plane / near;
			const double left3 = left * q;
			const double right3 = right * q;
			const double top3 = top * q;
			const double bottom3 = bottom * q;

			k3d::gl::color3d(reference_plane_color);
			glBegin(GL_LINE_LOOP);
				glVertex3d(left3, top3, reference_plane);
				glVertex3d(right3, top3, reference_plane);
				glVertex3d(right3, bottom3, reference_plane);
				glVertex3d(left3, bottom3, reference_plane);
			glEnd();
		}
	}

	void draw()
	{
		// Our dimensions
		const double bodylength = 0.5 * 0.5;
		const double bodywidth = 0.25 * 0.5;
		const double bodyheight = 0.25 * 0.5;
		const double lenslength = 0.25 * 0.5;
		const double lenswidth = 0.15 * 0.5;
		const double lensheight = 0.1 * 0.5;
		const double lensoffset = 0.05;
		const double filmradius = 0.2;
		const double filmwidth = 0.125 * 0.5;

		glTranslated(0, 0, -0.25);

		// Draw the camera body ...
		glBegin(GL_LINE_LOOP);
		glVertex3d(-bodywidth, bodyheight, bodylength);
		glVertex3d(bodywidth, bodyheight, bodylength);
		glVertex3d(bodywidth, bodyheight, -bodylength);
		glVertex3d(-bodywidth, bodyheight, -bodylength);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3d(-bodywidth, -bodyheight, bodylength);
		glVertex3d(bodywidth, -bodyheight, bodylength);
		glVertex3d(bodywidth, -bodyheight, -bodylength);
		glVertex3d(-bodywidth, -bodyheight, -bodylength);
		glEnd();

		glBegin(GL_LINES);
		glVertex3d(-bodywidth, bodyheight, bodylength);
		glVertex3d(-bodywidth, -bodyheight, bodylength);
		glVertex3d(bodywidth, bodyheight, bodylength);
		glVertex3d(bodywidth, -bodyheight, bodylength);
		glVertex3d(bodywidth, bodyheight, -bodylength);
		glVertex3d(bodywidth, -bodyheight, -bodylength);
		glVertex3d(-bodywidth, bodyheight, -bodylength);
		glVertex3d(-bodywidth, -bodyheight, -bodylength);
		glEnd();

		// Draw the camera lens ...
		glBegin(GL_LINE_LOOP);
		glVertex3d(-lenswidth, lensheight, bodylength);
		glVertex3d(lenswidth, lensheight, bodylength);
		glVertex3d(lenswidth, -lensheight, bodylength);
		glVertex3d(-lenswidth, -lensheight, bodylength);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3d(-lenswidth - lensoffset, lensheight + lensoffset, bodylength + lenslength);
		glVertex3d(lenswidth + lensoffset, lensheight + lensoffset, bodylength + lenslength);
		glVertex3d(lenswidth + lensoffset, -lensheight - lensoffset, bodylength + lenslength);
		glVertex3d(-lenswidth - lensoffset, -lensheight - lensoffset, bodylength + lenslength);
		glEnd();

		glBegin(GL_LINES);
		glVertex3d(-lenswidth, lensheight, bodylength);
		glVertex3d(-lenswidth - lensoffset, lensheight + lensoffset, bodylength + lenslength);
		glVertex3d(lenswidth, lensheight, bodylength);
		glVertex3d(lenswidth + lensoffset, lensheight + lensoffset, bodylength + lenslength);
		glVertex3d(lenswidth, -lensheight, bodylength);
		glVertex3d(lenswidth + lensoffset, -lensheight - lensoffset, bodylength + lenslength);
		glVertex3d(-lenswidth, -lensheight, bodylength);
		glVertex3d(-lenswidth - lensoffset, -lensheight - lensoffset, bodylength + lenslength);
		glEnd();

		// Draw the film can ...
		glBegin(GL_LINE_LOOP);
		glVertex3d(-filmwidth, bodyheight, -bodylength);
		glVertex3d(-filmwidth, bodyheight, -bodylength + filmradius);
		glVertex3d(-filmwidth, bodyheight + 0.8 * filmradius, -bodylength + 0.8 * filmradius);
		glVertex3d(-filmwidth, bodyheight + filmradius, -bodylength);
		glVertex3d(-filmwidth, bodyheight + 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(-filmwidth, bodyheight, -bodylength - filmradius);
		glVertex3d(-filmwidth, bodyheight - 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(-filmwidth, bodyheight - filmradius, -bodylength);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3d(filmwidth, bodyheight, -bodylength);
		glVertex3d(filmwidth, bodyheight, -bodylength + filmradius);
		glVertex3d(filmwidth, bodyheight + 0.8 * filmradius, -bodylength + 0.8 * filmradius);
		glVertex3d(filmwidth, bodyheight + filmradius, -bodylength);
		glVertex3d(filmwidth, bodyheight + 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(filmwidth, bodyheight, -bodylength - filmradius);
		glVertex3d(filmwidth, bodyheight - 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(filmwidth, bodyheight - filmradius, -bodylength);
		glEnd();

		glBegin(GL_LINES);
		glVertex3d(-filmwidth, bodyheight, -bodylength + filmradius);
		glVertex3d(filmwidth, bodyheight, -bodylength + filmradius);
		glVertex3d(-filmwidth, bodyheight + 0.8 * filmradius, -bodylength + 0.8 * filmradius);
		glVertex3d(filmwidth, bodyheight + 0.8 * filmradius, -bodylength + 0.8 * filmradius);
		glVertex3d(-filmwidth, bodyheight + filmradius, -bodylength);
		glVertex3d(filmwidth, bodyheight + filmradius, -bodylength);
		glVertex3d(-filmwidth, bodyheight + 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(filmwidth, bodyheight + 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(-filmwidth, bodyheight, -bodylength - filmradius);
		glVertex3d(filmwidth, bodyheight, -bodylength - filmradius);
		glVertex3d(-filmwidth, bodyheight - 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(filmwidth, bodyheight - 0.8 * filmradius, -bodylength - 0.8 * filmradius);
		glVertex3d(-filmwidth, bodyheight - filmradius, -bodylength);
		glVertex3d(filmwidth, bodyheight - filmradius, -bodylength);
		glEnd();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<camera,
			k3d::interface_list<k3d::icamera,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink> > > > factory(
				k3d::classes::Camera(),
				"Camera",
				_("Camera"),
				"Camera",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	class perspective_projection :
		public k3d::iperspective
	{
	public:
		perspective_projection(k3d::iproperty& Left, k3d::iproperty& Right, k3d::iproperty& Top, k3d::iproperty& Bottom, k3d::iproperty& Near, k3d::iproperty& Far) :
			m_left(Left),
			m_right(Right),
			m_top(Top),
			m_bottom(Bottom),
			m_near(Near),
			m_far(Far)
		{
		}

		k3d::iproperty& left()
		{
			return m_left;
		}

		k3d::iproperty& right()
		{
			return m_right;
		}

		k3d::iproperty& top()
		{
			return m_top;
		}

		k3d::iproperty& bottom()
		{
			return m_bottom;
		}

		k3d::iproperty& near()
		{
			return m_near;
		}

		k3d::iproperty& far()
		{
			return m_far;
		}

	private:
		k3d::iproperty& m_left;
		k3d::iproperty& m_right;
		k3d::iproperty& m_top;
		k3d::iproperty& m_bottom;
		k3d::iproperty& m_near;
		k3d::iproperty& m_far;
	};

	class orthographic_projection :
		public k3d::iorthographic
	{
	public:
		orthographic_projection(k3d::iproperty& Left, k3d::iproperty& Right, k3d::iproperty& Top, k3d::iproperty& Bottom, k3d::iproperty& Near, k3d::iproperty& Far) :
			m_left(Left),
			m_right(Right),
			m_top(Top),
			m_bottom(Bottom),
			m_near(Near),
			m_far(Far)
		{
		}

		k3d::iproperty& left()
		{
			return m_left;
		}

		k3d::iproperty& right()
		{
			return m_right;
		}

		k3d::iproperty& top()
		{
			return m_top;
		}

		k3d::iproperty& bottom()
		{
			return m_bottom;
		}

		k3d::iproperty& near()
		{
			return m_near;
		}

		k3d::iproperty& far()
		{
			return m_far;
		}

	private:
		k3d::iproperty& m_left;
		k3d::iproperty& m_right;
		k3d::iproperty& m_top;
		k3d::iproperty& m_bottom;
		k3d::iproperty& m_near;
		k3d::iproperty& m_far;
	};

	k3d_data(k3d::itransform_source*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_navigation_target;
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_world_target;
	k3d_data(double, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_target_distance;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_aspect_ratio;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_show_projection;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_orthographic;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_left;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_right;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_top;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_bottom;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_near;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_far;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_crop_window_left;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_crop_window_right;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_crop_window_top;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_crop_window_bottom;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_show_reference_plane;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_reference_plane;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_reference_plane_color;

	perspective_projection m_perspective_projection;
	orthographic_projection m_orthographic_projection;
};

k3d::iplugin_factory& camera_factory()
{
	return camera::get_factory();
}

} // namespace libk3dcore

