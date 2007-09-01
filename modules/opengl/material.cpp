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
		\brief Implements the Material K-3D object, which controls the surface appearance of rendered geometry
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_gl.h>
#include <k3dsdk/node.h>
#include <k3dsdk/node_change_signal.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility_gl.h>
#include <k3dsdk/vectors.h>

namespace module
{

namespace opengl
{

/////////////////////////////////////////////////////////////////////////////
// material

class material :
	public k3d::node_change_signal<k3d::persistent<k3d::node> >,
	public k3d::imaterial,
	public k3d::gl::imaterial
{
	typedef k3d::node_change_signal<k3d::persistent<k3d::node> > base;

public:
	material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_ambient_color(init_owner(*this) + init_name("ambient_color") + init_label(_("Ambient Color")) + init_description(_("Ambient Color")) + init_value(k3d::color(0, 0, 0))),
		m_diffuse_color(init_owner(*this) + init_name("diffuse_color") + init_label(_("Diffuse Color")) + init_description(_("Diffuse Color")) + init_value(k3d::color(1, 1, 1))),
		m_specular_color(init_owner(*this) + init_name("specular_color") + init_label(_("Specular Color")) + init_description(_("Specular Color")) + init_value(k3d::color(0, 0, 0))),
		m_emission_color(init_owner(*this) + init_name("emission_color") + init_label(_("Emission Color")) + init_description(_("Emission Color")) + init_value(k3d::color(0, 0, 0))),
		m_shininess(init_owner(*this) + init_name("shininess") + init_label(_("Shininess")) + init_description(_("Shininess")) + init_value(64.0) + init_constraint(constraint::minimum(0.0, constraint::maximum(128.0))))
	{
		m_ambient_color.changed_signal().connect(make_node_change_slot());
		m_diffuse_color.changed_signal().connect(make_node_change_slot());
		m_specular_color.changed_signal().connect(make_node_change_slot());
		m_emission_color.changed_signal().connect(make_node_change_slot());
		m_shininess.changed_signal().connect(make_node_change_slot());
	}

	k3d::gl::imaterial* gl_material()
	{
		return this;
	}

	k3d::ri::imaterial* ri_material()
	{
		return 0;
	}

	k3d::yafray::imaterial* yafray_material()
	{
		return 0;
	}

	void setup_gl_material()
	{
		k3d::gl::material(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambient_color.pipeline_value());
		k3d::gl::material(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuse_color.pipeline_value());
		k3d::gl::material(GL_FRONT_AND_BACK, GL_SPECULAR, m_specular_color.pipeline_value());
		k3d::gl::material(GL_FRONT_AND_BACK, GL_EMISSION, m_emission_color.pipeline_value());
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shininess.pipeline_value());

		glDisable(GL_POLYGON_STIPPLE);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<material,
				k3d::interface_list<k3d::imaterial,
				k3d::interface_list<k3d::gl::imaterial> > > factory(
			k3d::classes::OpenGLMaterial(),
			"OpenGLMaterial",
			_("OpenGL Material"),
			"OpenGL Materials",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_ambient_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_diffuse_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_specular_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_emission_color;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_shininess;
};

/////////////////////////////////////////////////////////////////////////////
// material_factory

k3d::iplugin_factory& material_factory()
{
	return material::get_factory();
}

} // namespace opengl

} // namespace module

