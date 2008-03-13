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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/idisplacement_shader_ri.h>
#include <k3dsdk/iimager_shader_ri.h>
#include <k3dsdk/ilight_shader_ri.h>
#include <k3dsdk/inetwork_render_farm.h>
#include <k3dsdk/inetwork_render_frame.h>
#include <k3dsdk/inetwork_render_job.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/istream_ri.h>
#include <k3dsdk/irender_preview.h>
#include <k3dsdk/ishader_collection_ri.h>
#include <k3dsdk/isurface_shader_ri.h>
#include <k3dsdk/ivolume_shader_ri.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/network_render_farm.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/property_group_collection.h>
#include <k3dsdk/stream_ri.h>
#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/shader_cache.h>
#include <k3dsdk/shader_collection_ri.h>
#include <k3dsdk/shader_ri.h>
#include <k3dsdk/share.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// displacement_shader

/// Implements a RenderMan displacement shader plugin
class displacement_shader :
	public k3d::ri::shader,
	public k3d::ri::idisplacement_shader
{
	typedef k3d::ri::shader base;

public:
	displacement_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::DISPLACEMENT)
	{
	}

	void setup_renderman_displacement_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiDisplacementV(shader_path(), shader_name(), shader_arguments(State));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<displacement_shader,
				k3d::interface_list<k3d::ri::idisplacement_shader> > factory(
			k3d::classes::RenderManDisplacementShader(),
			"RenderManDisplacementShader",
			_("Encapsulates a RenderMan displacement shader instance"),
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// imager_shader

/// Implements a RenderMan imager shader plugin
class imager_shader :
	public k3d::ri::shader,
	public k3d::ri::iimager_shader
{
	typedef k3d::ri::shader base;

public:
	imager_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::IMAGER)
	{
	}

	void setup_renderman_imager_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiImagerV(shader_path(), shader_name(), shader_arguments(State));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<imager_shader,
				k3d::interface_list<k3d::ri::iimager_shader> > factory(
			k3d::classes::RenderManImagerShader(),
			"RenderManImagerShader",
			"Encapsulates a RenderMan imager shader instance",
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// light_shader

/// Implements a RenderMan light shader plugin
class light_shader :
	public k3d::ri::shader,
	public k3d::ri::ilight_shader
{
	typedef k3d::ri::shader base;

public:
	light_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::LIGHT)
	{
	}

	void setup_renderman_light_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiLightSourceV(shader_path(), shader_name(), shader_arguments(State));
	}

	void setup_renderman_area_light_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiAreaLightSourceV(shader_path(), shader_name(), shader_arguments(State));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<light_shader,
				k3d::interface_list<k3d::ri::ilight_shader> > factory(
			k3d::classes::RenderManLightShader(),
			"RenderManLightShader",
			"Encapsulates a RenderMan light shader instance",
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// surface_shader

/// Implements a RenderMan surface shader plugin
class surface_shader :
	public k3d::ri::shader,
	public k3d::ri::isurface_shader,
	public k3d::property_group_collection
{
	typedef k3d::ri::shader base;

public:
	surface_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::SURFACE)
	{
	}

	void setup_renderman_surface_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiSurfaceV(shader_path(), shader_name(), shader_arguments(State));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<surface_shader, k3d::interface_list<k3d::ri::isurface_shader> > factory(
			k3d::classes::RenderManSurfaceShader(),
			"RenderManSurfaceShader",
			"Encapsulates a RenderMan surface shader instance",
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// volume_shader

/// Implements a RenderMan volume shader plugin
class volume_shader :
	public k3d::ri::shader,
	public k3d::ri::ivolume_shader
{
	typedef k3d::ri::shader base;

public:
	volume_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::VOLUME)
	{
	}

	void setup_renderman_atmosphere_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiAtmosphereV(shader_path(), shader_name(), shader_arguments(State));
	}

	void setup_renderman_interior_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiInteriorV(shader_path(), shader_name(), shader_arguments(State));
	}

	void setup_renderman_exterior_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.stream.RiExteriorV(shader_path(), shader_name(), shader_arguments(State));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<volume_shader,
				k3d::interface_list<k3d::ri::ivolume_shader> > factory(
			k3d::classes::RenderManVolumeShader(),
			"RenderManVolumeShader",
			"Encapsulates a RenderMan volume shader instance",
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// shader factories

k3d::iplugin_factory& displacement_shader_factory()
{
	return displacement_shader::get_factory();
}

k3d::iplugin_factory& imager_shader_factory()
{
	return imager_shader::get_factory();
}

k3d::iplugin_factory& light_shader_factory()
{
	return light_shader::get_factory();
}

k3d::iplugin_factory& surface_shader_factory()
{
	return surface_shader::get_factory();
}

k3d::iplugin_factory& volume_shader_factory()
{
	return volume_shader::get_factory();
}

} // namespace libk3drenderman

