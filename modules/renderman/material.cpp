// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/idisplacement_shader_ri.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_ri.h>
#include <k3dsdk/isurface_shader_ri.h>
#include <k3dsdk/ivolume_shader_ri.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/node_change_signal.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/vectors.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// material

const std::string shadowtype_none = "None";
const std::string shadowtype_opaque = "Opaque";
const std::string shadowtype_opacity = "Opacity";
const std::string shadowtype_shaded = "Shaded";

class material :
	public k3d::node,
	public k3d::node_change_signal<material>,
	public k3d::imaterial,
	public k3d::ri::imaterial
{
	typedef k3d::node  base;

public:
	material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_surface_shader(init_owner(*this) + init_name("surface_shader") + init_label(_("Surface Shader")) + init_description(_("Surface shader")) + init_value<k3d::ri::isurface_shader*>(0)),
		m_displacement_shader(init_owner(*this) + init_name("displacement_shader") + init_label(_("Displacement Shader")) + init_description(_("Displacement shader")) + init_value<k3d::ri::idisplacement_shader*>(0)),
		m_atmosphere_shader(init_owner(*this) + init_name("atmosphere_shader") + init_label(_("Atmosphere Shader")) + init_description(_("Atmosphere shader")) + init_value<k3d::ri::ivolume_shader*>(0)),
		m_interior_shader(init_owner(*this) + init_name("interior_shader") + init_label(_("Interior Shader")) + init_description(_("Interior shader")) + init_value<k3d::ri::ivolume_shader*>(0)),
		m_exterior_shader(init_owner(*this) + init_name("exterior_shader") + init_label(_("Exterior Shader")) + init_description(_("Exterior shader")) + init_value<k3d::ri::ivolume_shader*>(0)),
		m_matte(init_owner(*this) + init_name("matte") + init_label(_("Matte")) + init_description(_("Render geometry as a matte")) + init_value(false)),
		m_displacement_bounds(init_owner(*this) + init_name("displacement_bounds") + init_label(_("Displacement Bounds")) + init_description(_("Displacement bounds")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(1, 1, 1))),
		m_opacity(init_owner(*this) + init_name("opacity") + init_label(_("Opacity")) + init_description(_("Opacity")) + init_value(k3d::color(1, 1, 1)))
	{
		m_surface_shader.changed_signal().connect(make_node_change_slot());
		m_displacement_shader.changed_signal().connect(make_node_change_slot());
		m_atmosphere_shader.changed_signal().connect(make_node_change_slot());
		m_interior_shader.changed_signal().connect(make_node_change_slot());
		m_exterior_shader.changed_signal().connect(make_node_change_slot());
		m_matte.changed_signal().connect(make_node_change_slot());
		m_displacement_bounds.changed_signal().connect(make_node_change_slot());
		m_color.changed_signal().connect(make_node_change_slot());
		m_opacity.changed_signal().connect(make_node_change_slot());

		deleted_signal().connect(sigc::mem_fun(*this, &material::on_deleted));
	}

	void on_deleted()
	{
	}

	void setup_renderman_material(const k3d::ri::render_state& State)
	{
		// We only generate RIB on the final sample ...
		if(!k3d::ri::last_sample(State))
			return;

		// Setup displacement bounds ...
		k3d::ri::parameter_list displacement_attributes;
		displacement_attributes.push_back(k3d::ri::parameter("sphere", k3d::ri::UNIFORM, 1, static_cast<k3d::ri::real>(m_displacement_bounds.pipeline_value())));
		displacement_attributes.push_back(k3d::ri::parameter("coordinatesystem", k3d::ri::UNIFORM, 1, k3d::ri::string("world")));
		State.stream.RiAttributeV("displacementbound", displacement_attributes);

		// Set base color
		State.stream.RiColor(m_color.pipeline_value());

		// Set opacity
		State.stream.RiOpacity(m_opacity.pipeline_value());

		// Set the matte attribute
		State.stream.RiMatte(m_matte.pipeline_value() ? 1 : 0);

		// Setup shaders ...
		if(m_surface_shader.pipeline_value())
			m_surface_shader.pipeline_value()->setup_renderman_surface_shader(State);
		if(m_displacement_shader.pipeline_value())
			m_displacement_shader.pipeline_value()->setup_renderman_displacement_shader(State);
		if(m_atmosphere_shader.pipeline_value())
			m_atmosphere_shader.pipeline_value()->setup_renderman_atmosphere_shader(State);
		if(m_interior_shader.pipeline_value())
			m_interior_shader.pipeline_value()->setup_renderman_interior_shader(State);
		if(m_exterior_shader.pipeline_value())
			m_exterior_shader.pipeline_value()->setup_renderman_exterior_shader(State);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<material,
				k3d::interface_list<k3d::imaterial,
				k3d::interface_list<k3d::ri::imaterial> > > factory(
			k3d::classes::RenderManMaterial(),
			"RenderManMaterial",
			_("A RenderMan surface / displacement material"),
			"RenderMan Material",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::ri::isurface_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_surface_shader;
	k3d_data(k3d::ri::idisplacement_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_displacement_shader;
	k3d_data(k3d::ri::ivolume_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_atmosphere_shader;
	k3d_data(k3d::ri::ivolume_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_interior_shader;
	k3d_data(k3d::ri::ivolume_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_exterior_shader;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_matte;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_displacement_bounds;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_opacity;
};

/////////////////////////////////////////////////////////////////////////////
// material_factory

k3d::iplugin_factory& material_factory()
{
	return material::get_factory();
}

} // namespace libk3drenderman

