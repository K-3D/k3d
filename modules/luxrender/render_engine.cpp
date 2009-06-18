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

/*
#include "light.h"
#include "material.h"
*/

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/disk.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_range.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/inetwork_render_farm.h>
#include <k3dsdk/inetwork_render_frame.h>
#include <k3dsdk/inetwork_render_job.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irender_camera_animation.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/material.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/network_render_farm.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/resolutions.h>
#include <k3dsdk/sphere.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/triangulator.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

#include <iomanip>
#include <iterator>
#include <map>
#include <set>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// render_engine

class render_engine :
	public k3d::node,
	public k3d::inode_collection_sink,
	public k3d::irender_camera_preview,
	public k3d::irender_camera_frame,
	public k3d::irender_camera_animation
{
	typedef k3d::node base;
// initialise properties
public:
	render_engine(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_visible_nodes(init_owner(*this) + init_name("visible_nodes") + init_label(_("Visible Nodes")) + init_description(_("A list of nodes that will be visible in the rendered output.")) + init_value(std::vector<k3d::inode*>())),
		m_enabled_lights(init_owner(*this) + init_name("enabled_lights") + init_label(_("Enabled Lights")) + init_description(_("A list of light sources that will contribute to the rendered output.")) + init_value(std::vector<k3d::inode*>())),
		m_resolution(init_owner(*this) + init_name("resolution") + init_label(_("Resolution")) + init_description(_("Choose a predefined image resolution")) + init_enumeration(k3d::resolution_values()) + init_value(k3d::string_t(""))),
		m_pixel_width(init_owner(*this) + init_name("pixel_width") + init_label(_("pixel_width")) + init_description(_("The horizontal size in pixels of the rendered output image.")) + init_value(320) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_pixel_height(init_owner(*this) + init_name("pixel_height") + init_label(_("pixel_height")) + init_description(_("The vertical size in pixels of the rendered output image.")) + init_value(240) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1)))
	{
		m_resolution.changed_signal().connect(sigc::mem_fun(*this, &render_engine::on_resolution_changed));
	}

	const k3d::inode_collection_sink::properties_t node_collection_properties()
	{
		k3d::inode_collection_sink::properties_t results;
		results.push_back(&m_visible_nodes);
		results.push_back(&m_enabled_lights);

		return results;
	}

	void on_resolution_changed(k3d::iunknown*)
	{
		const k3d::string_t new_resolution = m_resolution.pipeline_value();

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

	k3d::bool_t render_camera_preview(k3d::icamera& Camera)
	{
		// Start a new render job ...
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-luxrender-preview");

		// Add a single render frame to the job ...
		k3d::inetwork_render_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path output_image = frame.add_file("output");

		// Render it ...
		return_val_if_fail(render(Camera, frame, output_image, true), false);

		// View the output when it's done ...
		frame.add_view_command(output_image + ".png");

		// Start the job running ...
		k3d::get_network_render_farm().start_job(job);

		return true;
	}

	k3d::bool_t render_camera_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const k3d::bool_t ViewImage)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImage.empty(), false);

		// Start a new render job ...
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-luxrender-render-frame");

		// Add a single render frame to the job ...
		k3d::inetwork_render_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path output_image = frame.add_file("output");

		// Render it ...
		return_val_if_fail(render(Camera, frame, output_image, false), false);

		// Copy the output image to its requested destination ...
		frame.add_copy_command(output_image + ".png", OutputImage);

		// View the output image when it's done ...
		if(ViewImage)
			frame.add_view_command(OutputImage);

		// Start the job running ...
		k3d::get_network_render_farm().start_job(job);

		return true;
	}

	k3d::bool_t render_camera_animation(k3d::icamera& Camera, k3d::iproperty& Time, const k3d::frames& Frames, const k3d::bool_t ViewCompletedImages)
	{
		// Start a new render job ...
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-luxrender-render-animation");

		// For each frame to be rendered ...
		k3d::uint_t frame_index = 0;
		for(k3d::frames::const_iterator frame = Frames.begin(); frame != Frames.end(); ++frame, ++frame_index)
		{
			// Set the frame time ...
			k3d::property::set_internal_value(Time, frame->begin_time);

			// Redraw everything ...
			k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::SYNCHRONOUS);

			// Add a render frame to the job ...
			std::stringstream buffer;
			buffer << "frame-" << frame_index;
			k3d::inetwork_render_frame& render_frame = job.create_frame(buffer.str());

			// Create an output image path ...
			const k3d::filesystem::path output_image = render_frame.add_file("output");

			// Render it (hidden rendering) ...
			return_val_if_fail(render(Camera, render_frame, output_image, false), false);

			// Copy the output image to its requested destination ...
			render_frame.add_copy_command(output_image + ".png", frame->destination);

			// View the output image when it's done ...
			if(ViewCompletedImages)
				render_frame.add_view_command(frame->destination);
		}

		// Start the job running ...
		k3d::get_network_render_farm().start_job(job);

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<render_engine,
			k3d::interface_list<k3d::irender_camera_animation,
			k3d::interface_list<k3d::irender_camera_frame,
			k3d::interface_list<k3d::irender_camera_preview> > > > factory(
				k3d::uuid(0xe28cbcb2, 0x1940a2f6, 0x5a8228ba, 0xdf1cd742),
				"LuxRenderEngine",
				_("LuxRender Render Engine"),
				"LuxRender RenderEngine",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
  /// Converts from K-3D's right-handed coordinate system
  static const k3d::point3 convert(const k3d::point3& Value)
  {
    return k3d::point3(-Value[0], Value[1], Value[2]);
  }

  /// Converts from K-3D's right-handed coordinate system
  static const k3d::vector3 convert(const k3d::vector3& Value)
  {
    return k3d::vector3(-Value[0], Value[1], Value[2]);
  }

  /// Converts from K-3D's right-handed coordinate system
  /** \todo Handle scaling and rotation */
  static const k3d::matrix4 convert(const k3d::matrix4& Value)
  {
    k3d::matrix4 result = k3d::transpose(Value);
    result[3][0] = -result[3][0];
    return result;
  }

  void render_disk(k3d::inode& MeshInstance, k3d::disk::const_primitive& Disk, std::ostream& Stream)
  {
    for(k3d::uint_t i = 0; i != Disk.matrices.size(); ++i)
    {
      Stream << k3d::standard_indent << "AttributeBegin\n" << k3d::push_indent;
      Stream << k3d::standard_indent << "Transform [" << convert(k3d::node_to_world_matrix(MeshInstance) * Disk.matrices[i]) << "]\n" << k3d::push_indent;
      Stream << k3d::standard_indent << "Shape \"disk\"";
      Stream << " \"float height\" [" << Disk.heights[i] << "]";
      Stream << " \"float radius\" [" << Disk.radii[i] << "]";
      Stream << " \"float phimax\" [" << k3d::degrees(Disk.sweep_angles[i]) << "]";
      Stream << "\n";
      Stream << k3d::pop_indent << k3d::standard_indent << "AttributeEnd\n";
    }
  }

  void render_sphere(k3d::inode& MeshInstance, k3d::sphere::const_primitive& Sphere, std::ostream& Stream)
  {
    for(k3d::uint_t i = 0; i != Sphere.matrices.size(); ++i)
    {
      Stream << k3d::standard_indent << "AttributeBegin\n" << k3d::push_indent;
      Stream << k3d::standard_indent << "Transform [" << convert(k3d::node_to_world_matrix(MeshInstance) * Sphere.matrices[i]) << "]\n" << k3d::push_indent;
      Stream << k3d::standard_indent << "Shape \"sphere\"";
      Stream << " \"float radius\" [" << Sphere.radii[i] << "]";
      Stream << "\n";
      Stream << k3d::pop_indent << k3d::standard_indent << "AttributeEnd\n";
    }
  }

	void render_mesh_instance(k3d::inode& MeshInstance, std::ostream& Stream)
	{
    const k3d::mesh* const mesh = k3d::property::pipeline_value<k3d::mesh*>(MeshInstance, "output_mesh");
    if(!mesh)
      return;

		for(k3d::mesh::primitives_t::const_iterator primitive = mesh->primitives.begin(); primitive != mesh->primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::disk::const_primitive> disk(k3d::disk::validate(**primitive));
			if(disk)
      {
        render_disk(MeshInstance, *disk, Stream);
				continue;
      }

			boost::scoped_ptr<k3d::sphere::const_primitive> sphere(k3d::sphere::validate(**primitive));
			if(sphere)
      {
        render_sphere(MeshInstance, *sphere, Stream);
				continue;
      }
		}
	}

	k3d::bool_t render(k3d::icamera& Camera, k3d::inetwork_render_frame& Frame, const k3d::filesystem::path& OutputImagePath, const k3d::bool_t VisibleRender)
	{
		try
		{
			// Start our luxrender XML file ...
			const k3d::filesystem::path scene_path = Frame.add_file("world.lxs");
			k3d::filesystem::ofstream stream(scene_path);
			return_val_if_fail(stream.good(), false);

			k3d::inetwork_render_frame::environment environment;

			k3d::inetwork_render_frame::arguments arguments;
			arguments.push_back(k3d::inetwork_render_frame::argument(scene_path.native_filesystem_string()));

			Frame.add_exec_command("luxconsole", environment, arguments);

			// Global setup ...
			stream << k3d::standard_indent << "# LuxRender scene generated by K-3D Version " K3D_VERSION ", http://www.k-3d.org\n\n";

			// Setup the camera ...
			k3d::iperspective* const projection = dynamic_cast<k3d::iperspective*>(&Camera.projection());
			if(!projection)
				throw std::runtime_error("A perspective projection is required.");

			const k3d::matrix4 camera_matrix = k3d::property::pipeline_value<k3d::matrix4>(Camera.transformation().transform_source_output());
			const k3d::point3 camera_from = k3d::position(camera_matrix);
      const k3d::point3 camera_to = camera_from + (camera_matrix * k3d::vector3(0, 0, 1));
			const k3d::vector3 camera_up = camera_matrix * k3d::vector3(0, 1, 0);
      stream << k3d::standard_indent << "LookAt " << convert(camera_from) << " " << convert(camera_to) << " " << convert(camera_up) << "\n";
/*
			const k3d::double_t camera_near = k3d::property::pipeline_value<k3d::double_t>(projection->near());
			const k3d::double_t camera_width = std::abs(k3d::property::pipeline_value<k3d::double_t>(projection->left()) - k3d::property::pipeline_value<k3d::double_t>(projection->right()));
*/

			stream << k3d::standard_indent << "Camera";
      stream << " \"perspective\"";
/*
			stream << k3d::standard_indent << "<pos>" << -camera_pos[0] << " " << camera_pos[1] << " " << camera_pos[2] << "</pos>\n";
			stream << k3d::standard_indent << "<up>" << -camera_up[0] << " " << camera_up[1] << " " << camera_up[2] << "</up>\n";
			stream << k3d::standard_indent << "<forwards>" << -camera_forwards[0] << " " << camera_forwards[1] << " " << camera_forwards[2] << "</forwards>\n";
			stream << k3d::standard_indent << "<aperture_radius>" << "0.001" << "</aperture_radius>\n";
			stream << k3d::standard_indent << "<focus_distance>" << "3.0" << "</focus_distance>\n";
			stream << k3d::standard_indent << "<aspect_ratio>" << static_cast<k3d::double_t>(pixel_width) / static_cast<k3d::double_t>(pixel_height) << "</aspect_ratio>\n";
			stream << k3d::standard_indent << "<sensor_width>" << camera_width << "</sensor_width>\n";
			stream << k3d::standard_indent << "<lens_sensor_dist>" << camera_near << "</lens_sensor_dist>\n";
			stream << k3d::standard_indent << "<white_balance>" << "D65" << "</white_balance>\n";
			stream << k3d::standard_indent << "<autofocus/>\n";
*/
      stream << "\n";

			// Setup output options ...
			const k3d::int32_t pixel_width = m_pixel_width.pipeline_value();
			const k3d::int32_t pixel_height = m_pixel_height.pipeline_value();
			stream << k3d::standard_indent << "Film";
      stream << " \"fleximage\"";
      stream << " \"string filename\" [\"" << OutputImagePath.native_filesystem_string() << "\"]";
			stream << " \"integer xresolution\" [" << pixel_width << "]";
			stream << " \"integer yresolution\" [" << pixel_height << "]";
			stream << " \"integer haltspp\" [" << 200 << "]";
      stream << "\n";

      // Scene setup ...
      stream << k3d::standard_indent << "WorldBegin\n" << k3d::push_indent;

/*
			// Setup lights ...
			const k3d::inode_collection_property::nodes_t enabled_lights = m_enabled_lights.pipeline_value();
			for(k3d::inode_collection_property::nodes_t::const_iterator node = enabled_lights.begin(); node != enabled_lights.end(); ++node)
			{
				if(luxrender::light* const light = dynamic_cast<luxrender::light*>(*node))
				{
					light->setup(stream);
				}
			}
*/

      stream <<
        "AttributeBegin\n"
        "LightSource \"distant\" \"point from\" [0 0 0] \"point to\" [0 0 -1] \"color L\" [0.5 0.5 0.5]\n"
        "AttributeEnd\n";

/*
			// Setup materials, assigning unique names as-we-go ...
			std::map<luxrender::material*, k3d::string_t> material_names;
			const std::vector<luxrender::material*> materials = k3d::node::lookup<luxrender::material>(document());
			for(k3d::uint_t i = 0; i != materials.size(); ++i)
			{
				const k3d::string_t material_name = "material_" + k3d::string_cast(material_names.size());
				material_names.insert(std::make_pair(materials[i], material_name));
				materials[i]->setup(material_name, stream);
			}
*/

			// Render geometry ...
			const k3d::inode_collection_property::nodes_t visible_nodes = m_visible_nodes.pipeline_value();
			for(k3d::inode_collection_property::nodes_t::const_iterator node = visible_nodes.begin(); node != visible_nodes.end(); ++node)
			{
				if((*node)->factory().factory_id() != k3d::classes::MeshInstance())
					continue;

				render_mesh_instance(**node, stream);
			}

			stream << k3d::pop_indent << k3d::standard_indent << "WorldEnd\n";
		}
		catch(std::exception& e)
		{
			k3d::log() << error << "exception: " << e.what() << std::endl;
			return false;
		}
		catch(...)
		{
			k3d::log() << error << "unknown exception" << std::endl;
			return false;
		}

		return true;
	}

	/// Helper class that limits the list of visible nodes to those that we can render
	template<typename value_t, class name_policy_t>
	class luxrender_visible_nodes_property :
		public k3d::data::writable_property<value_t, name_policy_t>,
		public k3d::inode_collection_property
	{
		typedef k3d::data::writable_property<value_t, name_policy_t> base;

	public:
		k3d::bool_t property_allow(k3d::inode& Node)
		{
			return Node.factory().factory_id() == k3d::classes::MeshInstance();
		}

	protected:
		template<typename init_t>
		luxrender_visible_nodes_property(const init_t& Init) :
			base(Init)
		{
		}
	};

	/// Helper class that limits the list of enabled lights
	template<typename value_t, class name_policy_t>
	class luxrender_enabled_lights_property :
		public k3d::data::writable_property<value_t, name_policy_t>,
		public k3d::inode_collection_property
	{
		typedef k3d::data::writable_property<value_t, name_policy_t> base;

	public:
		k3d::bool_t property_allow(k3d::inode& Node)
		{
      assert_not_implemented();
      return false;
/*
			return dynamic_cast<luxrender::light*>(&Node) ? true : false;
*/
		}

	protected:
		template<typename init_t>
		luxrender_enabled_lights_property(const init_t& Init) :
			base(Init)
		{
		}
	};

// define properties
	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, luxrender_visible_nodes_property, node_collection_serialization) m_visible_nodes;
	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, luxrender_enabled_lights_property, node_collection_serialization) m_enabled_lights;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_resolution;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_height;
};

k3d::iplugin_factory& render_engine_factory()
{
	return render_engine::get_factory();
}

} // namespace luxrender

} // namespace module

