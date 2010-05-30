// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3d-ngui-config.h>

#ifdef K3D_BUILD_NGUI_MODULE
#include <k3dsdk/python/atk_python.h>
#endif // K3D_BUILD_NGUI_MODULE

#include <k3dsdk/python/angle_axis_python.h>
#include <k3dsdk/python/any_python.h>
#include <k3dsdk/python/table_python.h>
#include <k3dsdk/python/bezier_triangle_patch_python.h>
#include <k3dsdk/python/bicubic_patch_python.h>
#include <k3dsdk/python/bilinear_patch_python.h>
#include <k3dsdk/python/bitmap_python.h>
#include <k3dsdk/python/blobby_python.h>
#include <k3dsdk/python/bounding_box3_python.h>
#include <k3dsdk/python/color_python.h>
#include <k3dsdk/python/cone_python.h>
#include <k3dsdk/python/const_table_python.h>
#include <k3dsdk/python/const_bitmap_python.h>
#include <k3dsdk/python/const_named_arrays_python.h>
#include <k3dsdk/python/const_named_tables_python.h>
#include <k3dsdk/python/const_typed_array_python.h>
#include <k3dsdk/python/cubic_curve_python.h>
#include <k3dsdk/python/cylinder_python.h>
#include <k3dsdk/python/difference_python.h>
#include <k3dsdk/python/disk_python.h>
#include <k3dsdk/python/euler_angles_python.h>
#include <k3dsdk/python/euler_python.h>
#include <k3dsdk/python/filesystem_python.h>
#include <k3dsdk/python/file_signal_python.h>
#include <k3dsdk/python/geometry_python.h>
#include <k3dsdk/python/hyperboloid_python.h>
#include <k3dsdk/python/idocument_python.h>
#include <k3dsdk/python/ifile_change_notifier_python.h>
#include <k3dsdk/python/iunknown_python.h>
#include <k3dsdk/python/linear_curve_python.h>
#include <k3dsdk/python/log_python.h>
#include <k3dsdk/python/matrix4_python.h>
#include <k3dsdk/python/mesh_python.h>
#include <k3dsdk/python/mime_python.h>
#include <k3dsdk/python/named_arrays_python.h>
#include <k3dsdk/python/named_tables_python.h>
#include <k3dsdk/python/node_python.h>
#include <k3dsdk/python/normal3_python.h>
#include <k3dsdk/python/nurbs_curve_python.h>
#include <k3dsdk/python/nurbs_patch_python.h>
#include <k3dsdk/python/object_model_python.h>
#include <k3dsdk/python/paraboloid_python.h>
#include <k3dsdk/python/parallel_python.h>
#include <k3dsdk/python/plugin_python.h>
#include <k3dsdk/python/point2_python.h>
#include <k3dsdk/python/point3_python.h>
#include <k3dsdk/python/point4_python.h>
#include <k3dsdk/python/particle_python.h>
#include <k3dsdk/python/polyhedron_python.h>
#include <k3dsdk/python/property_python.h>
#include <k3dsdk/python/resource_python.h>
#include <k3dsdk/python/ri_python.h>
#include <k3dsdk/python/script_python.h>
#include <k3dsdk/python/selection_python.h>
#include <k3dsdk/python/sphere_python.h>
#include <k3dsdk/python/teapot_python.h>
#include <k3dsdk/python/texture3_python.h>
#include <k3dsdk/python/torus_python.h>
#include <k3dsdk/python/typed_array_python.h>
#include <k3dsdk/python/uuid_python.h>
#include <k3dsdk/python/vector2_python.h>
#include <k3dsdk/python/vector3_python.h>
#include <k3dsdk/python/vector4_python.h>

#include <k3dsdk/algebra.h>
#include <k3dsdk/application.h>
#include <k3dsdk/batch_mode.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/geometric_operations.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iplugin_factory_collection.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/render_state_ri.h> // MinGW needs typeinfo
#include <k3dsdk/scripting.h>
#include <k3dsdk/share.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_interface.h>

#include <boost/algorithm/string.hpp>

#include <boost/python.hpp>
using namespace boost::python;

#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

////////////////////////////////////////////////////////////////////////////////////
// k3d module helpers

object module_create_plugin(const string_t& Type)
{
	throw std::runtime_error("k3d.create_plugin() has been removed, use k3d.plugin.create() instead.");
}

