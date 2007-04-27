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

#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/icamera_preview_render_engine.h>
#include <k3dsdk/idisplacement_shader_ri.h>
#include <k3dsdk/iimager_shader_ri.h>
#include <k3dsdk/ilight_shader_ri.h>
#include <k3dsdk/ishader_collection_ri.h>
#include <k3dsdk/ipreview_render_engine.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/irender_farm.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/irender_job.h>
#include <k3dsdk/isurface_shader_ri.h>
#include <k3dsdk/ivolume_shader_ri.h>
#include <k3dsdk/shader_collection_ri.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/property.h>
#include <k3dsdk/property_group_collection.h>
#include <k3dsdk/shader_ri.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/render_engine_ri.h>
#include <k3dsdk/render_farm.h>
#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/shader_cache.h>
#include <k3dsdk/shaders.h>
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
		State.engine.RiDisplacementV(shader_path(), shader_name(), shader_arguments(State));
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
		State.engine.RiImagerV(shader_path(), shader_name(), shader_arguments(State));
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
		State.engine.RiLightSourceV(shader_path(), shader_name(), shader_arguments(State));
	}

	void setup_renderman_area_light_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.engine.RiAreaLightSourceV(shader_path(), shader_name(), shader_arguments(State));
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
	public k3d::ipreview_render_engine,
	public k3d::property_group_collection
{
	typedef k3d::ri::shader base;

public:
	surface_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, k3d::sl::shader::SURFACE),
		m_left(init_owner(*this) + init_name("left") + init_label(_("Left")) + init_description(_("Left")) + init_value(-2.0/3.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_right(init_owner(*this) + init_name("right") + init_label(_("Right")) + init_description(_("Right")) + init_value(2.0/3.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_top(init_owner(*this) + init_name("top") + init_label(_("Top")) + init_description(_("Top")) + init_value(0.5) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_bottom(init_owner(*this) + init_name("bottom") + init_label(_("Bottom")) + init_description(_("Bottom")) + init_value(-0.5) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_near(init_owner(*this) + init_name("near") + init_label(_("Near")) + init_description(_("Near Plane Distance")) + init_value(1.0) + init_constraint(constraint::minimum(0.0)) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_far(init_owner(*this) + init_name("far") + init_label(_("Far")) + init_description(_("Far Plane Distance")) + init_value(1000.0) + init_constraint(constraint::minimum(0.0)) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_perspective_projection(m_left, m_right, m_top, m_bottom, m_near, m_far)
	{
		k3d::iproperty_group_collection::group preview_group("Preview");
		preview_group.properties.push_back(&static_cast<k3d::iproperty&>(m_left));
		preview_group.properties.push_back(&static_cast<k3d::iproperty&>(m_right));
		preview_group.properties.push_back(&static_cast<k3d::iproperty&>(m_top));
		preview_group.properties.push_back(&static_cast<k3d::iproperty&>(m_bottom));
		preview_group.properties.push_back(&static_cast<k3d::iproperty&>(m_near));
		preview_group.properties.push_back(&static_cast<k3d::iproperty&>(m_far));

		register_property_group(preview_group);
	}

	void setup_renderman_surface_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.engine.RiSurfaceV(shader_path(), shader_name(), shader_arguments(State));
	}

	bool render_preview()
	{
		// Start a new render job
		k3d::irender_job& job = k3d::render_farm().create_job("k3d-shader-preview");

		// Add a single frame to the job
		k3d::irender_frame& frame = job.create_frame("frame");

		// Start our RIB file
		const std::string ribfilename("world.rib");
		const k3d::filesystem::path ribfilepath = frame.add_input_file(ribfilename);

		// Open the RIB file stream
		k3d::filesystem::ofstream ribfile(ribfilepath);

		// Setup RenderMan engine
		k3d::inode* renderman_engine = 0;
		const k3d::nodes_t render_engines = k3d::find_nodes<k3d::icamera_preview_render_engine>(document().nodes());
		for(k3d::nodes_t::const_iterator object = render_engines.begin(); object != render_engines.end(); ++object)
		{
			if(k3d::classes::RenderManEngine() == (*object)->factory().class_id())
			{
				renderman_engine = *object;
				break;
			}
		}

		return_val_if_fail(renderman_engine, false);

		// Get render engine name
		k3d::iproperty* const engine_property = k3d::get_property(*renderman_engine, "render_engine");
		return_val_if_fail(engine_property, false);
		const std::string render_engine = boost::any_cast<std::string>(engine_property->property_value());

		// Create the RenderMan render engine object
		k3d::ri::render_engine engine(ribfile);

		// Setup frame
		frame.add_render_operation("ri", render_engine, k3d::filesystem::native_path(k3d::ustring::from_utf8(ribfilename)), true);

		k3d::ri::shader_collection unused;

		// Create the render state object
		k3d::ri::sample_times_t samples(1, 0.0);
		const k3d::matrix4 transform_matrix = k3d::translation3D(k3d::point3(-20, 20, -20)) * k3d::rotation3D(k3d::point3(35, 45, 0));
		k3d::ri::render_state state(frame, engine, unused, m_perspective_projection, k3d::ri::FINAL_FRAME, samples, 0, transform_matrix);

		// Setup options
		k3d::ri::parameter_list searchpath_options;
		searchpath_options.push_back(
			k3d::ri::parameter(
				"shader",
				k3d::ri::UNIFORM,
				1,
				k3d::ri::string(k3d::shader_cache_path().native_filesystem_string() + ":&")));
		engine.RiOptionV("searchpath", searchpath_options);

		engine.RiDisplayV("outputimage", k3d::ri::RI_FRAMEBUFFER(), k3d::ri::RI_RGB());
		engine.RiFormat(320, 240, 1);

		// Create RenderMan scene
		const double left = boost::any_cast<double>(k3d::get_value(document().dag(), m_perspective_projection.left()));
		const double right = boost::any_cast<double>(k3d::get_value(document().dag(), m_perspective_projection.right()));
		const double top = boost::any_cast<double>(k3d::get_value(document().dag(), m_perspective_projection.top()));
		const double bottom = boost::any_cast<double>(k3d::get_value(document().dag(), m_perspective_projection.bottom()));
		const double near = boost::any_cast<double>(k3d::get_value(document().dag(), m_perspective_projection.near()));
		const double far = boost::any_cast<double>(k3d::get_value(document().dag(), m_perspective_projection.far()));
		return_val_if_fail(near > 0, false);

		engine.RiProjectionV("perspective");
		engine.RiScreenWindow(left / near, right / near, bottom / near, top / near);
		engine.RiClipping(near, far);

		engine.RiRotate(0, 0, 0, 1);
		engine.RiRotate(-35, 1, 0, 0);
		engine.RiRotate(-90, 0, 1, 0);
		engine.RiTranslate(10, -7, 0);

		engine.RiWorldBegin();

		// Setup a light
		engine.RiTransformBegin();
		engine.RiTransform(k3d::ri::convert(k3d::translation3D(k3d::point3(-20, 20, -25))));
		k3d::ri::parameter_list parameters;
		parameters.push_back(k3d::ri::parameter(std::string("intensity"), k3d::ri::CONSTANT, 1, (k3d::ri::real)3000.0));
		parameters.push_back(k3d::ri::parameter(std::string("lightcolor"), k3d::ri::CONSTANT, 1, (k3d::ri::color)k3d::color(1, 1, 1)));
		engine.RiLightSourceV(k3d::share_path() / k3d::filesystem::generic_path("shaders/k3d_pointlight.sl"), "k3d_pointlight", parameters);
		engine.RiTransformEnd();

		// Setup shader
		setup_renderman_surface_shader(state);

		// Default object
		engine.RiCylinderV(5.0, -5.0, 5.0, 360.0);

		engine.RiWorldEnd();


		// Synchronize shader
		if(!k3d::compile_shader(shader_path(), "ri", render_engine))
			k3d::log() << error << k3d::string_cast(boost::format(_("Error compiling shader %1%")) % shader_path().native_utf8_string().raw()) << std::endl;

		// Run the job
		k3d::render_farm().start_job(job);

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<surface_shader,
				k3d::interface_list<k3d::ri::isurface_shader> > factory(
			k3d::classes::RenderManSurfaceShader(),
			"RenderManSurfaceShader",
			"Encapsulates a RenderMan surface shader instance",
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, no_serialization) m_left;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, no_serialization) m_right;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, no_serialization) m_top;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, no_serialization) m_bottom;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, with_constraint, measurement_property, no_serialization) m_near;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, with_constraint, measurement_property, no_serialization) m_far;

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

	perspective_projection m_perspective_projection;
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
		State.engine.RiAtmosphereV(shader_path(), shader_name(), shader_arguments(State));
	}

	void setup_renderman_interior_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.engine.RiInteriorV(shader_path(), shader_name(), shader_arguments(State));
	}

	void setup_renderman_exterior_shader(const k3d::ri::render_state& State)
	{
		State.shaders.use_shader(shader_path());
		State.engine.RiExteriorV(shader_path(), shader_name(), shader_arguments(State));
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

