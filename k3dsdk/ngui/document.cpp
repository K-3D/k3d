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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include "document.h"
#include "document_state.h"
#include "main_document_window.h"
#include "messages.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/auto_ptr.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/property.h>
#include <k3dsdk/share.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/user_properties.h>

#include <boost/format.hpp>

namespace libk3dngui
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// next_document_number

/// Returns unique document numbers for the duration of the current session
unsigned long next_document_number()
{
	static unsigned long document_number = 0;
	return ++document_number;
}

/////////////////////////////////////////////////////////////////////////////
// populate_new_document

/// Adds nodes to a newly-created document to give the user a better out-of-box experience
void populate_new_document(k3d::idocument& Document)
{
	const k3d::ustring new_title = k3d::ustring::from_utf8(k3d::string_cast(boost::format(_("Untitled Document %1%")) % k3d::string_cast(detail::next_document_number())));
	k3d::property::set_internal_value(Document.title(), new_title);

	// Setup the plugins that no document can live without ...
	k3d::create_plugin(k3d::classes::Axes(), Document, "Axes");
	k3d::create_plugin(k3d::classes::OpenGLEngine(), Document, "GL Engine");
	k3d::create_plugin(k3d::classes::TimeSource(), Document, "TimeSource");

	if(k3d::inode* const camera = k3d::create_plugin<k3d::inode>(k3d::classes::Camera(), Document, "Camera"))
	{
		const k3d::point3 origin = k3d::point3(0, 0, 0);
		const k3d::vector3 world_up = k3d::vector3(0, 0, 1);

		const k3d::point3 position = k3d::point3(-15, 20, 10);
		const k3d::vector3 look_vector = origin - position;
		const k3d::vector3 right_vector = look_vector ^ world_up;
		const k3d::vector3 up_vector = right_vector ^ look_vector;

		k3d::inode* camera_transformation = k3d::set_matrix(*camera, k3d::view_matrix(look_vector, up_vector, position));
		return_if_fail(camera_transformation);
		camera_transformation->set_name("Camera Transformation");
		k3d::property::set_internal_value(*camera, "world_target", k3d::point3(0, 0, 0));
	}

	// Setup RenderMan painters ...
	if(k3d::inode* const ri_multi_painter = k3d::create_plugin<k3d::inode>("RenderManMultiPainter", Document, "RenderMan Default Painter"))
	{
		k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(ri_multi_painter);
		k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(ri_multi_painter);
		if(property_collection && persistent_container)
		{
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("point_groups", "Point Groups", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManPointGroupPainter", Document, "RenderMan Point Group Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("polyhedra", "Polyhedra", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManPolyhedronPainter", Document, "RenderMan Polyhedron Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("subdivision_surfaces", "Subdivision Surfaces", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManSubdivisionSurfacePainter", Document, "RenderMan Subdivision Surface Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("linear_curves", "Linear Curves", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManLinearCurvePainter", Document, "RenderMan Linear Curve Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("cubic_curves", "Cubic Curves", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManCubicCurvePainter", Document, "RenderMan Cubic Curve Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("bilinear_patches", "Bilinear Patches", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManBilinearPatchPainter", Document, "RenderMan Bilinear Patch Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("bicubic_patches", "Bicubic Patches", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManBicubicPatchPainter", Document, "RenderMan Bicubic Patch Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("nurbs_patches", "NURBS Patches", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManNURBSPatchPainter", Document, "RenderMan NURBS Patch Painter"));
			k3d::user::create_property<k3d::user::ri_mesh_painter_property>("blobbies", "Blobbies", "", Document, *property_collection, *persistent_container, ri_multi_painter, k3d::create_plugin<k3d::ri::imesh_painter>("RenderManBlobbyPainter", Document, "RenderMan Blobby Painter"));
		}
	}

	// Setup a default RenderMan shading model ...
	k3d::iunknown* const material = k3d::create_plugin(k3d::classes::RenderManMaterial(), Document, "Material");
	k3d::inode* const surface_shader = k3d::create_plugin<k3d::inode>(k3d::classes::RenderManSurfaceShader(), Document, "Surface Shader");
	k3d::inode* const light = k3d::create_plugin<k3d::inode>(k3d::classes::RenderManLight(), Document, "Light");
	k3d::inode* const light_shader = k3d::create_plugin<k3d::inode>(k3d::classes::RenderManLightShader(), Document, "Light Shader");
	k3d::create_plugin(k3d::classes::RenderManEngine(), Document, "RenderMan Engine");

	if(light)
	{
		k3d::inode* light_transformation = k3d::set_matrix(*light, k3d::translation3D(k3d::point3(-20, 20, 30)));
		return_if_fail(light_transformation);
		light_transformation->set_name("Light Transformation");
	}

	if(surface_shader)
		k3d::property::set_internal_value(*surface_shader, "shader_path", k3d::share_path() / k3d::filesystem::generic_path("shaders/surface/k3d_plastic.sl"));

	if(material)
		k3d::property::set_internal_value(*material, "surface_shader", surface_shader);

	if(light_shader)
		k3d::property::set_internal_value(*light_shader, "shader_path", k3d::share_path() / k3d::filesystem::generic_path("shaders/light/k3d_pointlight.sl"));

	if(light)
		k3d::property::set_internal_value(*light, "shader", light_shader);
}

/// Add document nodes that need an opengl context for extension checking
void opengl_populate_new_document(k3d::idocument& Document)
{
	// Setup OpenGL painters ...
	if(k3d::inode* const gl_multi_painter = k3d::create_plugin<k3d::inode>("OpenGLMultiPainter", Document, "GL Default Painter"))
	{
		k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(gl_multi_painter);
		k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(gl_multi_painter);
		if(property_collection && persistent_container)
		{
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("points", "Points", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("GLPointPainter", Document, "GL Point Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("edges", "Edges", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("GLEdgePainter", Document, "GL Edge Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("faces", "Faces", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("GLFacePainter", Document, "GL Face Painter"));

			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("sds_points", "SDS Points", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("GLSDSPointPainter", Document, "SDS Point Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("sds_edges", "SDS Edges", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("GLSDSEdgePainter", Document, "SDS Edge Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("sds_faces", "SDS Faces", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("GLSDSFacePainter", Document, "SDS Face Painter"));

			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("linear_curves", "Linear Curves", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLLinearCurvePainter", Document, "GL Linear Curve Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("cubic_curves", "Cubic Curves", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLCubicCurvePainter", Document, "GL Cubic Curve Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("nurbs_curves", "NURBS Curves", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLNURBSCurvePainter", Document, "GL NURBS Curve Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("bilinear_patches", "Bilinear Patches", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLBilinearPatchPainter", Document, "GL Bilinear Patch Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("bicubic_patches", "Bicubic Patches", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLBicubicPatchPainter", Document, "GL Bicubic Patch Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("nurbs_patches", "NURBS Patches", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLNURBSPatchPainter", Document, "GL NURBS Patch Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("blobbies", "Blobbies", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLBlobbyPointPainter", Document, "GL Blobby Point Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("face_normals", "Face Normals", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLFaceNormalPainter", Document, "GL Face Normal Painter"));
			k3d::user::create_property<k3d::user::gl_mesh_painter_property>("face_orientation", "Face Orientation", "", Document, *property_collection, *persistent_container, gl_multi_painter, k3d::create_plugin<k3d::gl::imesh_painter>("OpenGLFaceOrientationPainter", Document, "GL Face Orientation Painter"));
		}
	}
}

} // namespace detail

void create_document()
{
	k3d::idocument* const document = k3d::application().create_document();
	return_if_fail(document);

	detail::populate_new_document(*document);

	document_state* const state = new document_state(*document);
	create_main_document_window(*state);
	detail::opengl_populate_new_document(*document);
}

void open_document(const k3d::filesystem::path& Path)
{
	k3d::auto_ptr<k3d::idocument_importer> filter(k3d::create_plugin<k3d::idocument_importer>(k3d::classes::DocumentImporter()));
	if(!filter.get())
	{
		error_message(_("Document importer plugin not installed."));
		return;
	}

	k3d::idocument* const document = k3d::application().create_document();
	return_if_fail(document);

	if(!filter->read_file(*document, Path))
	{
		error_message(k3d::string_cast(boost::format(_("Error reading document %1%")) % Path.native_filesystem_string()));
		return;
	}

	document_state* const state = new document_state(*document);
	create_main_document_window(*state);

	k3d::property::set_internal_value(document->path(), Path);
	k3d::property::set_internal_value(document->title(), Path.leaf());
}

} // namespace libk3dngui

