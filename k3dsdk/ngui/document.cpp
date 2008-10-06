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
#include <k3dsdk/classes.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/imetadata.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/share.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/user_properties.h>

#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

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
// setup_camera_document

void setup_camera_document(k3d::idocument& Document)
{
	return_if_fail(k3d::plugin::factory::lookup("Camera"));

	k3d::inode* const camera = k3d::plugin::create<k3d::inode>("Camera", Document, "Camera");
	return_if_fail(camera);

	const k3d::point3 origin = k3d::point3(0, 0, 0);
	const k3d::vector3 world_up = k3d::vector3(0, 0, 1);

	const k3d::point3 position = k3d::point3(-15, 20, 10);
	const k3d::vector3 look_vector = origin - position;
	const k3d::vector3 right_vector = look_vector ^ world_up;
	const k3d::vector3 up_vector = right_vector ^ look_vector;

	k3d::inode* const camera_transformation = k3d::set_matrix(*camera, k3d::view_matrix(look_vector, up_vector, position));
	return_if_fail(camera_transformation);

	camera_transformation->set_name("Camera Transformation");
	k3d::property::set_internal_value(*camera, "world_target", k3d::point3(0, 0, 0));
}

/////////////////////////////////////////////////////////////////////////////
// setup_renderman_document

void setup_renderman_document(k3d::idocument& Document)
{
	return_if_fail(k3d::plugin::factory::lookup("MultiMaterial"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManBicubicPatchPainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManBilinearPatchPainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManBlobbyPainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManCubicCurvePainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManEngine"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManLight"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManLightShader"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManLinearCurvePainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManMaterial"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManMultiPainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManNURBSPatchPainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManPointGroupPainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManPolyhedronPainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManSubdivisionSurfacePainter"));
	return_if_fail(k3d::plugin::factory::lookup("RenderManSurfaceShader"));

	// Setup RenderMan painters ...
	k3d::inode* const multi_painter = k3d::plugin::create<k3d::inode>("RenderManMultiPainter", Document, "RenderMan Default Painter");
	return_if_fail(multi_painter);

	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "point_groups", "Point Groups", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManPointGroupPainter", Document, "RenderMan Point Group Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "polyhedra", "Polyhedra", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManPolyhedronPainter", Document, "RenderMan Polyhedron Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "subdivision_surfaces", "Subdivision Surfaces", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManSubdivisionSurfacePainter", Document, "RenderMan Subdivision Surface Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "linear_curves", "Linear Curves", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManLinearCurvePainter", Document, "RenderMan Linear Curve Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "cubic_curves", "Cubic Curves", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManCubicCurvePainter", Document, "RenderMan Cubic Curve Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "bilinear_patches", "Bilinear Patches", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManBilinearPatchPainter", Document, "RenderMan Bilinear Patch Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "bicubic_patches", "Bicubic Patches", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManBicubicPatchPainter", Document, "RenderMan Bicubic Patch Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "nurbs_patches", "NURBS Patches", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManNURBSPatchPainter", Document, "RenderMan NURBS Patch Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "blobbies", "Blobbies", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManBlobbyPainter", Document, "RenderMan Blobby Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "cones", "Cones", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManConePainter", Document, "RenderMan Cone Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "cylinders", "Cylinders", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManCylinderPainter", Document, "RenderMan Cylinder Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "disks", "Disks", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManDiskPainter", Document, "RenderMan Disk Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "hyperboloids", "Hyperboloids", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManHyperboloidPainter", Document, "RenderMan Hyperboloid Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "paraboloids", "Paraboloids", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManParaboloidPainter", Document, "RenderMan Paraboloid Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "spheres", "Spheres", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManSpherePainter", Document, "RenderMan Sphere Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "teapots", "Teapots", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManTeapotPainter", Document, "RenderMan Teapot Painter"));
	k3d::property::create<k3d::ri::imesh_painter*>(*multi_painter, "tori", "Tori", "", k3d::plugin::create<k3d::ri::imesh_painter>("RenderManTorusPainter", Document, "RenderMan Torus Painter"));

	// Setup a default RenderMan shading model ...
	k3d::inode* const surface_shader = k3d::plugin::create<k3d::inode>("RenderManSurfaceShader", Document, "Surface Shader");
	return_if_fail(surface_shader);
	k3d::property::set_internal_value(*surface_shader, "shader_path", k3d::share_path() / k3d::filesystem::generic_path("shaders/surface/k3d_plastic.sl"));

	k3d::imaterial* const renderman_material = k3d::plugin::create<k3d::imaterial>("RenderManMaterial", Document, "RenderManMaterial");
	return_if_fail(renderman_material);
	k3d::property::set_internal_value(*renderman_material, "surface_shader", surface_shader);

	k3d::inode* const light_shader = k3d::plugin::create<k3d::inode>("RenderManLightShader", Document, "Light Shader");
	return_if_fail(light_shader);
	k3d::property::set_internal_value(*light_shader, "shader_path", k3d::share_path() / k3d::filesystem::generic_path("shaders/light/k3d_pointlight.sl"));

	k3d::inode* const light = k3d::plugin::create<k3d::inode>("RenderManLight", Document, "Light");
	return_if_fail(light);
	k3d::property::set_internal_value(*light, "shader", light_shader);

	k3d::inode* light_transformation = k3d::set_matrix(*light, k3d::translation3D(k3d::point3(-20, 20, 30)));
	return_if_fail(light_transformation);
	light_transformation->set_name("Light Transformation");

	k3d::inode* const default_material = k3d::plugin::create<k3d::inode>(k3d::classes::MultiMaterial(), Document, "Material");
	return_if_fail(default_material);

	k3d::property::create<k3d::imaterial*>(*default_material, "renderman_material", "RenderMan Material", "", renderman_material);

	// Setup a default choice of external render engine ...
	k3d::ri::irender_engine* const bundled_aqsis = k3d::plugin::factory::lookup("BundledAqsisRenderManEngine") ? k3d::plugin::create<k3d::ri::irender_engine>("BundledAqsisRenderManEngine", Document, "BundledAqsis") : 0;
	k3d::ri::irender_engine* const aqsis = k3d::plugin::create<k3d::ri::irender_engine>("AqsisRenderManEngine", Document, "Aqsis");
	k3d::ri::irender_engine* const pixie = k3d::plugin::create<k3d::ri::irender_engine>("PixieRenderManEngine", Document, "Pixie");

	k3d::ri::irender_engine* default_render_engine = 0;
	if(!default_render_engine && bundled_aqsis && bundled_aqsis->installed())
		default_render_engine = bundled_aqsis;
	if(!default_render_engine && aqsis && aqsis->installed())
		default_render_engine = aqsis;
	if(!default_render_engine && pixie && pixie->installed())
		default_render_engine = pixie;

	// Setup a render engine ...
	k3d::iunknown* const render_engine = k3d::plugin::create("RenderManEngine", Document, "RenderMan Engine");
	k3d::property::set_internal_value(*render_engine, "enabled_lights", k3d::inode_collection_property::nodes_t(1, light));
	k3d::property::set_internal_value(*render_engine, "render_engine", dynamic_cast<k3d::inode*>(default_render_engine));
}

/////////////////////////////////////////////////////////////////////////////
// setup_selection_document

/// Adds a node to store the document node selection
k3d::inode* setup_selection_document(k3d::idocument& Document)
{
	k3d::imetadata* node_selection = k3d::plugin::create<k3d::imetadata>("NodeSelection", Document, "Node Selection");
	node_selection->set_metadata("ngui:unique_node", "node_selection"); // metadata to ensure this node is found by the UI layer
	return dynamic_cast<k3d::inode*>(node_selection);
}

/////////////////////////////////////////////////////////////////////////////
// setup_document

/// Adds nodes to a newly-created document to give the user a better out-of-box experience
void setup_document(k3d::idocument& Document)
{
	k3d::plugin::create(k3d::classes::Axes(), Document, "Axes");
	k3d::iunknown* gl_engine = k3d::plugin::create(k3d::classes::OpenGLEngine(), Document, "GL Engine");
	k3d::plugin::create(k3d::classes::TimeSource(), Document, "TimeSource");
	
	setup_camera_document(Document);
	setup_renderman_document(Document);
	
	return_if_fail(k3d::property::set_internal_value(*gl_engine, "node_selection", setup_selection_document(Document)));
}

/// Add document nodes that need an opengl context for extension checking
void setup_opengl_document(k3d::idocument& Document)
{
	return_if_fail(k3d::plugin::factory::lookup("OpenGLBicubicPatchPainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLBilinearPatchPainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLBlobbyPointPainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLCubicCurvePainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLFaceNormalPainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLFaceOrientationPainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLLinearCurvePainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLMultiPainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLNURBSCurvePainter"));
	return_if_fail(k3d::plugin::factory::lookup("OpenGLNURBSPatchPainter"));
	return_if_fail(k3d::plugin::factory::lookup("VirtualOpenGLEdgePainter"));
	return_if_fail(k3d::plugin::factory::lookup("VirtualOpenGLFacePainter"));
	return_if_fail(k3d::plugin::factory::lookup("VirtualOpenGLPointPainter"));
	return_if_fail(k3d::plugin::factory::lookup("VirtualOpenGLSDSEdgePainter"));
	return_if_fail(k3d::plugin::factory::lookup("VirtualOpenGLSDSFacePainter"));
	return_if_fail(k3d::plugin::factory::lookup("VirtualOpenGLSDSPointPainter"));

	k3d::inode* const multi_painter = k3d::plugin::create<k3d::inode>("OpenGLMultiPainter", Document, "GL Default Painter");
	return_if_fail(multi_painter);

	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "points", "Points", "", k3d::plugin::create<k3d::gl::imesh_painter>("VirtualOpenGLPointPainter", Document, "GL Point Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "edges", "Edges", "", k3d::plugin::create<k3d::gl::imesh_painter>("VirtualOpenGLEdgePainter", Document, "GL Edge Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "faces", "Faces", "", k3d::plugin::create<k3d::gl::imesh_painter>("VirtualOpenGLFacePainter", Document, "GL Face Painter"));

	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "sds_points", "SDS Points", "", k3d::plugin::create<k3d::gl::imesh_painter>("VirtualOpenGLSDSPointPainter", Document, "SDS Point Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "sds_edges", "SDS Edges", "", k3d::plugin::create<k3d::gl::imesh_painter>("VirtualOpenGLSDSEdgePainter", Document, "SDS Edge Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "sds_faces", "SDS Faces", "", k3d::plugin::create<k3d::gl::imesh_painter>("VirtualOpenGLSDSFacePainter", Document, "SDS Face Painter"));

	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "linear_curves", "Linear Curves", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLLinearCurvePainter", Document, "GL Linear Curve Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "cubic_curves", "Cubic Curves", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLCubicCurvePainter", Document, "GL Cubic Curve Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "nurbs_curves", "NURBS Curves", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLNURBSCurvePainter", Document, "GL NURBS Curve Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "bilinear_patches", "Bilinear Patches", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLBilinearPatchPainter", Document, "GL Bilinear Patch Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "bicubic_patches", "Bicubic Patches", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLBicubicPatchPainter", Document, "GL Bicubic Patch Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "nurbs_patches", "NURBS Patches", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLNURBSPatchPainter", Document, "GL NURBS Patch Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "blobbies", "Blobbies", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLBlobbyPointPainter", Document, "GL Blobby Point Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "face_normals", "Face Normals", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLFaceNormalPainter", Document, "GL Face Normal Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "face_orientation", "Face Orientation", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLFaceOrientationPainter", Document, "GL Face Orientation Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "cones", "Cones", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLConePainter", Document, "GL Cone Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "cylinders", "Cylinders", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLCylinderPainter", Document, "GL Cylinder Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "disks", "Disks", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLDiskPainter", Document, "GL Disk Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "hyperboloids", "Hyperboloids", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLHyperboloidPainter", Document, "GL Hyperboloid Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "paraboloids", "Paraboloids", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLParaboloidPainter", Document, "GL Paraboloid Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "spheres", "Spheres", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLSpherePainter", Document, "GL Sphere Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "teapots", "Teapots", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLTeapotPainter", Document, "GL Teapot Painter"));
	k3d::property::create<k3d::gl::imesh_painter*>(*multi_painter, "tori", "Tori", "", k3d::plugin::create<k3d::gl::imesh_painter>("OpenGLTorusPainter", Document, "GL Torus Painter"));
}

} // namespace detail

void create_document()
{
	k3d::idocument* const document = k3d::application().create_document();
	return_if_fail(document);

	const k3d::ustring new_title = k3d::ustring::from_utf8(k3d::string_cast(boost::format(_("Untitled Document %1%")) % detail::next_document_number()));
	k3d::property::set_internal_value(document->title(), new_title);

	detail::setup_document(*document);

	document_state* const state = new document_state(*document);
	create_main_document_window(*state);
	detail::setup_opengl_document(*document);
}

void open_document(const k3d::filesystem::path& Path)
{
	boost::scoped_ptr<k3d::idocument_importer> importer(k3d::plugin::create<k3d::idocument_importer>(k3d::classes::DocumentImporter()));
	if(!importer)
	{
		error_message(_("Document importer plugin not installed."));
		return;
	}

	k3d::idocument* const document = k3d::application().create_document();
	return_if_fail(document);

	if(!importer->read_file(*document, Path))
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

