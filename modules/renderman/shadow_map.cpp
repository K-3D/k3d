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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/inetwork_render_frame.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/istream_ri.h>
#include <k3dsdk/itexture_ri.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/resolutions.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/vectors.h>

namespace libk3drenderman
{

/// Implements the ShadowMap plugin, which is used to generate RenderMan shadow maps for fast rendering
/////////////////////////////////////////////////////////////////////////////
// shadow_map

class shadow_map :
	public k3d::persistent<k3d::node>,
	public k3d::ri::itexture
{
	typedef k3d::persistent<k3d::node> base;

public:
	shadow_map(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_camera(init_owner(*this) + init_name("camera") + init_label(_("Camera")) + init_description(_("Shadow Map Camera")) + init_value<k3d::icamera*>(0)),
		m_create_shadow_map(init_owner(*this) + init_name("create") + init_label(_("create")) + init_description(_("Create shadow map during rendering")) + init_value(true)),
		m_view_shadow_map(init_owner(*this) + init_name("view") + init_label(_("view")) + init_description(_("View shadow map creation during rendering")) + init_value(true)),
		m_resolution(init_owner(*this) + init_name("resolution") + init_label(_("Resolution")) + init_description(_("Choose a predefined image resolution")) + init_enumeration(k3d::resolution_values()) + init_value(std::string(""))),
		m_pixel_width(init_owner(*this) + init_name("pixel_width") + init_label(_("pixel_width")) + init_description(_("Output pixel width")) + init_value(256) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_pixel_height(init_owner(*this) + init_name("pixel_height") + init_label(_("pixel_height")) + init_description(_("Output pixel height")) + init_value(256) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_pixel_aspect_ratio(init_owner(*this) + init_name("pixel_aspect_ratio") + init_label(_("pixel_aspect_ratio")) + init_description(_("Output pixel aspect ratio")) + init_value(1.0) + init_constraint(constraint::minimum(std::numeric_limits<double>::epsilon())) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_resolution.changed_signal().connect(sigc::mem_fun(*this, &shadow_map::on_resolution_changed));
	}

	void on_resolution_changed(k3d::iunknown*)
	{
		const std::string new_resolution = m_resolution.pipeline_value();

		const k3d::resolutions_t& resolutions = k3d::resolutions();
		for(k3d::resolutions_t::const_iterator resolution = resolutions.begin(); resolution != resolutions.end(); ++resolution)
		{
			if(resolution->name != new_resolution)
				continue;

			m_pixel_width.set_value(resolution->width);
			m_pixel_height.set_value(resolution->height);
			return;
		}

		assert_not_reached();
	}

	void setup_renderman_texture(k3d::inetwork_render_frame& Frame, k3d::ri::istream& Stream, k3d::ri::ishader_collection& Shaders)
	{
		m_shadow_map_path = k3d::filesystem::path();

		if(!m_create_shadow_map.pipeline_value())
			return;

		k3d::icamera* const camera = m_camera.pipeline_value();
		return_if_fail(camera);

		Stream.RiFrameBegin(0);

		m_shadow_map_path = Frame.add_file("shadowmap.z");

		Stream.RiDisplayV(m_shadow_map_path.native_filesystem_string(), "shadow", k3d::ri::RI_Z());

		if(m_view_shadow_map.pipeline_value())
			Stream.RiDisplayV("+" + name(), "zframebuffer", k3d::ri::RI_Z());

		Stream.RiFormat(m_pixel_width.pipeline_value(), m_pixel_height.pipeline_value(), 1);
		Stream.RiPixelSamples(1, 1);
		Stream.RiPixelFilter(k3d::ri::RI_BOX(), 1, 1);

		k3d::ri::parameter_list hider_parameters;
		hider_parameters.push_back(k3d::ri::parameter("depthfilter", k3d::ri::UNIFORM, 1, k3d::ri::string("midpoint")));
		Stream.RiHiderV("hidden", hider_parameters);

		// Setup up viewing transformations
		Stream.RiComment("Setup shadowmap viewing transformations");

		if(k3d::iperspective* const perspective = dynamic_cast<k3d::iperspective*>(&camera->projection()))
		{
			const double left = boost::any_cast<double>(k3d::property::pipeline_value(perspective->left()));
			const double right = boost::any_cast<double>(k3d::property::pipeline_value(perspective->right()));
			const double top = boost::any_cast<double>(k3d::property::pipeline_value(perspective->top()));
			const double bottom = boost::any_cast<double>(k3d::property::pipeline_value(perspective->bottom()));
			const double near = boost::any_cast<double>(k3d::property::pipeline_value(perspective->near()));
			const double far = boost::any_cast<double>(k3d::property::pipeline_value(perspective->far()));
			return_if_fail(near > 0);

			Stream.RiProjectionV("perspective");
			Stream.RiScreenWindow(left / near, right / near, bottom / near, top / near);
			Stream.RiClipping(near, far);
		}
		else if(k3d::iorthographic* const orthographic = dynamic_cast<k3d::iorthographic*>(&camera->projection()))
		{
			const double left = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->left()));
			const double right = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->right()));
			const double top = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->top()));
			const double bottom = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->bottom()));
			const double near = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->near()));
			const double far = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->far()));
			return_if_fail(near > 0);

			const k3d::matrix4 transform_matrix = boost::any_cast<k3d::matrix4>(k3d::property::pipeline_value(camera->transformation().transform_source_output()));
			const k3d::point3 world_position = transform_matrix * k3d::point3(0, 0, 0);
			const k3d::point3 world_target = boost::any_cast<k3d::point3>(camera->world_target().property_internal_value());

			const double distance = k3d::distance(world_position, world_target);
			const double height = top - bottom;
			return_if_fail(height);

			const double aspect = (right - left) / height;
			const double tan_fov = height * 0.5 / near;
			const double size = distance * tan_fov;

			Stream.RiProjectionV("orthographic");
			Stream.RiScreenWindow(-size * aspect, size * aspect, -size, size);
			Stream.RiClipping(near, far);
		}

		// Setup the camera viewing transform ...
		const k3d::matrix4 transform_matrix = boost::any_cast<k3d::matrix4>(k3d::property::pipeline_value(camera->transformation().transform_source_output()));
		Stream.RiTransform(k3d::ri::convert(k3d::inverse(transform_matrix)));

		Stream.RiWorldBegin();

		// Render objects ...
		k3d::ri::render_state state(Frame, Stream, Shaders, camera->projection(), k3d::ri::SHADOW_MAP, k3d::ri::sample_times_t(1, 0.0), 0, transform_matrix);
		for(k3d::nodes_t::const_iterator node = document().nodes().collection().begin(); node != document().nodes().collection().end(); ++node)
		{
			if(k3d::ri::irenderable* const renderable = dynamic_cast<k3d::ri::irenderable*>(*node))
				renderable->renderman_render(state);
		}

		Stream.RiWorldEnd();
		Stream.RiFrameEnd();
	}

	const k3d::filesystem::path renderman_texture_path(const k3d::ri::render_state& State)
	{
		return m_shadow_map_path;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<shadow_map,
				k3d::interface_list<k3d::ri::itexture> > factory(
				k3d::uuid(0x9bce6e2b, 0x083c4353, 0x829052e9, 0xf98c7023),
				"RenderManShadowMap",
				_("A shadow_map source within the 3D workspace"),
				"RenderMan",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::icamera*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_camera;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_create_shadow_map;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_view_shadow_map;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_resolution;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_height;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_aspect_ratio;

	k3d::filesystem::path m_shadow_map_path;
};

/////////////////////////////////////////////////////////////////////////////
// shadow_map_factory

k3d::iplugin_factory& shadow_map_factory()
{
	return shadow_map::get_factory();
}

} // namespace libk3drenderman

