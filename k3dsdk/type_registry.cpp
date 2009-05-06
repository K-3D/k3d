// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include "bitmap.h"
#include "color.h"
#include "i3d_2d_mapping.h"
#include "ibitmap_exporter.h"
#include "ibitmap_importer.h"
#include "ibitmap_sink.h"
#include "ibitmap_source.h"
#include "icamera.h"
#include "icolor_source.h"
#include "idisplacement_shader_ri.h"
#include "idocument_exporter.h"
#include "idocument_importer.h"
#include "idouble_source.h"
#include "ifile_change_notifier.h"
#include "iimager_shader_ri.h"
#include "iint32_source.h"
#include "ikeyframer.h"
#include "ilight_gl.h"
#include "ilight_ri.h"
#include "ilight_shader_ri.h"
#include "ilight_yafray.h"
#include "imaterial.h"
#include "imaterial_gl.h"
#include "imaterial_ri.h"
#include "imaterial_yafray.h"
#include "imesh_painter_gl.h"
#include "imesh_painter_ri.h"
#include "imesh_sink.h"
#include "imesh_source.h"
#include "imesh_storage.h"
#include "imime_type_handler.h"
#include "imulti_mesh_sink.h"
#include "inode.h"
#include "inode_selection.h"
#include "irender_animation.h"
#include "irender_camera_animation.h"
#include "irender_camera_frame.h"
#include "irender_camera_preview.h"
#include "irender_engine_ri.h"
#include "irender_frame.h"
#include "irender_preview.h"
#include "iscript_engine.h"
#include "istring_source.h"
#include "isurface_shader_ri.h"
#include "itexture_ri.h"
#include "itime_sink.h"
#include "itransform_array_1d.h"
#include "itransform_array_2d.h"
#include "itransform_array_3d.h"
#include "itransform_sink.h"
#include "itransform_source.h"
#include "iuri_handler.h"
#include "ivector3_source.h"
#include "ivolume_shader_ri.h"
#include "legacy_mesh.h"
#include "log.h"
#include "mesh.h"
#include "mesh_selection.h"
#include "texture3.h"
#include "type_registry.h"
#include "types.h"

#include <map>

#if defined(__GNUC__) &&  __GNUC__ >= 3

	#define K3D_HAVE_GCC_DEMANGLE

	// http://lists.debian.org/debian-gcc/2003/09/msg00055.html notes
	// that, in cxxabi.h of gcc-3.x for x < 4, this type is used before it
	// is declared.

	#if __GNUC__ == 3 && __GNUC_MINOR__ < 4
		class __class_type_info;
	#endif

	#include <cxxabi.h>
#endif

namespace k3d
{

namespace detail
{

/// "Wraps" a std::type_info object so it can be used as the key in a sorted container
struct type_info
{
	type_info(const std::type_info& Info) :
		info(Info)
	{
	}

	bool operator<(const type_info& RHS) const
	{
		return info.before(RHS.info);
	}

