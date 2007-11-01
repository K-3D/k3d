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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>
#include <k3dsdk/ilight_ri.h>
#include <k3dsdk/ilight_shader_ri.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/vectors.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// area_light

class area_light :
	public k3d::persistent<k3d::node>,
	public k3d::ri::ilight
{
	typedef k3d::persistent<k3d::node> base;

public:
	area_light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_shader(init_owner(*this) + init_name("shader") + init_label(_("Shader")) + init_description(_("Light shader")) + init_value<k3d::ri::ilight_shader*>(0)),
		m_geometry(init_owner(*this) + init_name("geometry") + init_label(_("Geometry")) + init_description(_("Geometry")) + init_value<k3d::ri::irenderable*>(0))
	{
	}

	void setup_renderman_light(const k3d::ri::render_state& State)
	{
		// If this is a shadow pass, we're done ...
		if(k3d::ri::SHADOW_MAP == State.render_context)
			return;

		// We only generate RIB on the last sample ...
		if(!k3d::ri::last_sample(State))
			return;

		// If the user hasn't selected a shader, we're done ...
		k3d::ri::ilight_shader* const shader = m_shader.pipeline_value();
		if(!shader)
			return;

		// If the user hasn't selected some geometry, we're done ...
		k3d::ri::irenderable* const geometry = m_geometry.pipeline_value();
		if(!geometry)
			return;

		// Make it happen ...
		k3d::ri::set_attributes(*this, State.stream);
		shader->setup_renderman_area_light_shader(State);
		geometry->renderman_render(State);
		State.stream.RiAreaLightSourceV(k3d::ri::path(), "null");
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<area_light,
			k3d::interface_list<k3d::ri::ilight> > factory(
			k3d::uuid(0xd7cab1c3, 0x60214984, 0x995601b5, 0x4dc8d62e),
			"RenderManAreaLight",
			_("RenderMan area light source"),
			"RenderMan Lights",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::ri::ilight_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_shader;
	k3d_data(k3d::ri::irenderable*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_geometry;
};

/////////////////////////////////////////////////////////////////////////////
// area_light_factory

k3d::iplugin_factory& area_light_factory()
{
	return area_light::get_factory();
}

} // namespace libk3drenderman

