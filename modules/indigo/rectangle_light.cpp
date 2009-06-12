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
	\author Tim Shead <tshead@k-3d.com>
*/

#include "light.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace indigo
{

/////////////////////////////////////////////////////////////////////////////
// rectangle_light

class rectangle_light :
	public k3d::gl::renderable<k3d::transformable<k3d::node> >,
	public indigo::light
{
	typedef k3d::gl::renderable<k3d::transformable<k3d::node> > base;

public:
	rectangle_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Width")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Height")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_power(init_owner(*this) + init_name("power") + init_label(_("Power")) + init_description(_("Power-drawn by the light source in Watts.")) + init_value(100.0)),
		m_luminous_efficacy(init_owner(*this) + init_name("luminous_efficacy") + init_label(_("Luminous Efficacy")) + init_description(_("Luminous efficacy of the light source in luminous flux per Watt.")) + init_value(17.5)), 
		m_blackbody_temperature(init_owner(*this) + init_name("blackbody_temperature") + init_label(_("Blackbody Temperature")) + init_description(_("Blackbody Temperature of the light source in degrees kelvin.")) + init_value(6500)), 
		m_blackbody_gain(init_owner(*this) + init_name("blackbody_gain") + init_label(_("Blackbody Gain")) + init_description(_("Blackbody Temperature Gain, multiplier.")) + init_value(1.0))
	{
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);

		k3d::gl::color3d(State.node_selection ? k3d::color(1, 1, 1) : k3d::color(1, 1, 0));
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
    const k3d::point3 center(0, 0, 0);
    const k3d::double_t width = 0.5 * m_width.pipeline_value();
    const k3d::double_t height = 0.5 * m_height.pipeline_value();

		glBegin(GL_LINE_LOOP);

		glVertex3d(center[0] - width, center[1] - height, center[2]);
		glVertex3d(center[0] + width, center[1] - height, center[2]);
		glVertex3d(center[0] + width, center[1] + height, center[2]);
		glVertex3d(center[0] - width, center[1] + height, center[2]);

		glEnd();
	}

  void setup(std::ostream& Stream)
  {
    const k3d::point3 pos = k3d::world_position(*this);

    Stream << "<rectanglelight>\n";
    Stream << "<pos>" << -pos[0] << " " << pos[1] << " " << pos[2] << "</pos>\n";
    Stream << "<width>" << m_width.pipeline_value() << "</width>\n";
    Stream << "<height>" << m_height.pipeline_value() << "</height>\n";
    Stream << "<spectrum> <blackbody> \n";
    Stream << "<temperature>" << m_blackbody_temperature.pipeline_value() << "</temperature>\n";
    Stream << "<gain>" << m_blackbody_gain.pipeline_value() << "</gain>\n";
    Stream << "</blackbody> </spectrum> \n";
    Stream << "<efficacy_scale>\n";
    Stream << "<power_drawn>" << m_power.pipeline_value() << "</power_drawn>\n";
    Stream << "<overall_luminous_efficacy>" << m_luminous_efficacy.pipeline_value() << "</overall_luminous_efficacy>\n";
    Stream << "</efficacy_scale>\n";
    Stream << "</rectanglelight>\n";
  }

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<rectangle_light> factory(
			k3d::uuid(0xfe05aa8d, 0xcf485202, 0x82a460ba, 0xab3184a3),
			"IndigoRectangleLight",
			_("Indigo Rectangle Light"),
			"Indigo Rectangle Light",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_power;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_luminous_efficacy;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blackbody_temperature;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blackbody_gain;
};

k3d::iplugin_factory& rectangle_light_factory()
{
	return rectangle_light::get_factory();
}

} // namespace indigo

} // namespace module