void module_check_node_environment(const k3d::iscript_engine::context& Context, const string_t& PluginType)
{
	k3d::iscript_engine::context::const_iterator n = Context.find("node");
	if(n != Context.end())
	{
		if(k3d::inode* const node = boost::any_cast<k3d::inode*>(n->second))
		{
			if(node->factory().name() == PluginType)
			{
				return;
			}
		}
	}

	k3d::user_interface().error_message(k3d::string_cast(boost::format("This script can only be used from within a %1% plugin.") % PluginType));
	throw std::runtime_error("script can only be run from " + PluginType);
}

const double module_length(const object& Value)
{
	extract<k3d::vector3> vector3(Value);
	if(vector3.check())
		return k3d::length(vector3());

	throw std::invalid_argument("can't calculate length for this type");
}

void module_log_critical(const string_t& Message)
{
	throw std::runtime_error("k3d.log_critical() has been removed, use k3d.log.critical() instead.");
}

void module_log_debug(const string_t& Message)
{
	throw std::runtime_error("k3d.log_debug() has been removed, use k3d.log.debug() instead.");
}

void module_log_error(const string_t& Message)
{
	throw std::runtime_error("k3d.log_error() has been removed, use k3d.log.error() instead.");
}

void module_log_info(const string_t& Message)
{
	throw std::runtime_error("k3d.log_info() has been removed, use k3d.log.info() instead.");
}

void module_log_warning(const string_t& Message)
{
	throw std::runtime_error("k3d.log_warning() has been removed, use k3d.log.warning() instead.");
}

const list module_plugins()
{
	throw std::runtime_error("k3d.plugins() has been removed, use k3d.plugin.factory.lookup() instead.");
}

static const k3d::matrix4 rotate3_a(const k3d::angle_axis& Value)
{
	return k3d::rotate3(Value);
}

static const k3d::matrix4 rotate3_b(const k3d::euler_angles& Value)
{
	return k3d::rotate3(k3d::quaternion(Value));
}

static const k3d::matrix4 scale3_a(const double_t X, const double_t Y, const double_t Z)
{
	return k3d::scale3(X, Y, Z);
}

static const k3d::matrix4 scale3_b(const double_t S)
{
	return k3d::scale3(S);
}

static const k3d::matrix4 translate3_a(const k3d::vector3& Offset)
{
	return k3d::translate3(Offset);
}

static const k3d::matrix4 translate3_b(const double_t X, const double_t Y, const double_t Z)
{
	return k3d::translate3(X, Y, Z);
}

static const k3d::matrix4 translate3_c(const boost::python::tuple& Tuple)
{
	if(len(Tuple) != 3)
		throw std::invalid_argument("3-tuple required.");

	return k3d::translate3(extract<double>(Tuple[0]), extract<double>(Tuple[1]), extract<double>(Tuple[2]));
}

object module_ui()
{
	return wrap_unknown(k3d::user_interface());
}

void module_exit()
{
	k3d::application().exit();
}

object module_new_document()
{
	return wrap(k3d::application().create_document());
}

list module_documents()
{
	list results;

	const k3d::iapplication::document_collection_t documents = k3d::application().documents();
	for(k3d::iapplication::document_collection_t::const_iterator document = documents.begin(); document != documents.end(); ++document)
		results.append(wrap(*document));

	return results;
}

void module_close_document(idocument_wrapper& Document)
{
	k3d::application().close_document(Document.wrapped());
}

object module_open_document(const k3d::filesystem::path& Path)
{
	boost::scoped_ptr<k3d::idocument_importer> importer(k3d::plugin::create<k3d::idocument_importer>(k3d::classes::DocumentImporter()));
	if(!importer)
		throw std::runtime_error("no importer plugin available");

	k3d::idocument* const document = k3d::application().create_document();
	if(!document)
		throw std::runtime_error("couldn't create empty document");

	if(!importer->read_file(Path, *document))
		throw std::runtime_error("error loading document");

	return wrap(document);
}

const k3d::vector3 module_to_vector3(const k3d::point3& v)
{
	return k3d::to_vector(v);
}

const k3d::point3 module_world_position(iunknown_wrapper& Node)
{
	return k3d::world_position(Node.wrapped());
}

object module_get_time(idocument_wrapper& Document)
{
	return wrap_unknown(k3d::get_time(Document.wrapped()));
}

////////////////////////////////////////////////////////////////////////////////////
// k3d module

