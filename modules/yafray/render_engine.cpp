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
	\author Romain Behar <romainbehar@yahoo.com>
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
#include <k3dsdk/ilight_yafray.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_yafray.h>
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
#include <k3dsdk/irenderable_gl.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/material.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/network_render_farm.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/resolutions.h>
#include <k3dsdk/subdivision_surface/k3d_sds_binding.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/triangulator.h>
#include <k3dsdk/utility_gl.h>

#include <boost/scoped_ptr.hpp>

#include <iomanip>
#include <iterator>
#include <map>

namespace module
{

namespace yafray
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
		m_resolution(init_owner(*this) + init_name("resolution") + init_label(_("Resolution")) + init_description(_("Choose a predefined image resolution")) + init_enumeration(k3d::resolution_values()) + init_value(std::string(""))),
		m_pixel_width(init_owner(*this) + init_name("pixel_width") + init_label(_("pixel_width")) + init_description(_("The horizontal size in pixels of the rendered output image.")) + init_value(320) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_pixel_height(init_owner(*this) + init_name("pixel_height") + init_label(_("pixel_height")) + init_description(_("The vertical size in pixels of the rendered output image.")) + init_value(240) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_AA_passes(init_owner(*this) + init_name("AA_passes") + init_label(_("AA_passes")) + init_description(_("AA passes")) + init_value(3) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(0))),
		m_AA_minsamples(init_owner(*this) + init_name("AA_minsamples") + init_label(_("AA_minsamples")) + init_description(_("AA min samples")) + init_value(2) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(0))),
		m_AA_pixelwidth(init_owner(*this) + init_name("AA_pixelwidth") + init_label(_("AA_pixelwidth")) + init_description(_("AA pixelwidth")) + init_value(1.5)),
		m_AA_threshold(init_owner(*this) + init_name("AA_threshold") + init_label(_("AA_threshold")) + init_description(_("AA threshold")) + init_value(0.05)),
		m_raydepth(init_owner(*this) + init_name("raydepth") + init_label(_("raydepth")) + init_description(_("raydepth")) + init_value(3) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(0))),
		m_bias(init_owner(*this) + init_name("bias") + init_label(_("bias")) + init_description(_("bias")) + init_value(0.1)),
		m_save_alpha(init_owner(*this) + init_name("save_alpha") + init_label(_("save_alpha")) + init_description(_("Save alpha")) + init_value(false)),
		m_exposure(init_owner(*this) + init_name("exposure") + init_label(_("exposure")) + init_description(_("exposure")) + init_value(0.0)),
		m_gamma(init_owner(*this) + init_name("gamma") + init_label(_("gamma")) + init_description(_("gamma")) + init_value(1)),
		m_fog_density(init_owner(*this) + init_name("fog_density") + init_label(_("fog_density")) + init_description(_("fog_density")) + init_value(0.0)),
		m_fog_color(init_owner(*this) + init_name("fog_color") + init_label(_("fog_color")) + init_description(_("Fog color")) + init_value(k3d::color(1, 1, 1))),
		m_preview_sds(init_owner(*this) + init_name("preview_sds") + init_label(_("Preview SDS")) + init_description(_("Show SDS Surfaces")) + init_value(true))
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
		// Start a new render job ...
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-yafray-preview");

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

	bool render_camera_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const bool ViewImage)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImage.empty(), false);

		// Start a new render job ...
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-yafray-render-frame");

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

	bool render_camera_animation(k3d::icamera& Camera, k3d::iproperty& Time, const k3d::frames& Frames, const bool ViewCompletedImages)
	{
		// Start a new render job ...
		k3d::inetwork_render_job& job = k3d::get_network_render_farm().create_job("k3d-yafray-render-animation");

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
				k3d::uuid(0xef38bf93, 0x66654f9f, 0x992ca91b, 0x62bae139),
				"YafrayEngine",
				_("Yafray Render Engine"),
				"Yafray RenderEngine",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef std::map<k3d::yafray::imaterial*, k3d::string_t> shader_names_t;

	void render_sphere(const shader_names_t& ShaderNames, const k3d::string_t& Name, const k3d::inode& Sphere, std::ostream& Stream)
	{
		// This stopped working somewhere between yafray 0.0.7 - 0.0.9 !?
		assert_not_implemented();

/*
		const k3d::point3 sphere_center = k3d::node_to_world_matrix(Sphere) * k3d::point3(0, 0, 0);
		const double sphere_radius = k3d::property::pipeline_value<double>(Sphere, "radius");
	
		Stream << "<!-- K-3D plugin: " << Sphere.factory().name() << " name: " << Sphere.name() << " -->\n";
		Stream << "<object name=\"" << Sphere.name() << "\" shader_name=\"" << shader_name(ShaderNames, Sphere) << "\">\n";
		Stream << "	<attributes>\n";
		Stream << "	</attributes>\n";
		Stream << "	<sphere radius=\"" << sphere_radius << "\">\n";
		Stream << "		<center x=\"" << std::fixed << -sphere_center[0] << "\" y=\"" << std::fixed << sphere_center[1] << "\" z=\"" << std::fixed << sphere_center[2] << "\"/>\n";
		Stream << "	</sphere>\n";
		Stream << "</object>\n";
*/
	}

	/// Helper class used to triangulate faces for Yafray
	class create_triangles :
		public k3d::triangulator
	{
		typedef k3d::triangulator base;

	public:
		create_triangles(const k3d::mesh::materials_t& OriginalMaterials, k3d::mesh::points_t& Points, k3d::mesh::indices_t& APoints, k3d::mesh::indices_t& BPoints, k3d::mesh::indices_t& CPoints, k3d::mesh::materials_t& Materials) :
			m_original_materials(OriginalMaterials),
			m_points(Points),
			m_a_points(APoints),
			m_b_points(BPoints),
			m_c_points(CPoints),
			m_materials(Materials)
		{
		}

	private:
		void start_face(const k3d::uint_t Face)
		{
			m_current_face = Face;
		}

		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], double Weights[4], k3d::uint_t& NewVertex)
		{
			NewVertex = m_points.size();
			m_points.push_back(Coordinates);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			m_a_points.push_back(Vertices[0]);
			m_b_points.push_back(Vertices[1]);
			m_c_points.push_back(Vertices[2]);
			m_materials.push_back(m_original_materials[m_current_face]);
		}

		const k3d::mesh::materials_t& m_original_materials;
		k3d::mesh::points_t& m_points;
		k3d::mesh::indices_t& m_a_points;
		k3d::mesh::indices_t& m_b_points;
		k3d::mesh::indices_t& m_c_points;
		k3d::mesh::materials_t& m_materials;

		k3d::uint_t m_current_face;
	};

	void render_mesh_instance(const shader_names_t& ShaderNames, const k3d::string_t& Name, k3d::inode& MeshInstance, std::ostream& Stream)
	{
		const k3d::mesh* const mesh = k3d::property::pipeline_value<k3d::mesh*>(MeshInstance, "output_mesh");
		if(!mesh)
			return;

		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(*mesh));
		if(!polyhedron)
			return;

		// Triangulate the mesh faces ...
		k3d::mesh::points_t points(*(mesh->points));
		const k3d::matrix4 transformation = k3d::node_to_world_matrix(MeshInstance); 
		for (k3d::uint_t point = 0; point != points.size(); ++point)
			points[point] = points[point] * transformation;
		k3d::mesh::indices_t a_points;
		k3d::mesh::indices_t b_points;
		k3d::mesh::indices_t c_points;
		k3d::mesh::materials_t materials;
		create_triangles(polyhedron->face_materials, points, a_points, b_points, c_points, materials).process(*mesh, *polyhedron);

		// Sort faces by material ...
		typedef std::vector<k3d::uint_t> faces_t;
		typedef std::map<k3d::imaterial*, faces_t> sorted_faces_t;
		sorted_faces_t sorted_faces;
		for(k3d::uint_t i = 0; i != a_points.size(); ++i)
			sorted_faces[materials[i]].push_back(i);

		Stream << "<!-- K-3D plugin: " << MeshInstance.factory().name() << " name: " << MeshInstance.name() << " -->\n";

		// Write out each group of faces that shares the same material ...
		k3d::uint_t index = 0;
		for(sorted_faces_t::const_iterator i = sorted_faces.begin(); i != sorted_faces.end(); ++i, ++index)
		{
			const k3d::string_t object_name = Name + "_" + k3d::string_cast(index);

			k3d::imaterial* const material = i->first;
			const faces_t& faces = i->second;

			k3d::string_t shader_name = "shader_0";
			bool shadow = true;
			bool emit_rad = true;
			bool recv_rad = true;
			bool caustics = true;
			double caus_IOR = 1.0;
			k3d::color caus_rcolor(0, 0, 0);
			k3d::color caus_tcolor(0, 0, 0);
			double autosmooth_value = 89.9;
			bool has_orco = false;

			if(k3d::yafray::imaterial* const yafray_material = k3d::material::lookup<k3d::yafray::imaterial>(material))
			{
				shader_name = ShaderNames.count(yafray_material) ? ShaderNames.find(yafray_material)->second : "shader_0";

				shadow = k3d::property::pipeline_value<bool>(*yafray_material, "shadow");
				emit_rad = k3d::property::pipeline_value<bool>(*yafray_material, "emit_rad");
				recv_rad = k3d::property::pipeline_value<bool>(*yafray_material, "recv_rad");
				caustics = k3d::property::pipeline_value<bool>(*yafray_material, "caustics");
				caus_IOR = k3d::property::pipeline_value<double>(*yafray_material, "caus_IOR");
				caus_rcolor = k3d::property::pipeline_value<k3d::color>(*yafray_material, "caus_rcolor");
				caus_tcolor = k3d::property::pipeline_value<k3d::color>(*yafray_material, "caus_tcolor");
				autosmooth_value = k3d::property::pipeline_value<double>(*yafray_material, "mesh_autosmooth_value");
				has_orco = k3d::property::pipeline_value<bool>(*yafray_material, "has_orco");
			}


			Stream << "<object name=\"" << object_name << "\"";
			Stream << " shader_name=\"" << shader_name << "\"";
			Stream << " shadow=\"" << (shadow ? "on" : "off") << "\"";
			Stream << " emit_rad=\"" << (emit_rad ? "on" : "off") << "\"";
			Stream << " recv_rad=\"" << (recv_rad ? "on" : "off") << "\"";
			Stream << " caustics=\"" << (caustics ? "on" : "off") << "\"";
			Stream << " caus_IOR=\"" << caus_IOR << "\"";
			Stream << ">\n";
			Stream << "	<attributes>\n";
			Stream << "		<caus_rcolor r=\"" << caus_rcolor.red << "\" g=\"" << caus_rcolor.green << "\" b=\"" << caus_rcolor.blue << "\"/>\n";
			Stream << "		<caus_tcolor r=\"" << caus_tcolor.red << "\" g=\"" << caus_tcolor.green << "\" b=\"" << caus_tcolor.blue << "\"/>\n";
			Stream << "	</attributes>\n";
			Stream << "	<mesh autosmooth=\"" << autosmooth_value << "\">\n";
			Stream << "		<points>\n";
			// Note: we write out every point here, to keep things simple
			for(k3d::uint_t i = 0; i != points.size(); ++i)
				Stream << "			<p x=\"" << points[i][0] << "\" y=\"" << points[i][1] << "\" z=\"" << points[i][2] << "\"/>\n";
			Stream << "		</points>\n";
			Stream << "		<faces>\n";
			for(faces_t::const_iterator face = faces.begin(); face != faces.end(); ++face)
				Stream << "			<f a=\"" << a_points[*face] << "\" b=\"" << b_points[*face] << "\" c=\"" << c_points[*face] << "\"/>\n";
			Stream << "		</faces>\n";
			Stream << "	</mesh>\n";
			Stream << "</object>\n";
		}
	}

	bool render(k3d::icamera& Camera, k3d::inetwork_render_frame& Frame, const k3d::filesystem::path& OutputImagePath, const bool VisibleRender)
	{
		try
		{
			// Sanity checks ...
			return_val_if_fail(!OutputImagePath.empty(), false);

			// Start our YafRay XML file ...
			const k3d::filesystem::path filepath = Frame.add_file("world.xml");

			// Open the RIB file stream ...
			k3d::filesystem::ofstream stream(filepath);
			return_val_if_fail(stream.good(), false);

			// Setup the frame for YafRay rendering ...
			k3d::inetwork_render_frame::environment environment;

			k3d::inetwork_render_frame::arguments arguments;
			arguments.push_back(k3d::inetwork_render_frame::argument(filepath.native_filesystem_string()));

			Frame.add_exec_command("yafray", environment, arguments);

			// Setup a YafRay scene description ...
			stream << "<!-- Yafray scene generated by K-3D Version " K3D_VERSION ", http://www.k-3d.org -->\n";
			stream << "<scene>\n";

			// Get the document contents ...
			const k3d::nodes_t nodes(document().nodes().collection());

			// Setup Yafray shaders, keeping-track of names as-we-go ...
			shader_names_t shader_names;
			for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
			{
				if(k3d::yafray::imaterial* const material = dynamic_cast<k3d::yafray::imaterial*>(*node))
				{
					const k3d::string_t shader_name = "shader_" + k3d::string_cast(shader_names.size());
					shader_names.insert(std::make_pair(material, shader_name));
					material->setup_material(shader_name, stream);
				}
			}

			// Render geometry, keeping-track of names as we go ...
			std::map<k3d::inode*, k3d::string_t> object_names;

			const k3d::inode_collection_property::nodes_t visible_nodes = m_visible_nodes.pipeline_value();
			for(k3d::inode_collection_property::nodes_t::const_iterator node = visible_nodes.begin(); node != visible_nodes.end(); ++node)
			{
				const k3d::string_t object_name = "object_" + k3d::string_cast(object_names.size());
				object_names.insert(std::make_pair(*node, object_name));

				if((**node).factory().factory_id() == k3d::classes::Sphere())
					render_sphere(shader_names, object_name, **node, stream);
				else if((*node)->factory().factory_id() == k3d::classes::MeshInstance())
					render_mesh_instance(shader_names, object_name, **node, stream);
			}

			// Setup lights, keeping-track of names as we go ...
			std::map<k3d::yafray::ilight*, k3d::string_t> light_names;

			const k3d::inode_collection_property::nodes_t enabled_lights = m_enabled_lights.pipeline_value();
			for(k3d::inode_collection_property::nodes_t::const_iterator node = enabled_lights.begin(); node != enabled_lights.end(); ++node)
			{
				if(k3d::yafray::ilight* const light = dynamic_cast<k3d::yafray::ilight*>(*node))
				{
					const k3d::string_t light_name = "light_" + k3d::string_cast(light_names.size());
					light_names.insert(std::make_pair(light, light_name));
					light->setup_light(light_name, stream);
				}
			}

			// Setup the camera ...
			k3d::inode* const camera_node = dynamic_cast<k3d::inode*>(&Camera);
			if(!camera_node)
				throw std::runtime_error("camera not a node");

			const k3d::matrix4 camera_matrix = k3d::property::pipeline_value<k3d::matrix4>(Camera.transformation().transform_source_output());
			const k3d::point3 camera_position = k3d::position(camera_matrix);
			const k3d::point3 camera_to_vector = camera_matrix * k3d::point3(0, 0, 1);
			const k3d::point3 camera_up_vector = camera_matrix * k3d::point3(0, 1, 0);

			stream << "<!-- K-3D plugin: " << camera_node->factory().name() << " name: " << camera_node->name() << " -->\n";
			stream << "<camera name=\"camera_0\" resx=\"" << m_pixel_width.pipeline_value() << "\" resy=\"" << m_pixel_height.pipeline_value() << "\" focal=\"0.7\">\n";
			stream << "	<from x=\"" << -camera_position[0] << "\" y=\"" << camera_position[1] << "\" z=\"" << camera_position[2] << "\"/>\n";
			stream << "	<to x=\"" << -camera_to_vector[0] << "\" y=\"" << camera_to_vector[1] << "\" z=\"" << camera_to_vector[2] << "\"/>\n";
			stream << "	<up x=\"" << -camera_up_vector[0] << "\" y=\"" << camera_up_vector[1] << "\" z=\"" << camera_up_vector[2] << "\"/>\n";
			stream << "</camera>\n";

			// Generate the output file ...
			const k3d::color fog_color = m_fog_color.pipeline_value();

			stream << "<!-- K-3D plugin: " << factory().name() << " name: " << name() << " -->\n";
			stream << "<render camera_name=\"camera_0\" AA_passes=\"" << m_AA_passes.pipeline_value() << "\"" << " AA_minsamples=\"" << m_AA_minsamples.pipeline_value() << "\" AA_pixelwidth=\"" << m_AA_pixelwidth.pipeline_value() << "\" AA_threshold=\"" << m_AA_threshold.pipeline_value() << "\" raydepth=\"" << m_raydepth.pipeline_value() << "\" bias=\"" << m_bias.pipeline_value() << "\">\n";
			stream << "	<outfile value=\"" << OutputImagePath.native_filesystem_string() << "\"/>\n";
			stream << "	<save_alpha value=\"" << (m_save_alpha.pipeline_value() ? "on" : "off") << "\"/>\n";
			stream << "	<exposure value=\"" << m_exposure.pipeline_value() << "\"/>\n";
			stream << "	<gamma value=\"" << m_gamma.pipeline_value() << "\"/>\n";
			stream << "	<fog_density value=\"" << m_fog_density.pipeline_value() << "\"/>\n";
			stream << "	<fog_color r=\"" << fog_color.red << "\" g=\"" << fog_color.green << "\" b=\"" << fog_color.blue << "\"/>\n";
			stream << "</render>\n";

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

/*
	/// Apply SDS if needed
	void sds_filter(const k3d::legacy::mesh& Input, const std::string& RenderType, k3d::legacy::mesh& Output, int Levels)
	{
		if (!m_preview_sds.pipeline_value() || !(Input.polyhedra.size() > 0 && (RenderType == "catmull-clark")))
		{
			k3d::legacy::deep_copy(Input, Output);
			return;
		}
		k3d::sds::k3d_mesh_sds_cache sds_cache;

		// Set levels -before- input
		sds_cache.set_levels(Levels);
		sds_cache.set_input(&Input);
		sds_cache.update();
		sds_cache.output(&Output);
	}
*/

	/// Helper class that limits the list of visible nodes to those that we can render
	template<typename value_t, class name_policy_t>
	class yafray_visible_nodes_property :
		public k3d::data::writable_property<value_t, name_policy_t>,
		public k3d::inode_collection_property
	{
		typedef k3d::data::writable_property<value_t, name_policy_t> base;

	public:
		bool property_allow(k3d::inode& Node)
		{
			return Node.factory().factory_id() == k3d::classes::MeshInstance();
		}

	protected:
		template<typename init_t>
		yafray_visible_nodes_property(const init_t& Init) :
			base(Init)
		{
		}
	};

	/// Helper class that limits the list of enabled lights to those the implement k3d::yafray::ilight
	template<typename value_t, class name_policy_t>
	class yafray_enabled_lights_property :
		public k3d::data::writable_property<value_t, name_policy_t>,
		public k3d::inode_collection_property
	{
		typedef k3d::data::writable_property<value_t, name_policy_t> base;

	public:
		bool property_allow(k3d::inode& Node)
		{
			return dynamic_cast<k3d::yafray::ilight*>(&Node) ? true : false;
		}

	protected:
		template<typename init_t>
		yafray_enabled_lights_property(const init_t& Init) :
			base(Init)
		{
		}
	};

	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, yafray_visible_nodes_property, node_collection_serialization) m_visible_nodes;
	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, yafray_enabled_lights_property, node_collection_serialization) m_enabled_lights;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_resolution;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_width;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_height;

	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_AA_passes;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_AA_minsamples;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_AA_pixelwidth;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_AA_threshold;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_raydepth;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bias;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_save_alpha;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_exposure;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gamma;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_fog_density;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_fog_color;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_preview_sds;
};

k3d::iplugin_factory& render_engine_factory()
{
	return render_engine::get_factory();
}

} // namespace yafray

} // namespace module

