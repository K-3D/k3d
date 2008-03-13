// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_range.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/icrop_window.h>
#include <k3dsdk/iimager_shader_ri.h>
#include <k3dsdk/ilight_ri.h>
#include <k3dsdk/inetwork_render_farm.h>
#include <k3dsdk/inetwork_render_frame.h>
#include <k3dsdk/inetwork_render_job.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irender_camera_animation.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/istream_ri.h>
#include <k3dsdk/irenderable_ri.h>
#include <k3dsdk/itexture_ri.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/ivolume_shader_ri.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/network_render_farm.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/property_group_collection.h>
#include <k3dsdk/stream_ri.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/resolutions.h>
#include <k3dsdk/shader_cache.h>
#include <k3dsdk/shader_collection_ri.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/user_interface.h>
#include <k3dsdk/utility_gl.h>

#include <iomanip>

#include <boost/scoped_ptr.hpp>

#ifdef	WIN32
#ifdef	near
#undef	near
#endif	//near
#ifdef	far
#undef	far
#endif	//far
#endif	//WIN32

namespace libk3drenderman
{

namespace detail
{

class setup_texture
{
public:
	explicit setup_texture(k3d::inetwork_render_frame& Frame, k3d::ri::istream& Stream, k3d::ri::ishader_collection& Shaders) :
		frame(Frame),
		stream(Stream),
		shaders(Shaders)
	{
	}