	const std::type_info& info;
};

/// Defines storage for a mapping of type_info objects to their corresponding string representation
typedef std::map<type_info, string_t> type_to_name_map_t;
/// Stores a mapping of type_info objects to their corresponding string representation
type_to_name_map_t type_to_name_map;

/// Defines storage for a mapping of string to the corresponding type_info
typedef std::map<string_t, type_info> name_to_type_map_t;
/// Stores a mapping of string to the corresponding type_info
name_to_type_map_t name_to_type_map;

void register_type(const std::type_info& Info, const string_t& Name)
{
	if(type_to_name_map.count(type_info(Info)))
	{
		k3d::log() << error << k3d_file_reference << ": attempt to re-register type [" << demangle(Info) << "] with existing name [" << type_to_name_map[type_info(Info)] << "] under new name [" << Name << "]" << std::endl;
		return;
	}

	if(type_to_name_map.count(type_info(Info)) || name_to_type_map.count(Name))
	{
		k3d::log() << error << k3d_file_reference << ": attempt to register new type [" << demangle(Info) << "] using existing name [" << Name << "]" << std::endl;
		return;
	}

	type_to_name_map.insert(std::make_pair(type_info(Info), Name));
	name_to_type_map.insert(std::make_pair(Name, type_info(Info)));
}

void initialize_types()
{
	static bool initialized = false;
	if(initialized)
		return;

	register_type(typeid(k3d::angle_axis), "k3d::angle_axis");
	register_type(typeid(k3d::bitmap), "k3d::bitmap");
	register_type(typeid(k3d::bitmap*), "k3d::bitmap*");
	register_type(typeid(k3d::bool_t), "k3d::bool_t");
	register_type(typeid(k3d::bounding_box3), "k3d::bounding_box3");
	register_type(typeid(k3d::color), "k3d::color");
	register_type(typeid(k3d::double_t), "k3d::double_t");
	register_type(typeid(k3d::filesystem::path), "k3d::filesystem::path");
	register_type(typeid(k3d::float_t), "k3d::float_t");
	register_type(typeid(k3d::gl::ilight), "k3d::gl::ilight");
	register_type(typeid(k3d::gl::imaterial), "k3d::gl::imaterial");
	register_type(typeid(k3d::gl::imesh_painter), "k3d::gl::imesh_painter");
	register_type(typeid(k3d::gl::imesh_painter*), "k3d::gl::imesh_painter*");
	register_type(typeid(k3d::half_t), "k3d::half_t");
	register_type(typeid(k3d::i3d_2d_mapping), "k3d::i3d_2d_mapping");
	register_type(typeid(k3d::ibitmap_exporter), "k3d::ibitmap_exporter");
	register_type(typeid(k3d::ibitmap_importer), "k3d::ibitmap_importer");
	register_type(typeid(k3d::ibitmap_sink), "k3d::ibitmap_sink");
	register_type(typeid(k3d::ibitmap_source), "k3d::ibitmap_source");
	register_type(typeid(k3d::icamera), "k3d::icamera");
	register_type(typeid(k3d::icolor_source), "k3d::icolor_source");
	register_type(typeid(k3d::idocument_exporter), "k3d::idocument_exporter");
	register_type(typeid(k3d::idocument_importer), "k3d::idocument_importer");
	register_type(typeid(k3d::idouble_source), "k3d::idouble_source");
	register_type(typeid(k3d::ifile_change_notifier), "k3d::ifile_change_notifier");
	register_type(typeid(k3d::iint32_source), "k3d::iint32_source");
	register_type(typeid(k3d::ikeyframer), "k3d::ikeyframer");
	register_type(typeid(k3d::imaterial), "k3d::imaterial");
	register_type(typeid(k3d::imaterial*), "k3d::imaterial*");
	register_type(typeid(k3d::imesh_sink), "k3d::imesh_sink");
	register_type(typeid(k3d::imesh_source), "k3d::imesh_source");
	register_type(typeid(k3d::imesh_storage), "k3d::imesh_storage");
	register_type(typeid(k3d::imime_type_handler), "k3d::imime_type_handler");
	register_type(typeid(k3d::imulti_mesh_sink), "k3d::imulti_mesh_sink");
	register_type(typeid(k3d::inode), "k3d::inode");
	register_type(typeid(k3d::inode*), "k3d::inode*");
	register_type(typeid(k3d::inode_selection), "k3d::inode_selection");
	register_type(typeid(k3d::inode_selection*), "k3d::inode_selection*");
	register_type(typeid(k3d::int16_t), "k3d::int16_t");
	register_type(typeid(k3d::int32_t), "k3d::int32_t");
	register_type(typeid(k3d::int64_t), "k3d::int64_t");
	register_type(typeid(k3d::int8_t), "k3d::int8_t");
	register_type(typeid(k3d::irender_animation), "k3d::irender_animation");
	register_type(typeid(k3d::irender_camera_animation), "k3d::irender_camera_animation");
	register_type(typeid(k3d::irender_camera_frame), "k3d::irender_camera_frame");
	register_type(typeid(k3d::irender_camera_preview), "k3d::irender_camera_preview");
	register_type(typeid(k3d::irender_frame), "k3d::irender_frame");
	register_type(typeid(k3d::irender_preview), "k3d::irender_preview");
	register_type(typeid(k3d::iscript_engine), "k3d::iscript_engine");
	register_type(typeid(k3d::istring_source), "k3d::istring_source");
	register_type(typeid(k3d::itime_sink), "k3d::itime_sink");
	register_type(typeid(k3d::itransform_array_1d), "k3d::itransform_array_1d");
	register_type(typeid(k3d::itransform_array_2d), "k3d::itransform_array_2d");
	register_type(typeid(k3d::itransform_array_3d), "k3d::itransform_array_3d");
	register_type(typeid(k3d::itransform_sink), "k3d::itransform_sink");
	register_type(typeid(k3d::itransform_source), "k3d::itransform_source");
	register_type(typeid(k3d::iunknown), "k3d::iunknown");
	register_type(typeid(k3d::iunknown*), "k3d::iunknown*");
	register_type(typeid(k3d::iuri_handler), "k3d::iuri_handler");
	register_type(typeid(k3d::ivector3_source), "k3d::ivector3_source");
	register_type(typeid(k3d::legacy::mesh), "k3d::legacy::mesh");
	register_type(typeid(k3d::legacy::mesh*), "k3d::legacy::mesh*");
	register_type(typeid(k3d::matrix4), "k3d::matrix4");
	register_type(typeid(k3d::mesh), "k3d::mesh");
	register_type(typeid(k3d::mesh*), "k3d::mesh*");
	register_type(typeid(k3d::mesh_selection), "k3d::mesh_selection");
	register_type(typeid(k3d::normal3), "k3d::normal3");
	register_type(typeid(k3d::point2), "k3d::point2");
	register_type(typeid(k3d::point3), "k3d::point3");
	register_type(typeid(k3d::point4), "k3d::point4");
	register_type(typeid(k3d::ri::idisplacement_shader), "k3d::ri::idisplacement_shader");
	register_type(typeid(k3d::ri::iimager_shader), "k3d::ri::iimager_shader");
	register_type(typeid(k3d::ri::ilight), "k3d::ri::ilight");
	register_type(typeid(k3d::ri::ilight_shader), "k3d::ri::ilight_shader");
	register_type(typeid(k3d::ri::imaterial), "k3d::ri::imaterial");
	register_type(typeid(k3d::ri::imesh_painter), "k3d::ri::imesh_painter");
	register_type(typeid(k3d::ri::imesh_painter*), "k3d::ri::imesh_painter*");
	register_type(typeid(k3d::ri::irender_engine), "k3d::ri::irender_engine");
	register_type(typeid(k3d::ri::irender_engine*), "k3d::ri::irender_engine*");
	register_type(typeid(k3d::ri::isurface_shader), "k3d::ri::isurface_shader");
	register_type(typeid(k3d::ri::itexture), "k3d::ri::itexture");
	register_type(typeid(k3d::ri::itexture*), "k3d::ri::itexture*");
	register_type(typeid(k3d::ri::ivolume_shader), "k3d::ri::ivolume_shader");
	register_type(typeid(k3d::string_t), "k3d::string_t");
	register_type(typeid(k3d::texture3), "k3d::texture3");
	register_type(typeid(k3d::uint16_t), "k3d::uint16_t");
	register_type(typeid(k3d::uint32_t), "k3d::uint32_t");
	register_type(typeid(k3d::uint64_t), "k3d::uint64_t");
	register_type(typeid(k3d::uint8_t), "k3d::uint8_t");
	register_type(typeid(k3d::vector2), "k3d::vector2");
	register_type(typeid(k3d::vector3), "k3d::vector3");
	register_type(typeid(k3d::yafray::ilight), "k3d::yafray::ilight");
	register_type(typeid(k3d::yafray::imaterial), "k3d::yafray::imaterial");

	/** \todo Come up with a more explicit type for these */
	register_type(typeid(k3d::typed_array<unsigned int>), "k3d::typed_array<unsigned int>");
	register_type(typeid(k3d::typed_array<k3d::double_t>), "k3d::typed_array<k3d::double_t>");
	register_type(typeid(std::vector<k3d::point3>), "std::vector<k3d::point3>");
	register_type(typeid(std::vector<unsigned int>), "std::vector<unsigned int>");
	register_type(typeid(std::vector<k3d::inode*>), "std::vector<k3d::inode*>");

	initialized = true;
}

const string_t demangle(const string_t& Type)
{
	string_t result = Type;

#ifdef K3D_HAVE_GCC_DEMANGLE

	int status = 0;
	char* const temp = ::abi::__cxa_demangle(Type.c_str(), 0, 0, &status);
	if(temp && (status == 0))
		result = temp;
	if(temp)
		::free(temp);

#endif // K3D_HAVE_GCC_DEMANGLE

	return result;
}

} // namespace detail

bool_t type_registered(const std::type_info& Info)
{
	detail::initialize_types();

	return detail::type_to_name_map.count(detail::type_info(Info));
}

const string_t type_string(const std::type_info& Info)
{
	detail::initialize_types();

	detail::type_to_name_map_t::iterator type = detail::type_to_name_map.find(detail::type_info(Info));
	if(type != detail::type_to_name_map.end())
		return type->second;

	log() << error << k3d_file_reference << ": unknown type: " << demangle(Info) << std::endl;
	return "";
}

const std::type_info* type_id(const string_t& Name)
{
	detail::initialize_types();

	detail::name_to_type_map_t::iterator type = detail::name_to_type_map.find(Name);
	if(type != detail::name_to_type_map.end())
		return &type->second.info;

	log() << error << k3d_file_reference << ": unknown type: " << Name << std::endl;
	return 0;
}

const std::type_info& type_id_k3d_bitmap_ptr()
{
    return typeid(k3d::bitmap*);
}

const string_t demangle(const std::type_info& Type)
{
	return detail::demangle(Type.name());
}

} // namespace k3d

