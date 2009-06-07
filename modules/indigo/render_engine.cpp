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

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_range.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
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
#include <k3dsdk/properties.h>
#include <k3dsdk/resolutions.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

#include <iomanip>
#include <iterator>
#include <map>

namespace module
{

namespace indigo
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
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-indigo-preview");

		// Add a single render frame to the job ...
		k3d::inetwork_render_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path output_image = frame.add_file("salida.tga");

		// Render it ...
		return_val_if_fail(render(Camera, frame, output_image, true), false);

		// View the output when it's done ...
		frame.add_view_command(output_image);

		// Start the job running ...
		k3d::get_network_render_farm().start_job(job);

		return true;
	}

	k3d::bool_t render_camera_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const k3d::bool_t ViewImage)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImage.empty(), false);

		// Start a new render job ...
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-indigo-render-frame");

		// Add a single render frame to the job ...
		k3d::inetwork_render_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path output_image = frame.add_file("salida.tga");

		// Render it ...
		return_val_if_fail(render(Camera, frame, output_image, false), false);

		// Copy the output image to its requested destination ...
		frame.add_copy_command(output_image, OutputImage);

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
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-indigo-render-animation");

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
			const k3d::filesystem::path output_image = render_frame.add_file("salida.tga");

			// Render it (hidden rendering) ...
			return_val_if_fail(render(Camera, render_frame, output_image, false), false);

			// Copy the output image to its requested destination ...
			render_frame.add_copy_command(output_image, frame->destination);

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
				k3d::uuid(0xd3415ff0, 0x084b272d, 0x76eba2ad, 0x813d5f1d),
				"IndigoEngine",
				_("Indigo Render Engine"),
				"Indigo RenderEngine",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d::bool_t render(k3d::icamera& Camera, k3d::inetwork_render_frame& Frame, const k3d::filesystem::path& OutputImagePath, const k3d::bool_t VisibleRender)
	{
		try
		{
			// Sanity checks ...
			return_val_if_fail(!OutputImagePath.empty(), false);

			// Start our indigo XML file ...
			const k3d::filesystem::path filepath = Frame.add_file("world.igs");
			k3d::filesystem::ofstream stream(filepath);
			return_val_if_fail(stream.good(), false);

			k3d::inetwork_render_frame::environment environment;

			k3d::inetwork_render_frame::arguments arguments;
			arguments.push_back(k3d::inetwork_render_frame::argument(filepath.native_filesystem_string()));

			Frame.add_exec_command("indigo_console", environment, arguments);

			// Setup an indigo scene description ...
      stream << k3d::xml::declaration();
			stream << "<!-- Indigo scene generated by K-3D Version " K3D_VERSION ", http://www.k-3d.org -->\n";
			stream << "<scene>\n";

      // Set renderer options ...
      stream << "<renderer_settings>\n";
      stream << "<width>" << m_pixel_width.pipeline_value() << "</width>\n";
      stream << "<height>" << m_pixel_height.pipeline_value() << "</height>\n";
      stream << "<halt_time>" << "5" << "</halt_time>\n";
      stream << "</renderer_settings>\n";

      // Setup the camera ...
      stream << "<camera>\n";
      stream << "<pos>" << "0 -2 1" << "</pos>\n";
      stream << "<up>" << "0 0 1" << "</up>\n";
      stream << "<forwards>" << "0 1 0" << "</forwards>\n";
      stream << "<aperture_radius>" << "0.001" << "</aperture_radius>\n";
      stream << "<focus_distance>" << "3.0" << "</focus_distance>\n";
      stream << "<aspect_ratio>" << "1.33" << "</aspect_ratio>\n";
      stream << "<sensor_width>" << "0.036" << "</sensor_width>\n";
      stream << "<lens_sensor_dist>" << "0.02" << "</lens_sensor_dist>\n";
      stream << "<white_balance>" << "E" << "</white_balance>\n";
      stream << "<autofocus/>\n";
      stream << "</camera>\n";

      // Setup tonemapping ...
      stream << "<tonemapping>\n";
      stream << "<linear>\n";
      stream << "<scale>" << "1.0" << "</scale>\n";
      stream << "</linear>\n";
      stream << "</tonemapping>\n";

      // Setup a skylight
      /** \todo Make this a separate light node */
      stream << "<skylight>\n";
      stream << "<sundir>0 0.6 1</sundir>\n";
      stream << "<turbidity>2</turbidity>\n"; 
      stream << "</skylight>\n";

      // Setup a material
      /** \todo Make this a separate material node */
      stream << "<material>"
      "<name>test_material</name>"
      "<diffuse>"
      "<base_emission>"
      "<constant>"
      "<rgb>"
      "<rgb>"
      "2000000000 2000000000 2000000000"
      "</rgb>"
      "<gamma>1</gamma>"
      "</rgb>"
      "</constant>"
      "</base_emission>"
      "</diffuse>"
      "</material>\n";

      // Setup a sphere
      /** \todo Replace this with code to handle geometric primitives */
      stream << "<sphere>\n";
      stream << "<center>0 0 0</center>\n";
      stream << "<radius>1</radius>\n";
      stream << "<material_name>test_material</material_name>\n";
      stream << "</sphere>\n";

			// Finish the scene ...
			stream << "</scene>\n";
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
	class indigo_visible_nodes_property :
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
		indigo_visible_nodes_property(const init_t& Init) :
			base(Init)
		{
		}
	};

	/// Helper class that limits the list of enabled lights
	template<typename value_t, class name_policy_t>
	class indigo_enabled_lights_property :
		public k3d::data::writable_property<value_t, name_policy_t>,
		public k3d::inode_collection_property
	{
		typedef k3d::data::writable_property<value_t, name_policy_t> base;

	public:
		k3d::bool_t property_allow(k3d::inode& Node)
		{
			return true;
		}

	protected:
		template<typename init_t>
		indigo_enabled_lights_property(const init_t& Init) :
			base(Init)
		{
		}
	};

	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, indigo_visible_nodes_property, node_collection_serialization) m_visible_nodes;
	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, indigo_enabled_lights_property, node_collection_serialization) m_enabled_lights;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_resolution;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_height;
};

k3d::iplugin_factory& render_engine_factory()
{
	return render_engine::get_factory();
}

} // namespace indigo

} // namespace module