BOOST_PYTHON_MODULE(k3d)
{
	define_typed_array_classes();
	define_const_typed_array_classes();

	define_class_angle_axis();
#ifdef K3D_BUILD_NGUI_MODULE
	define_class_atk_object();
#endif // K3D_BUILD_NGUI_MODULE
	define_class_table();
	define_class_bitmap();
	define_class_bounding_box3();
	define_class_color();
	define_class_const_table();
	define_class_const_bitmap();
	define_class_const_named_arrays();
	define_class_const_named_tables();
	define_class_euler_angles();
	define_class_file_change_receiver();
	define_class_file_signal();
	define_class_idocument();
	define_class_iunknown();
	define_class_matrix4();
	define_class_mesh();
	define_class_const_mesh();
	define_class_named_arrays();
	define_class_named_tables();
	define_class_normal3();
	define_class_point2();
	define_class_point3();
	define_class_point4();
	define_class_texture3();
	define_class_uuid();
	define_class_vector2();
	define_class_vector3();
	define_class_vector4();
	define_namespace_bezier_triangle_patch();
	define_namespace_bicubic_patch();
	define_namespace_bilinear_patch();
	define_namespace_blobby();
	define_namespace_cone();
	define_namespace_cubic_curve();
	define_namespace_cylinder();
	define_namespace_difference();
	define_namespace_disk();
	define_namespace_euler();
	define_namespace_filesystem();
	define_namespace_geometry();
	define_namespace_hyperboloid();
	define_namespace_linear_curve();
	define_namespace_log();
	define_namespace_mime();
	define_namespace_node();
	define_namespace_nurbs_curve();
	define_namespace_nurbs_patch();
	define_namespace_paraboloid();
	define_namespace_parallel();
	define_namespace_plugin();
	define_namespace_particle();
	define_namespace_polyhedron();
	define_namespace_property();
	define_namespace_resource();
	define_namespace_ri();
	define_namespace_script();
	define_namespace_selection();
	define_namespace_sphere();
	define_namespace_teapot();
	define_namespace_torus();

	def("batch_mode", k3d::batch_mode,
		"Returns True if batch (no user intervention) mode is enabled for the user interface.\n"
		"@note: Well-behaved scripts should not prompt the user for input if batch mode is enabled.");
	def("check_node_environment", module_check_node_environment,
		"Checks to see whether the current script is running from within the given node type.");
	def("close_document", module_close_document,
		"Closes an open document.");
	def("create_plugin", module_create_plugin,
		"Creates an application plugin instance by name (fails if there is no application plugin factory with the given name).");
	def("documents", module_documents,
		"Returns a list containing all open documents.");
	def("exit", module_exit,
		"Request program exit (may be overridden by user input).");
	def("identity3", k3d::identity3,
		"Returns a L{matrix4} containing a three-dimensional identity matrix.");
	def("intersect_lines", k3d::intersect_lines,
		"Find the point at which two infinite lines intersect.");
	def("inverse", k3d::inverse,
		"Computes the inverse of a matrix.");
	def("length", module_length,
		"Returns the length of a L{vector3}.");
	def("log_critical", module_log_critical,
		"Sends a critical message to the K-3D log.");
	def("log_debug", module_log_debug,
		"Sends a debug message to the K-3D log.");
	def("log_error", module_log_error,
		"Sends an error message to the K-3D log.");
	def("log_info", module_log_info,
		"Sends an informational message to the K-3D log.");
	def("log_warning", module_log_warning,
		"Sends a warning message to the K-3D log.");
	def("new_document", module_new_document,
		"Returns a new (completely empty) document.");
	def("open_document", module_open_document,
		"Opens an existing document stored on disk.");
	def("plugins", module_plugins,
		"Returns a list containing the set of all plugin factories.");
	def("rotate3", rotate3_a,
		"Returns a L{matrix4} containing a three-dimensional rotation matrix.");
	def("rotate3", rotate3_b,
		"Returns a L{matrix4} containing a three-dimensional rotation matrix.");
	def("scale3", scale3_a,
		"Returns a L{matrix4} containing a three-dimensional scaling matrix.");
	def("scale3", scale3_b,
		"Returns a L{matrix4} containing a three-dimensional scaling matrix.");
	def("share_path", k3d::share_path,
		"Returns the runtime path to shared data.");
	def("to_vector3", module_to_vector3,
		"Explicit conversion from point3 to vector3");
	def("translate3", translate3_a,
		"Returns a L{matrix4} containing a three-dimensional translation matrix.");
	def("translate3", translate3_b,
		"Returns a L{matrix4} containing a three-dimensional translation matrix.");
	def("translate3", translate3_c,
		"Returns a L{matrix4} containing a three-dimensional translation matrix.");
	def("ui", module_ui,
		"Returns the singleton runtime user interface plugin instance.");
	def("world_position", module_world_position,
		"Returns a node's position in world coordinates (returns the origin for non-transformable nodes)");
	def("get_time", module_get_time,
		"Returns the time property for a document (could return NULL)");

	scope().attr("__doc__") = "Provides access to the K-3D API";
}

} // namespace python

} // namespace k3d

