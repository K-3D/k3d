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
		\author Tim Shead <tshead@k-3d.com>
		\author Romain Behar <romainbehar@yahoo.com>
*/

#include <subdivision_surface/k3d_sds_binding.h>

#include <k3dsdk/algebra.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_range.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/icamera_animation_render_engine.h>
#include <k3dsdk/icamera_preview_render_engine.h>
#include <k3dsdk/icamera_still_render_engine.h>
#include <k3dsdk/idrawable_gl.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_client.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irender_farm.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/irender_job.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/iyafray.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>
#include <k3dsdk/render_farm.h>
#include <k3dsdk/resolutions.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>

#include <iomanip>
#include <iterator>
#include <map>

namespace libk3dyafray
{

/////////////////////////////////////////////////////////////////////////////
// render_engine

class render_engine :
	public k3d::persistent<k3d::node>,
	public k3d::icamera_preview_render_engine,
	public k3d::icamera_still_render_engine,
	public k3d::icamera_animation_render_engine
{
	typedef k3d::persistent<k3d::node> base;

public:
	render_engine(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_resolution(init_owner(*this) + init_name("resolution") + init_label(_("Resolution")) + init_description(_("Choose a predefined image resolution")) + init_enumeration(k3d::resolution_values()) + init_value(std::string(""))),
		m_pixel_width(init_owner(*this) + init_name("pixel_width") + init_label(_("pixel_width")) + init_description(_("Output pixel width")) + init_value(320) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(1L))),
		m_pixel_height(init_owner(*this) + init_name("pixel_height") + init_label(_("pixel_height")) + init_description(_("Output pixel height")) + init_value(240) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(1L))),
		m_AA_passes(init_owner(*this) + init_name("AA_passes") + init_label(_("AA_passes")) + init_description(_("AA passes")) + init_value(3) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0L))),
		m_AA_minsamples(init_owner(*this) + init_name("AA_minsamples") + init_label(_("AA_minsamples")) + init_description(_("AA min samples")) + init_value(2) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0L))),
		m_AA_pixelwidth(init_owner(*this) + init_name("AA_pixelwidth") + init_label(_("AA_pixelwidth")) + init_description(_("AA pixelwidth")) + init_value(1.5)),
		m_AA_threshold(init_owner(*this) + init_name("AA_threshold") + init_label(_("AA_threshold")) + init_description(_("AA threshold")) + init_value(0.05)),
		m_raydepth(init_owner(*this) + init_name("raydepth") + init_label(_("raydepth")) + init_description(_("raydepth")) + init_value(3) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0L))),
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

	void on_resolution_changed(k3d::iunknown*)
	{
		const std::string new_resolution = m_resolution.value();

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
		k3d::irender_job& job = k3d::render_farm().create_job("k3d-preview");

		// Add a single render frame to the job ...
		k3d::irender_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path outputimagepath = frame.add_output_file("salida.tga");
		return_val_if_fail(!outputimagepath.empty(), false);

		// View the output image when it's done ...
		frame.add_view_operation(outputimagepath);

		// Render it (visible rendering) ...
		return_val_if_fail(render(Camera, frame, outputimagepath, true), false);

		// Start the job running ...
		k3d::render_farm().start_job(job);

		return true;
	}

	bool render_camera_frame(k3d::icamera& Camera, const k3d::filesystem::path& OutputImage, const bool ViewImage)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImage.empty(), false);

		// Start a new render job ...
		k3d::irender_job& job = k3d::render_farm().create_job("k3d-render-frame");

		// Add a single render frame to the job ...
		k3d::irender_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path outputimagepath = frame.add_output_file("salida.tga");
		return_val_if_fail(!outputimagepath.empty(), false);

		// Copy the output image to its requested destination ...
		frame.add_copy_operation(outputimagepath, OutputImage);

		// View the output image when it's done ...
		if(ViewImage)
			frame.add_view_operation(OutputImage);

		// Render it (hidden rendering) ...
		return_val_if_fail(render(Camera, frame, outputimagepath, false), false);

		// Start the job running ...
		k3d::render_farm().start_job(job);

		return true;
	}

	bool render_camera_animation(k3d::icamera& Camera, const k3d::file_range& Files, const bool ViewCompletedImages)
	{
		// Ensure that the document has animation capabilities, first ...
		k3d::iproperty* const start_time_property = k3d::get_start_time(document());
		k3d::iproperty* const end_time_property = k3d::get_end_time(document());
		k3d::iproperty* const frame_rate_property = k3d::get_frame_rate(document());
		k3d::iwritable_property* const time_property = dynamic_cast<k3d::iwritable_property*>(k3d::get_time(document()));
		return_val_if_fail(start_time_property && end_time_property && frame_rate_property && time_property, false);

		// Test the output images filepath to make sure it can hold all the frames we're going to generate ...
		const double start_time = boost::any_cast<double>(k3d::get_value(document().dag(), *start_time_property));
		const double end_time = boost::any_cast<double>(k3d::get_value(document().dag(), *end_time_property));
		const double frame_rate = boost::any_cast<double>(k3d::get_value(document().dag(), *frame_rate_property));

		const size_t start_frame = static_cast<size_t>(k3d::round(frame_rate * start_time));
		const size_t end_frame = static_cast<size_t>(k3d::round(frame_rate * end_time));

		return_val_if_fail(Files.max_file_count() > end_frame, false);

		// Start a new render job ...
		k3d::irender_job& job = k3d::render_farm().create_job("k3d-render-animation");

		// For each frame to be rendered ...
		for(size_t view_frame = start_frame; view_frame < end_frame; ++view_frame)
		{
			// Set the frame time ...
			time_property->property_set_value(view_frame / frame_rate);

			// Redraw everything ...
			k3d::gl::redraw_all(document(), k3d::gl::irender_engine::SYNCHRONOUS);

			// Add a render frame to the job ...
			std::stringstream buffer;
			buffer << "frame-" << std::setw(Files.digits) << std::setfill('0') << view_frame;
			k3d::irender_frame& frame = job.create_frame(buffer.str());

			// Create an output image path ...
			const k3d::filesystem::path outputimagepath = frame.add_output_file("salida.tga");
			return_val_if_fail(!outputimagepath.empty(), false);

			// Copy the output image to its requested destination ...
			const k3d::filesystem::path destination = Files.file(view_frame);
			frame.add_copy_operation(outputimagepath, destination);

			// View the output image when it's done ...
			if(ViewCompletedImages)
				frame.add_view_operation(destination);

			// Render it (hidden rendering) ...
			return_val_if_fail(render(Camera, frame, outputimagepath, false), false);
		}

		// Start the job running ...
		k3d::render_farm().start_job(job);

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<render_engine,
			k3d::interface_list<k3d::icamera_animation_render_engine,
			k3d::interface_list<k3d::icamera_still_render_engine,
			k3d::interface_list<k3d::icamera_preview_render_engine> > > > factory(
				k3d::uuid(0xef38bf93, 0x66654f9f, 0x992ca91b, 0x62bae139),
				"YafrayEngine",
				_("Yafray Render Engine"),
				"Yafray RenderEngines",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	const std::string shader_name(k3d::inode& Node)
	{
		if(k3d::imaterial_client* const material_client = dynamic_cast<k3d::imaterial_client*>(&Node))
		{
			if(k3d::imaterial* const material = material_client->material())
			{
				if(k3d::yafray::imaterial* const yafray_material = material->yafray_material())
					return dynamic_cast<k3d::inode*>(yafray_material)->name();
			}
		}

		return "default_shader";
	}

	const std::string shader_name(k3d::imaterial* Material)
	{
		if(Material)
		{
			if(k3d::yafray::imaterial* const yafray_material = Material->yafray_material())
				return dynamic_cast<k3d::inode*>(yafray_material)->name();
		}

		return "default_shader";
	}

	bool render(k3d::icamera& Camera, k3d::irender_frame& Frame, const k3d::filesystem::path& OutputImagePath, const bool VisibleRender)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImagePath.empty(), false);

		// Start our YafRay XML file ...
		const k3d::filesystem::path filepath = Frame.add_input_file("world.xml");
		return_val_if_fail(!filepath.empty(), false);

		// Open the RIB file stream ...
		k3d::filesystem::ofstream stream(filepath);
		return_val_if_fail(stream.good(), false);

		// Setup the frame for YafRay rendering ...
		Frame.add_render_operation("yafray", "yafray", filepath, VisibleRender);

		// Setup a YafRay scene description ...
		stream << "<scene>" << std::endl;

		// YafRay segfaults when a scene doesn't contain at least one shader, one object and one light, so keep track of what we create as we go ...
		bool found_object = false;
		bool found_light = false;

		// Get the document contents ...
		const k3d::nodes_t nodes(document().nodes().collection());

		// Setup shaders ...
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if(k3d::yafray::imaterial* const material = dynamic_cast<k3d::yafray::imaterial*>(*node))
				material->setup_material(stream);
		}

		// Add a default shader ...
		stream << "<shader type=\"generic\" name=\"default_shader\">" << std::endl;
		stream << "	<attributes>" << std::endl;
		stream << "		<color r=\"1.0\" g=\"1.0\" b=\"1.0\"/>" << std::endl;
		stream << "		<specular r=\"1.0\" g=\"1.0\" b=\"1.0\"/>" << std::endl;
		stream << "		<reflected r=\"1.0\" g=\"1.0\" b=\"1.0\"/>" << std::endl;
		stream << "		<transmitted r=\"0.0\" g=\"0.0\" b=\"0.0\"/>" << std::endl;
		stream << "		<hard value=\"128.0\"/>" << std::endl;
		stream << "		<IOR value=\"1.0\"/>" << std::endl;
		stream << "		<min_refle value=\"0\"/>" << std::endl;
		stream << "		<fast_fresnel value=\"off\"/>" << std::endl;
		stream << "	</attributes>" << std::endl;
		stream << "</shader>" << std::endl;

		// Setup lights ...
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if(k3d::yafray::ilight* const light = dynamic_cast<k3d::yafray::ilight*>(*node))
			{
				found_light = true;
				light->setup_light(stream);
			}
		}

		// YafRay segfaults if there isn't at least one light, so create something to keep it happy ...
		if(!found_light)
		{
			stream << "<light type=\"pointlight\" name=\"k3d_null_light\" power=\"0\">" << std::endl;
			stream << "	<from x=\"0\" y=\"0\" z=\"0\"/>" << std::endl;
			stream << "	<color r=\"1\" g=\"1\" b=\"1\"/>" << std::endl;
			stream << "</light>" << std::endl;
		}

		// Setup geometry
		for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			// Render sphere nodes ...
			if((*node)->factory().class_id() == k3d::classes::Sphere())
			{
				const k3d::point3 sphere_center = k3d::node_to_world_matrix(**node) * k3d::point3(0, 0, 0);
				const boost::any sphere_radius(k3d::get_value(**node, "radius"));
				if(typeid(double) == sphere_radius.type())
				{
					stream << "<object name=\"" << (*node)->name() << "\" shader_name=\"" << shader_name(**node) << "\">" << std::endl;
					stream << "	<attributes>" << std::endl;
					stream << "	</attributes>" << std::endl;
					stream << "	<sphere radius=\"" << boost::any_cast<double>(sphere_radius) << "\">" << std::endl;
					stream << "		<center x=\"" << std::fixed << -sphere_center[0] << "\" y=\"" << std::fixed << sphere_center[1] << "\" z=\"" << std::fixed << sphere_center[2] << "\"/>" << std::endl;
					stream << "	</sphere>" << std::endl;
					stream << "</object>" << std::endl;

					found_object = true;
				}

				continue;
			}

			// Render mesh nodes ...
			if((*node)->factory().class_id() == k3d::classes::MeshInstance())
			{
				if(!dynamic_cast<k3d::gl::idrawable*>(*node))
					continue;

				k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(*node);
				if(!mesh_source)
					continue;

				// Check for transformed output mesh
				k3d::dev::mesh* mesh = 0;
				if(k3d::iproperty* property = get_property(**node, "transformed_output_mesh"))
				{
					mesh = boost::any_cast<k3d::dev::mesh*>(property->property_value());
				}
				else
				{
					// Fall back to mesh source
					k3d::iproperty& property = mesh_source->mesh_source_output();
					mesh = boost::any_cast<k3d::dev::mesh*>(property.property_value());
				}

				if(!mesh)
					continue;

				k3d::legacy::mesh out_mesh;
				if (get_property(**node, "polyhedron_render_type"))
				{
					const std::string& render_type =  boost::any_cast<std::string>(k3d::get_internal_value(**node, "polyhedron_render_type"));
					assert_not_implemented();
//					sds_filter(*mesh, render_type, out_mesh, boost::any_cast<int>(k3d::get_internal_value(**node, "sds_render_level")));
				}

				k3d::legacy::polyhedron::faces_t new_faces;
				k3d::legacy::mesh::points_t new_points;

				for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = out_mesh.polyhedra.begin(); polyhedron != out_mesh.polyhedra.end(); ++polyhedron)
					k3d::legacy::triangulate((*polyhedron)->faces, new_faces, new_points);

				typedef std::multimap<k3d::imaterial*, k3d::legacy::face*> sorted_faces_t;
				sorted_faces_t sorted_faces;
				for(k3d::legacy::polyhedron::faces_t::iterator face = new_faces.begin(); face != new_faces.end(); ++face)
					sorted_faces.insert(std::make_pair((*face)->material, *face));

				unsigned long unique_id = 0;
				for(sorted_faces_t::iterator begin = sorted_faces.begin(); begin != sorted_faces.end(); begin = sorted_faces.upper_bound(begin->first))
				{
					const sorted_faces_t::iterator end = sorted_faces.upper_bound(begin->first);

					std::string material_name = "default_shader";
					bool shadow = true;
					bool emit_rad = true;
					bool recv_rad = true;
					bool caustics = true;
					double caus_IOR = 1.0;
					k3d::color caus_rcolor(0, 0, 0);
					k3d::color caus_tcolor(0, 0, 0);
					bool autosmooth = true;
					double autosmooth_value = 90;
					bool has_orco = false;

					if(begin->first)
					{
						if(k3d::yafray::imaterial* const material = begin->first->yafray_material())
						{
							material_name = dynamic_cast<k3d::inode*>(material)->name();
							shadow = boost::any_cast<bool>(k3d::get_internal_value(*material, "shadow"));
							emit_rad = boost::any_cast<bool>(k3d::get_internal_value(*material, "emit_rad"));
							recv_rad = boost::any_cast<bool>(k3d::get_internal_value(*material, "recv_rad"));
							caustics = boost::any_cast<bool>(k3d::get_internal_value(*material, "caustics"));
							caus_IOR = boost::any_cast<double>(k3d::get_internal_value(*material, "caus_IOR"));
							caus_rcolor = boost::any_cast<k3d::color>(k3d::get_internal_value(*material, "caus_rcolor"));
							caus_tcolor = boost::any_cast<k3d::color>(k3d::get_internal_value(*material, "caus_tcolor"));
							autosmooth = boost::any_cast<bool>(k3d::get_internal_value(*material, "mesh_autosmooth"));
							autosmooth_value = boost::any_cast<double>(k3d::get_internal_value(*material, "mesh_autosmooth_value"));
							has_orco = boost::any_cast<bool>(k3d::get_internal_value(*material, "has_orco"));
						}
					}

					stream
						<< "<object name=\"" << (*node)->name() << k3d::string_cast(++unique_id)
						<< "\" shader_name=\"" << shader_name(begin->first)
						<< "\" shadow=\"" << (shadow ? "on" : "off")
						<< "\" emit_rad=\"" << (emit_rad ? "on" : "off")
						<< "\" recv_rad=\"" << (recv_rad ? "on" : "off")
						<< "\" caustics=\"" << (caustics ? "on" : "off")
						<< "\" caus_IOR=\"" << caus_IOR << "\">" << std::endl;

					stream
						<< "	<attributes>" << std::endl;

					stream
						<< "		<caus_rcolor r=\"" << caus_rcolor.red
						<< "\" g=\"" << caus_rcolor.green
						<< "\" b=\"" << caus_rcolor.blue << "\"/>" << std::endl;

					stream
						<< "		<caus_tcolor r=\"" << caus_tcolor.red
						<< "\" g=\"" << caus_tcolor.green
						<< "\" b=\"" << caus_tcolor.blue << "\"/>" << std::endl;

					stream << "	</attributes>" << std::endl;
					if(!autosmooth)
						stream << "	<mesh>" << std::endl;
					else
						stream << "	<mesh autosmooth=\"" << autosmooth_value << "\">" << std::endl;

					typedef std::set<k3d::legacy::point*> points_t;
					points_t points;
					for(sorted_faces_t::const_iterator face = begin; face != end; ++face)
					{
						k3d::legacy::split_edge* const e0 = face->second->first_edge;
						k3d::legacy::split_edge* const e1 = e0 ? e0->face_clockwise : 0;
						k3d::legacy::split_edge* const e2 = e1 ? e1->face_clockwise : 0;

						if(e0->vertex)
							points.insert(e0->vertex);
						if(e1->vertex)
							points.insert(e1->vertex);
						if(e2->vertex)
							points.insert(e2->vertex);
					}

					stream << "		<points>" << std::endl;

					std::map<k3d::legacy::point*, unsigned long> point_indices;
					for(points_t::const_iterator point = points.begin(); point != points.end(); ++point)
					{
						point_indices.insert(std::make_pair(*point, point_indices.size()));
						stream
							<< "			<p x=\"" << std::fixed << -(*point)->position[0]
							<< "\" y=\"" << std::fixed << (*point)->position[1]
							<< "\" z=\"" << std::fixed << (*point)->position[2] << "\"/>" << std::endl;
					}
					stream << "		</points>" << std::endl;

					stream << "		<faces>" << std::endl;
					for(sorted_faces_t::const_iterator face = begin; face != end; ++face)
					{
						k3d::legacy::split_edge* const e0 = face->second->first_edge;
						k3d::legacy::split_edge* const e1 = e0 ? e0->face_clockwise : 0;
						k3d::legacy::split_edge* const e2 = e1 ? e1->face_clockwise : 0;

						if(!e0 || !e1 || !e2 || !e0->vertex || !e1->vertex || !e2->vertex)
							continue;

						stream
							<< "			<f a=\"" << point_indices[e0->vertex]
							<< "\" b=\"" << point_indices[e1->vertex]
							<< "\" c=\""<< point_indices[e2->vertex] << "\"/>" << std::endl;
					}
					stream << "		</faces>" << std::endl;

					stream << "	</mesh>" << std::endl;
					stream << "</object>" << std::endl;

					found_object = true;
				}

				std::for_each(new_faces.begin(), new_faces.end(), k3d::delete_object());
				std::for_each(new_points.begin(), new_points.end(), k3d::delete_object());

				continue;
			}
		}

		// YafRay segfaults if there isn't at least one object, so create something invisible to keep it happy ...
		if(!found_object)
		{
			stream << "<object name=\"default_object\" shader_name=\"default_shader\">" << std::endl;
			stream << "	<attributes>" << std::endl;
			stream << "	</attributes>" << std::endl;
			stream << "	<sphere radius=\"0.0\">" << std::endl;
			stream << "		<center x=\"0\" y=\"0\" z=\"0\"/>" << std::endl;
			stream << "	</sphere>" << std::endl;
			stream << "</object>" << std::endl;
		}

		// Setup the camera ...
		const k3d::matrix4 camera_matrix = boost::any_cast<k3d::matrix4>(get_value(document().dag(), Camera.transformation().transform_source_output()));
		const k3d::point3 camera_position = k3d::position(camera_matrix);
		const k3d::point3 camera_to_vector = camera_matrix * k3d::point3(0, 0, 1);
		const k3d::point3 camera_up_vector = camera_matrix * k3d::point3(0, 1, 0);

		stream << "<camera name=\"camera\" resx=\"" << m_pixel_width.value() << "\" resy=\"" << m_pixel_height.value() << "\" focal=\"0.7\">" << std::endl;
		stream << "	<from x=\"" << -camera_position[0] << "\" y=\"" << camera_position[1] << "\" z=\"" << camera_position[2] << "\"/>" << std::endl;
		stream << "	<to x=\"" << -camera_to_vector[0] << "\" y=\"" << camera_to_vector[1] << "\" z=\"" << camera_to_vector[2] << "\"/>" << std::endl;
		stream << "	<up x=\"" << -camera_up_vector[0] << "\" y=\"" << camera_up_vector[1] << "\" z=\"" << camera_up_vector[2] << "\"/>" << std::endl;
		stream << "</camera>" << std::endl;

		// Generate the output file ...
		const k3d::color fog_color = m_fog_color.value();

		stream << "<render camera_name=\"camera\" AA_passes=\"" << m_AA_passes.value() << "\"" << " AA_minsamples=\"" << m_AA_minsamples.value() << "\" AA_pixelwidth=\"" << m_AA_pixelwidth.value() << "\" AA_threshold=\"" << m_AA_threshold.value() << "\" raydepth=\"" << m_raydepth.value() << "\" bias=\"" << m_bias.value() << "\">" << std::endl;
		stream << "	<outfile value=\"" << OutputImagePath.native_filesystem_string() << "\"/>" << std::endl;
		stream << "	<save_alpha value=\"" << (m_save_alpha.value() ? "on" : "off") << "\"/>" << std::endl;
		stream << "	<exposure value=\"" << m_exposure.value() << "\"/>" << std::endl;
		stream << "	<gamma value=\"" << m_gamma.value() << "\"/>" << std::endl;
		stream << "	<fog_density value=\"" << m_fog_density.value() << "\"/>" << std::endl;
		stream << "	<fog_color r=\"" << fog_color.red << "\" g=\"" << fog_color.green << "\" b=\"" << fog_color.blue << "\"/>" << std::endl;
		stream << "</render>" << std::endl;

		// Finish the scene ...
		stream << "</scene>" << std::endl;

		return true;
	}

	/// Apply SDS if needed
	void sds_filter(const k3d::legacy::mesh& Input, const std::string& RenderType, k3d::legacy::mesh& Output, int Levels)
	{
		if (!m_preview_sds.value() || !(Input.polyhedra.size() > 0 && (RenderType == "catmull-clark")))
		{
			k3d::legacy::deep_copy(Input, Output);
			return;
		}
//		k3d::sds::k3d_mesh_sds_cache sds_cache;
//
//		// Set levels -before- input
//		sds_cache.set_levels(Levels);
//		sds_cache.set_input(&Input);
//		sds_cache.update();
//		sds_cache.output(&Output);
	}

	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_resolution;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_width;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_pixel_height;

	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_AA_passes;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_AA_minsamples;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_AA_pixelwidth;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_AA_threshold;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_raydepth;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bias;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_save_alpha;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_exposure;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gamma;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_fog_density;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_fog_color;
	// SDS options
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_preview_sds;
};

k3d::iplugin_factory& render_engine_factory()
{
	return render_engine::get_factory();
}

} // namespace libk3dyafray