	void operator()(k3d::inode* const Node)
	{
		if(k3d::ri::itexture* const texture = dynamic_cast<k3d::ri::itexture*>(Node))
			texture->setup_renderman_texture(frame, stream, shaders);
	}

private:
	k3d::inetwork_render_frame& frame;
	k3d::ri::istream& stream;
	k3d::ri::ishader_collection& shaders;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// render_engine

class render_engine :
	public k3d::persistent<k3d::node>,
	public k3d::inode_collection_sink,
	public k3d::irender_camera_preview,
	public k3d::irender_camera_frame,
	public k3d::irender_camera_animation,
	public k3d::property_group_collection
{
	typedef k3d::persistent<k3d::node> base;

public:
	render_engine(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_visible_nodes(init_owner(*this) + init_name("visible_nodes") + init_label(_("Visible Nodes")) + init_description(_("A list of nodes that will be visible in the rendered output.")) + init_value(std::vector<k3d::inode*>())),
		m_enabled_lights(init_owner(*this) + init_name("enabled_lights") + init_label(_("Enabled Lights")) + init_description(_("A list of light sources that will contribute to the rendered output.")) + init_value(std::vector<k3d::inode*>())),
		m_render_engine(init_owner(*this) + init_name("render_engine") + init_label(_("Render Engine")) + init_description(_("Choose an installed RenderMan engine to be used for rendering.")) + init_value<k3d::ri::irender_engine*>(0)),
		m_resolution(init_owner(*this) + init_name("resolution") + init_label(_("Resolution")) + init_description(_("Choose a predefined image resolution")) + init_enumeration(k3d::resolution_values()) + init_value(std::string(""))),
		m_hider(init_owner(*this) + init_name("hider") + init_label(_("Hider")) + init_description(_("Hider Algorithm")) + init_value(std::string())),
		m_pixel_width(init_owner(*this) + init_name("pixel_width") + init_label(_("Pixel Width")) + init_description(_("Output pixel width")) + init_value(320) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_pixel_height(init_owner(*this) + init_name("pixel_height") + init_label(_("Pixel Height")) + init_description(_("Output pixel height")) + init_value(240) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_pixel_aspect_ratio(init_owner(*this) + init_name("pixel_aspect_ratio") + init_label(_("Pixel Aspect Ratio")) + init_description(_("Output pixel aspect ratio")) + init_value(1.0) + init_constraint(constraint::minimum(std::numeric_limits<double>::epsilon())) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_default_atmosphere_shader(init_owner(*this) + init_name("default_atmosphere_shader") + init_label(_("Default Atmosphere Shader")) + init_description(_("Default atmosphere shader")) + init_value<k3d::ri::ivolume_shader*>(0)),
		m_default_interior_shader(init_owner(*this) + init_name("default_interior_shader") + init_label(_("Default Interior Shader")) + init_description(_("Default interior shader")) + init_value<k3d::ri::ivolume_shader*>(0)),
		m_default_exterior_shader(init_owner(*this) + init_name("default_exterior_shader") + init_label(_("Default Exterior Shader")) + init_description(_("Default exterior shader")) + init_value<k3d::ri::ivolume_shader*>(0)),
		m_imager_shader(init_owner(*this) + init_name("imager_shader") + init_label(_("Imager Shader")) + init_description(_("Imager shader")) + init_value<k3d::ri::iimager_shader*>(0)),
		m_bucket_width(init_owner(*this) + init_name("bucket_width") + init_label(_("Bucket Width")) + init_description(_("Bucket Width")) + init_value(16) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_bucket_height(init_owner(*this) + init_name("bucket_height") + init_label(_("Bucket Height")) + init_description(_("Bucket Height")) + init_value(16) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_grid_size(init_owner(*this) + init_name("grid_size") + init_label(_("Grid Size")) + init_description(_("Grid Size")) + init_value(256) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_eye_splits(init_owner(*this) + init_name("eye_splits") + init_label(_("Eye Splits")) + init_description(_("Eye Splits")) + init_value(10) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_texture_memory(init_owner(*this) + init_name("texture_memory") + init_label(_("Texture Memory")) + init_description(_("Texture Memory")) + init_value(1024) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_render_alpha(init_owner(*this) + init_name("render_alpha") + init_label(_("Render Alpha")) + init_description(_("Render Alpha")) + init_value(false)),
		m_pixel_xsamples(init_owner(*this) + init_name("pixel_xsamples") + init_label(_("Pixel X Samples")) + init_description(_("The smaller the faster, the higher the better (anti-aliasing)")) + init_value(3.0) + init_constraint(constraint::minimum(1.0)) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_pixel_ysamples(init_owner(*this) + init_name("pixel_ysamples") + init_label(_("Pixel Y Samples")) + init_description(_("The smaller the faster, the higher the better (anti-aliasing)")) + init_value(3.0) + init_constraint(constraint::minimum(1.0)) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_pixel_filter(init_owner(*this) + init_name("pixel_filter") + init_label(_("Pixel Filter")) + init_description(_("Anti-aliasing type")) + init_value(k3d::ri::RI_GAUSSIAN()) + init_values(pixel_filter_values())),
		m_pixel_filter_width(init_owner(*this) + init_name("pixel_filter_width") + init_label(_("Pixel Filter Width")) + init_description(_("Filter width in X")) + init_value(2.0) + init_constraint(constraint::minimum(std::numeric_limits<double>::epsilon())) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_pixel_filter_height(init_owner(*this) + init_name("pixel_filter_height") + init_label(_("Pixel Filter Height")) + init_description(_("Filter width in Y")) + init_value(2.0) + init_constraint(constraint::minimum(std::numeric_limits<double>::epsilon())) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_exposure(init_owner(*this) + init_name("exposure") + init_label(_("Exposure")) + init_description(_("Exposure")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_gamma(init_owner(*this) + init_name("gamma") + init_label(_("Gamma")) + init_description(_("Gamma")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_dof(init_owner(*this) + init_name("dof") + init_label(_("DOF")) + init_description(_("Depth of Field")) + init_value(false)),
		m_fstop(init_owner(*this) + init_name("fstop") + init_label(_("f-Stop")) + init_description(_("f-Stop")) + init_value(0.3) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_focal_length(init_owner(*this) + init_name("focal_length") + init_label(_("Focal Length")) + init_description(_("Focal Length")) + init_value(1.6) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_focus_plane(init_owner(*this) + init_name("focus_plane") + init_label(_("Focus Plane")) + init_description(_("Focus Plane")) + init_value(30.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_shading_rate(init_owner(*this) + init_name("shading_rate") + init_label(_("Shading Rate")) + init_description(_("Shading Rate")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_shading_interpolation(init_owner(*this) + init_name("shading_interpolation") + init_label(_("Shading Interpolation")) + init_description(_("Shading Interpolation")) + init_value(k3d::ri::RI_CONSTANT()) + init_values(shading_interpolation_values())),
		m_two_sided(init_owner(*this) + init_name("two_sided") + init_label(_("Two-Sided")) + init_description(_("Two Sided")) + init_value(true)),
		m_motion_blur(init_owner(*this) + init_name("motion_blur") + init_label(_("Motion Blur")) + init_description(_("Motion Blur")) + init_value(false)),
		m_render_motion_blur(init_owner(*this) + init_name("render_motion_blur") + init_label(_("Render Motion Blur")) + init_description(_("Render Motion Blur")) + init_value(false))
	{
		k3d::iproperty_group_collection::group output_group("Output");
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_resolution));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_hider));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_width));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_height));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_aspect_ratio));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_default_atmosphere_shader));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_default_interior_shader));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_default_exterior_shader));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_imager_shader));
		output_group.properties.push_back(&static_cast<k3d::iproperty&>(m_render_alpha));

		k3d::iproperty_group_collection::group sampling_group("Sampling");
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_bucket_width));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_bucket_height));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_grid_size));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_eye_splits));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_xsamples));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_ysamples));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_filter));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_filter_width));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_pixel_filter_height));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_shading_rate));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_shading_interpolation));
		sampling_group.properties.push_back(&static_cast<k3d::iproperty&>(m_render_motion_blur));

		k3d::iproperty_group_collection::group lens_group("Lens");
		lens_group.properties.push_back(&static_cast<k3d::iproperty&>(m_exposure));
		lens_group.properties.push_back(&static_cast<k3d::iproperty&>(m_gamma));
		lens_group.properties.push_back(&static_cast<k3d::iproperty&>(m_dof));
		lens_group.properties.push_back(&static_cast<k3d::iproperty&>(m_fstop));
		lens_group.properties.push_back(&static_cast<k3d::iproperty&>(m_focal_length));
		lens_group.properties.push_back(&static_cast<k3d::iproperty&>(m_focus_plane));

		register_property_group(output_group);
		register_property_group(sampling_group);
		register_property_group(lens_group);

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

	bool render_camera_preview(k3d::icamera& Camera)
	{
		k3d::ri::irender_engine* const render_engine = m_render_engine.pipeline_value();
		return_val_if_fail(render_engine, false);
		return_val_if_fail(render_engine->installed(), false);

		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-renderman-preview");
		k3d::inetwork_render_frame& frame = job.create_frame("frame");
		const k3d::filesystem::path output_image = frame.add_file("output_image");

		k3d::ri::shader_collection shaders;
		return_val_if_fail(render(Camera, frame, *render_engine, output_image, true, shaders), false);
		synchronize_shaders(shaders, *render_engine);

		k3d::get_network_render_farm().start_job(job);

		return true;
	}

	bool render_camera_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const bool ViewImage)
	{
		k3d::ri::irender_engine* const render_engine = m_render_engine.pipeline_value();
		return_val_if_fail(render_engine, false);
		return_val_if_fail(render_engine->installed(), false);

		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-renderman-render-frame");
		k3d::inetwork_render_frame& frame = job.create_frame("frame");
		const k3d::filesystem::path output_image = frame.add_file("output_image");

		k3d::ri::shader_collection shaders;
		return_val_if_fail(render(Camera, frame, *render_engine, output_image, false, shaders), false);
		synchronize_shaders(shaders, *render_engine);

		frame.add_copy_command(output_image, OutputImage);
		if(ViewImage)
			frame.add_view_command(OutputImage);

		k3d::get_network_render_farm().start_job(job);

		return true;
	}

	bool render_camera_animation(k3d::icamera& Camera, const k3d::file_range& Files, const bool ViewCompletedImages)
	{
		k3d::ri::irender_engine* const render_engine = m_render_engine.pipeline_value();
		return_val_if_fail(render_engine, false);
		return_val_if_fail(render_engine->installed(), false);

		// Ensure that the document has animation capabilities, first ...
		k3d::iproperty* const start_time_property = k3d::get_start_time(document());
		k3d::iproperty* const end_time_property = k3d::get_end_time(document());
		k3d::iproperty* const frame_rate_property = k3d::get_frame_rate(document());
		k3d::iwritable_property* const time_property = dynamic_cast<k3d::iwritable_property*>(k3d::get_time(document()));
		return_val_if_fail(start_time_property && end_time_property && frame_rate_property && time_property, false);

		// Test the output images filepath to make sure it can hold all the frames we're going to generate ...
		const double start_time = boost::any_cast<double>(k3d::property::pipeline_value(*start_time_property));
		const double end_time = boost::any_cast<double>(k3d::property::pipeline_value(*end_time_property));
		const double frame_rate = boost::any_cast<double>(k3d::property::pipeline_value(*frame_rate_property));

		const size_t start_frame = static_cast<size_t>(k3d::round(frame_rate * start_time));
		const size_t end_frame = static_cast<size_t>(k3d::round(frame_rate * end_time));

		return_val_if_fail(Files.max_file_count() > end_frame, false);

		k3d::ri::shader_collection shaders;
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-renderman-render-animation");

		// For each frame to be rendered ...
		for(size_t view_frame = start_frame; view_frame < end_frame; ++view_frame)
		{
			// Set the frame time ...
			time_property->property_set_value(view_frame / frame_rate);

			// Redraw everything ...
			k3d::gl::redraw_all(document(), k3d::gl::irender_viewport::SYNCHRONOUS);

			// Add a render frame to the job ...
			std::stringstream buffer;
			buffer << "frame-" << std::setw(Files.digits) << std::setfill('0') << view_frame;
			k3d::inetwork_render_frame& frame = job.create_frame(buffer.str());

			// Create an output image path ...
			const k3d::filesystem::path output_image = frame.add_file("output_image");

			// Render it (hidden rendering) ...
			return_val_if_fail(render(Camera, frame, *render_engine, output_image, false, shaders), false);

			// Copy the output image to its requested destination ...
			const k3d::filesystem::path destination = Files.file(view_frame);
			frame.add_copy_command(output_image, destination);

			// View the output image when it's done ...
			if(ViewCompletedImages)
				frame.add_view_command(destination);
		}

		// Ensure shaders are up-to-date ...
		synchronize_shaders(shaders, *render_engine);

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
					k3d::classes::RenderManEngine(),
					"RenderManEngine",
					_("RenderMan Render Engine"),
					"RenderMan RenderEngines",
					k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	bool render(k3d::icamera& Camera, k3d::inetwork_render_frame& Frame, k3d::ri::irender_engine& RenderEngine, const k3d::filesystem::path& OutputImagePath, const bool VisibleRender, k3d::ri::shader_collection& Shaders)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImagePath.empty(), false);

		// Setup global time for this frame ...
		k3d::iproperty* const frame_rate_property = k3d::get_frame_rate(document());
		k3d::iproperty* const time_property = k3d::get_time(document());
		k3d::iwritable_property* const writable_time_property = dynamic_cast<k3d::iwritable_property*>(time_property);
		return_val_if_fail(frame_rate_property && time_property && writable_time_property, false);

		const double frame_delta = 1.0 / boost::any_cast<double>(k3d::property::pipeline_value(*frame_rate_property));
		const double frame_time = boost::any_cast<double>(k3d::property::pipeline_value(*time_property));

		// Start our RIB file ...
		const std::string ribfilename("world.rib");
		const k3d::filesystem::path ribfilepath = Frame.add_file(ribfilename);

		// Open the RIB file stream ...
		k3d::filesystem::ofstream ribfile(ribfilepath);
		return_val_if_fail(ribfile.good(), false);

		// Setup the frame for RI rendering with the user's preferred engine ...
		return_val_if_fail(RenderEngine.render(Frame, ribfilepath), false);

		// Create the Ri render engine object ...
		k3d::ri::stream stream(ribfile);

		// Administrivia ...
		stream.RiNewline();
		stream.RiComment("Created with K-3D Version " K3D_VERSION ", http://www.k-3d.org");

		// Set path options ...
		stream.RiNewline();
		stream.RiComment("Setup file search paths");

		k3d::ri::parameter_list searchpath_options;
		searchpath_options.push_back(
			k3d::ri::parameter(
				"shader",
				k3d::ri::UNIFORM,
				1,
				k3d::ri::string(k3d::shader_cache_path().native_filesystem_string() + ":&")));
		stream.RiOptionV("searchpath", searchpath_options);

		// Perform frame-start operations for textures
		std::for_each(document().nodes().collection().begin(), document().nodes().collection().end(), detail::setup_texture(Frame, stream, Shaders));

		// Start the (final output) frame ...
		stream.RiNewline();
		stream.RiFrameBegin(1);

		const std::string hider = m_hider.pipeline_value();
		if(!hider.empty())
		    stream.RiHiderV(hider);

		// Set limits options
		stream.RiNewline();
		stream.RiComment("Setup options");

		k3d::typed_array<k3d::ri::integer>* const bucketsize = new k3d::typed_array<k3d::ri::integer>();
		bucketsize->push_back(m_bucket_width.pipeline_value());
		bucketsize->push_back(m_bucket_height.pipeline_value());

		k3d::ri::parameter_list limits;
		limits.push_back(k3d::ri::parameter("gridsize", k3d::ri::UNIFORM, 1, static_cast<k3d::ri::integer>(m_grid_size.pipeline_value())));
		limits.push_back(k3d::ri::parameter("bucketsize", k3d::ri::UNIFORM, 2, bucketsize));
		limits.push_back(k3d::ri::parameter("eyesplits", k3d::ri::UNIFORM, 1, static_cast<k3d::ri::integer>(m_eye_splits.pipeline_value())));
		limits.push_back(k3d::ri::parameter("texturememory", k3d::ri::UNIFORM, 1, static_cast<k3d::ri::integer>(m_texture_memory.pipeline_value())));
		stream.RiOptionV("limits", limits);

		// Setup custom options ...
		stream.RiNewline();
		stream.RiComment("Setup user options");
		k3d::ri::set_options(*this, stream);

		// Set the display type ...
		stream.RiNewline();
		stream.RiComment("Setup Display");

		k3d::filesystem::path outputimage(OutputImagePath);

		if(VisibleRender)
			stream.RiDisplayV(outputimage.leaf().raw(), k3d::ri::RI_FRAMEBUFFER(), k3d::ri::RI_RGB());
		else
		{
			if(m_render_alpha.pipeline_value())
				stream.RiDisplayV(outputimage.leaf().raw(), k3d::ri::RI_FILE(), k3d::ri::RI_RGBA());
			else
				stream.RiDisplayV(outputimage.leaf().raw(), k3d::ri::RI_FILE(), k3d::ri::RI_RGB());
		}

		const k3d::ri::unsigned_integer pixel_width = m_pixel_width.pipeline_value();
		const k3d::ri::unsigned_integer pixel_height = m_pixel_height.pipeline_value();
		const k3d::ri::real pixel_aspect_ratio = m_pixel_aspect_ratio.pipeline_value();

		stream.RiFormat(pixel_width, pixel_height, pixel_aspect_ratio);

		// Set pixel sampling rates ...
		stream.RiPixelSamples(m_pixel_xsamples.pipeline_value(), m_pixel_ysamples.pipeline_value());
		stream.RiPixelFilter(m_pixel_filter.pipeline_value(), m_pixel_filter_width.pipeline_value(), m_pixel_filter_height.pipeline_value());

		// Set gain & gamma ...
		stream.RiExposure(m_exposure.pipeline_value(), m_gamma.pipeline_value());

		// Set depth-of-field options ...
		if(m_dof.pipeline_value())
			stream.RiDepthOfField(
				m_fstop.pipeline_value(),
				m_focal_length.pipeline_value(),
				m_focus_plane.pipeline_value());

		// Set global shading rate ...
		stream.RiShadingRate(m_shading_rate.pipeline_value());

		// Set global shading interpolation ...
		stream.RiShadingInterpolation(m_shading_interpolation.pipeline_value());

		// We use LH orientation, the way Our Lord Who Art In Heaven wants us to ...
		stream.RiOrientation(k3d::ri::RI_LH());

		// Double-sided surfaces ...
		stream.RiSides(m_two_sided.pipeline_value() ? 2 : 1);

		// Crop window ...
		const double crop_left = boost::any_cast<double>(k3d::property::pipeline_value(Camera.crop_window().crop_left()));
		const double crop_right = boost::any_cast<double>(k3d::property::pipeline_value(Camera.crop_window().crop_right()));
		const double crop_top = boost::any_cast<double>(k3d::property::pipeline_value(Camera.crop_window().crop_top()));
		const double crop_bottom = boost::any_cast<double>(k3d::property::pipeline_value(Camera.crop_window().crop_bottom()));

		if(crop_left >= crop_right || crop_top >= crop_bottom)
			k3d::user_interface().message("Current crop window settings will produce an empty image");

		stream.RiCropWindow(crop_left, crop_right, crop_top, crop_bottom);

		// Setup our motion-blur sampling loop ...
		k3d::ri::sample_times_t samples(1, 0.0);
		if(m_render_motion_blur.pipeline_value())
			samples.push_back(1.0);

		const bool motion_blur_camera = m_motion_blur.pipeline_value();
		std::vector<k3d::point3> motion_blur_camera_position_samples;
		std::vector<k3d::angle_axis> motion_blur_camera_orientation_samples;

		for(boost::uint32_t sample_index = 0; sample_index < samples.size(); ++sample_index)
		{
			// Calculate the current time and animate the document ...
			const double sample_delta = (samples.size() > 1) ? frame_delta / static_cast<double>(samples.size() - 1) : frame_delta;
			const double sample_time = frame_time + (sample_index * sample_delta);

			writable_time_property->property_set_value(sample_time);

			const k3d::matrix4 transform_matrix = boost::any_cast<k3d::matrix4>(k3d::property::pipeline_value(Camera.transformation().transform_source_output()));
			const k3d::angle_axis orientation(k3d::euler_angles(transform_matrix, k3d::euler_angles::ZXYstatic));
			const k3d::point3 position(k3d::position(transform_matrix));

			motion_blur_camera_position_samples.push_back(position);
			motion_blur_camera_orientation_samples.push_back(orientation);

			// Setup render state ...
			k3d::ri::render_state state(Frame, stream, Shaders, Camera.projection(), k3d::ri::FINAL_FRAME, samples, sample_index, transform_matrix);

			if(k3d::ri::last_sample(state))
			{
				stream.RiNewline();
				stream.RiComment("Setup viewing transformation");

				if(k3d::iperspective* const perspective = dynamic_cast<k3d::iperspective*>(&Camera.projection()))
				{
					const double left = boost::any_cast<double>(k3d::property::pipeline_value(perspective->left()));
					const double right = boost::any_cast<double>(k3d::property::pipeline_value(perspective->right()));
					const double top = boost::any_cast<double>(k3d::property::pipeline_value(perspective->top()));
					const double bottom = boost::any_cast<double>(k3d::property::pipeline_value(perspective->bottom()));
					const double near = boost::any_cast<double>(k3d::property::pipeline_value(perspective->near()));
					const double far = boost::any_cast<double>(k3d::property::pipeline_value(perspective->far()));
					return_val_if_fail(near > 0, false);

					stream.RiProjectionV("perspective");
					stream.RiScreenWindow(left / near, right / near, bottom / near, top / near);
					stream.RiClipping(near, far);
				}
				else if(k3d::iorthographic* const orthographic = dynamic_cast<k3d::iorthographic*>(&Camera.projection()))
				{
					const double left = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->left()));
					const double right = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->right()));
					const double top = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->top()));
					const double bottom = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->bottom()));
					const double near = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->near()));
					const double far = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->far()));
					return_val_if_fail(near > 0, false);

					const k3d::matrix4 transform_matrix = boost::any_cast<k3d::matrix4>(k3d::property::pipeline_value(Camera.transformation().transform_source_output()));
					const k3d::point3 world_position = transform_matrix * k3d::point3(0, 0, 0);
					const k3d::point3 world_target = boost::any_cast<k3d::point3>(Camera.world_target().property_internal_value());

					const double distance = k3d::distance(world_position, world_target);
					const double height = top - bottom;
					return_val_if_fail(height, false);

					const double aspect = (right - left) / height;
					const double tan_fov = height * 0.5 / near;
					const double size = distance * tan_fov;

					stream.RiProjectionV("orthographic");
					stream.RiScreenWindow(-size * aspect, size * aspect, -size, size);
					stream.RiClipping(near, far);
				}

				if(k3d::ri::motion_blur(state) && motion_blur_camera)
				{
					stream.RiMotionBeginV(state.sample_times);
					for(unsigned int i = 0; i < motion_blur_camera_orientation_samples.size(); ++i)
					{
						k3d::quaternion q(motion_blur_camera_orientation_samples[i]);
						k3d::euler_angles a(q, k3d::euler_angles::ZXYstatic);
						stream.RiRotate(-k3d::degrees(a[0]), 0.0f, 0.0f, 1.0f);
					}
					stream.RiMotionEnd();

					stream.RiMotionBeginV(state.sample_times);
					for(unsigned int i = 0; i < motion_blur_camera_orientation_samples.size(); ++i)
					{
						k3d::quaternion q(motion_blur_camera_orientation_samples[i]);
						k3d::euler_angles a(q, k3d::euler_angles::ZXYstatic);
						stream.RiRotate(-k3d::degrees(a[1]), 1.0f, 0.0f, 0.0f);
					}
					stream.RiMotionEnd();

					stream.RiMotionBeginV(state.sample_times);
					for(unsigned int i = 0; i < motion_blur_camera_orientation_samples.size(); ++i)
					{
						k3d::quaternion q(motion_blur_camera_orientation_samples[i]);
						k3d::euler_angles a(q, k3d::euler_angles::ZXYstatic);
						stream.RiRotate(-k3d::degrees(a[2]), 0.0f, 1.0f, 0.0f);
					}
					stream.RiMotionEnd();

					stream.RiMotionBeginV(state.sample_times);
					for(unsigned int i = 0; i < motion_blur_camera_position_samples.size(); ++i)
						stream.RiTranslate(-motion_blur_camera_position_samples[i][0], -motion_blur_camera_position_samples[i][1], -motion_blur_camera_position_samples[i][2]);
					stream.RiMotionEnd();
				}
				else
				{
					k3d::quaternion q(motion_blur_camera_orientation_samples[0]);
					k3d::euler_angles a(q, k3d::euler_angles::ZXYstatic);
					stream.RiRotate(-k3d::degrees(a[0]), 0.0f, 0.0f, 1.0f);

					q = k3d::quaternion(motion_blur_camera_orientation_samples[0]);
					a = k3d::euler_angles(q, k3d::euler_angles::ZXYstatic);
					stream.RiRotate(-k3d::degrees(a[1]), 1.0f, 0.0f, 0.0f);

					q = k3d::quaternion(motion_blur_camera_orientation_samples[0]);
					a = k3d::euler_angles(q, k3d::euler_angles::ZXYstatic);
					stream.RiRotate(-k3d::degrees(a[2]), 0.0f, 1.0f, 0.0f);

					stream.RiTranslate(-motion_blur_camera_position_samples[0][0], -motion_blur_camera_position_samples[0][1], -motion_blur_camera_position_samples[0][2]);
				}

				// Default shaders ...
				if(m_default_atmosphere_shader.pipeline_value())
					m_default_atmosphere_shader.pipeline_value()->setup_renderman_atmosphere_shader(state);
				if(m_default_interior_shader.pipeline_value())
					m_default_interior_shader.pipeline_value()->setup_renderman_interior_shader(state);
				if(m_default_exterior_shader.pipeline_value())
					m_default_exterior_shader.pipeline_value()->setup_renderman_exterior_shader(state);
				if(m_imager_shader.pipeline_value())
					m_imager_shader.pipeline_value()->setup_renderman_imager_shader(state);

				// Begin the world ...
				stream.RiNewline();
				stream.RiWorldBegin();
			}

			// Setup lights ...
			const k3d::inode_collection_property::nodes_t enabled_lights = m_enabled_lights.pipeline_value();
			for(k3d::inode_collection_property::nodes_t::const_iterator node = enabled_lights.begin(); node != enabled_lights.end(); ++node)
			{
				if(k3d::ri::ilight* const light = dynamic_cast<k3d::ri::ilight*>(*node))
					light->setup_renderman_light(state);
			}

			// Render nodes ...
			const k3d::inode_collection_property::nodes_t visible_nodes = m_visible_nodes.pipeline_value();
			for(k3d::inode_collection_property::nodes_t::const_iterator node = visible_nodes.begin(); node != visible_nodes.end(); ++node)
			{
				if(k3d::ri::irenderable* const renderable = dynamic_cast<k3d::ri::irenderable*>(*node))
					renderable->renderman_render(state);
			}

			// Give nodes a chance to cleanup ...
			for(k3d::inode_collection_property::nodes_t::const_iterator node = visible_nodes.begin(); node != visible_nodes.end(); ++node)
			{
				if(k3d::ri::irenderable* const renderable = dynamic_cast<k3d::ri::irenderable*>(*node))
					renderable->renderman_render_complete(state);
			}

			if(k3d::ri::last_sample(state))
			{
				// Finish the world ...
				stream.RiWorldEnd();
			}
		}

		// Finish the frame ...
		stream.RiFrameEnd();

		// Reset the time back to the beginning of the frame ...
		writable_time_property->property_set_value(frame_time);

		return true;
	}

	void synchronize_shaders(const k3d::ri::shader_collection& Shaders, k3d::ri::irender_engine& RenderEngine)
	{
		// Compile each shader in the given collection ...
		const k3d::ri::shader_collection::shaders_t& shaders = Shaders.shaders();
		for(k3d::ri::shader_collection::shaders_t::const_iterator shader = shaders.begin(); shader != shaders.end(); ++shader)
		{
			if(!RenderEngine.compile_shader(*shader))
				k3d::log() << error << k3d::string_cast(boost::format(_("Error compiling shader %1%")) % shader->native_utf8_string().raw()) << std::endl;
		}
	}

	/// Helper class that limits the list of visible nodes to those that implement k3d::ri::irenderable
	template<typename value_t, class name_policy_t>
	class renderman_visible_nodes_property :
		public k3d::data::writable_property<value_t, name_policy_t>,
		public k3d::inode_collection_property
	{
		typedef k3d::data::writable_property<value_t, name_policy_t> base;

	public:
		bool property_allow(k3d::inode& Node)
		{
			return dynamic_cast<k3d::ri::irenderable*>(&Node) ? true : false;
		}

	protected:
		template<typename init_t>
		renderman_visible_nodes_property(const init_t& Init) :
			base(Init)
		{
		}
	};

	/// Helper class that limits the list of enabled lights to those the implement k3d::ri::ilight
	template<typename value_t, class name_policy_t>
	class renderman_enabled_lights_property :
		public k3d::data::writable_property<value_t, name_policy_t>,
		public k3d::inode_collection_property
	{
		typedef k3d::data::writable_property<value_t, name_policy_t> base;

	public:
		bool property_allow(k3d::inode& Node)
		{
			return dynamic_cast<k3d::ri::ilight*>(&Node) ? true : false;
		}

	protected:
		template<typename init_t>
		renderman_enabled_lights_property(const init_t& Init) :
			base(Init)
		{
		}
	};

	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_visible_nodes_property, node_collection_serialization) m_visible_nodes;
	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_enabled_lights_property, node_collection_serialization) m_enabled_lights;
	k3d_data(k3d::ri::irender_engine*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_render_engine;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_resolution;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_hider;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_height;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_aspect_ratio;
	k3d_data(k3d::ri::ivolume_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_default_atmosphere_shader;
	k3d_data(k3d::ri::ivolume_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_default_interior_shader;
	k3d_data(k3d::ri::ivolume_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_default_exterior_shader;
	k3d_data(k3d::ri::iimager_shader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_imager_shader;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_bucket_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_bucket_height;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_grid_size;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_eye_splits;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_texture_memory;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_render_alpha;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_xsamples;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_ysamples;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_pixel_filter;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_filter_width;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_filter_height;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_exposure;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_gamma;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_dof;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_fstop;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_focal_length;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_focus_plane;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_shading_rate;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_shading_interpolation;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_two_sided;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_motion_blur;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_render_motion_blur;

	const k3d::ilist_property<std::string>::values_t& pixel_filter_values()
	{
		static k3d::ilist_property<std::string>::values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ri::RI_GAUSSIAN());
			values.push_back(k3d::ri::RI_BOX());
			values.push_back(k3d::ri::RI_TRIANGLE());
			values.push_back(k3d::ri::RI_CATMULL_ROM());
			values.push_back(k3d::ri::RI_SINC());
		}
		return values;
	}

	const k3d::ilist_property<std::string>::values_t& shading_interpolation_values()
	{
		static k3d::ilist_property<std::string>::values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ri::RI_CONSTANT());
			values.push_back(k3d::ri::RI_SMOOTH());
		}
		return values;
	}
};

k3d::iplugin_factory& render_engine_factory()
{
	return render_engine::get_factory();
}

} // namespace libk3drenderman

