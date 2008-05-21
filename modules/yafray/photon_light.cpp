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
// photon_light

class photon_light :
	public colored_light<photons<light<k3d::gl::renderable<k3d::transformable<k3d::node > > > > >
{
	typedef colored_light<photons<light<k3d::gl::renderable<k3d::transformable<k3d::node > > > > > base;

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

	void setup_light(const k3d::string_t& Name, std::ostream& Stream)
	{
		if(!m_emit.pipeline_value())
			return;

		const k3d::color color = m_color.pipeline_value();

		const k3d::point3 from = k3d::world_position(*this);
		const k3d::point3 to = from + k3d::node_to_world_matrix(*this) * k3d::vector3(0, 0, 1);

		Stream << "<!-- K-3D plugin: " << factory().name() << " name: " << name() << " -->\n";
		Stream << "<light type=\"photonlight\" name=\"" << Name << "\" power=\"" << m_power.pipeline_value() << "\" photons=\"" << m_photons.pipeline_value() << "\" depth=\"" << m_depth.pipeline_value() << "\" mindepth=\"" << m_mindepth.pipeline_value() << "\" bias=\"" << m_bias.pipeline_value() << "\" search=\"" << m_search.pipeline_value() << "\" angle=\"" << m_angle.pipeline_value() << "\" mode=\"" << (m_mode.pipeline_value() == DIFFUSE ? "diffuse" : "caustic") << "\" fixedradius=\"" << m_fixedradius.pipeline_value() << "\" cluster=\"" << m_cluster.pipeline_value() << "\" use_QMC=\"" << (m_use_QMC.pipeline_value() ? "on" : "off") << "\">\n";
		Stream << "	<from  x =\"" << -from[0] << "\" y =\"" << from[1] << "\" z =\"" << from[2] << "\"/>\n";
		Stream << "	<to  x =\"" << -to[0] << "\" y =\"" << to[1] << "\" z =\"" << to[2] << "\"/>\n";
		Stream << "	<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>\n";
		Stream << "</light>\n";
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::gl::store_attributes attributes;

		const bool emitting = m_emit.pipeline_value();

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
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_mindepth;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bias;
};

k3d::iplugin_factory& photon_light_factory()
{
	return photon_light::get_factory();
}

} // namespace yafray

} // namespace module


