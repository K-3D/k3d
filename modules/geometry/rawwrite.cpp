// K-3D
// Copyright (c) 2003-2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Implements the RAWWriter which exports polygons as raw ASCII files
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "helpers.h"

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/idocument_write_format.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/utility.h>

#include <k3dsdk/fstream.h>

#include <map>

namespace libk3dgeometry
{

typedef std::vector<unsigned long> polygon_t;
typedef std::vector<polygon_t> polygons_t;

void write_raw_mesh(k3d::legacy::mesh& Mesh, k3d::inode& Object, std::ostream& Stream)
{
	// Get the collection of points ...
	k3d::legacy::mesh::points_t points;
	std::copy(Mesh.points.begin(), Mesh.points.end(), std::back_inserter(points));

	// Create a mapping of points-to-zero-based-indices ...
	std::map<k3d::legacy::point*, unsigned long> point_map;
	unsigned long index = 0;
	for(k3d::legacy::mesh::points_t::iterator point = points.begin(); point != points.end(); ++point)
		point_map[*point] = index++;

	// Build polygon collection ...
	polygons_t polygons;
	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); polyhedron++)
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); face++)
		{
			polygon_t polygon;

			k3d::legacy::split_edge* first = (*face)->first_edge;
			// Skip empty faces
			if(!first)
				continue;

			polygon.push_back(point_map[first->vertex]);

			k3d::legacy::split_edge* current_edge = first->face_clockwise;
			while(current_edge)
			{
				polygon.push_back(point_map[current_edge->vertex]);

				current_edge = current_edge->face_clockwise;
				if(current_edge == first)
					break;
			}

			polygons.push_back(polygon);
		}

	// Write object info and header
	Stream << "# object: " << Object.name() << std::endl;
	Stream << "# num points [" << points.size() << "] num polygons [" << polygons.size() << "]" << std::endl;
	Stream << points.size() << " " << polygons.size() << std::endl;

	// Write points
	Stream << "# points" << std::endl;
	for(k3d::legacy::mesh::points_t::iterator point = points.begin(); point != points.end(); ++point)
		{
			const k3d::point3 coords = (*point)->position;
			Stream << coords[0] << " " << coords[1] << " " << coords[2] << std::endl;
		}

	// Write polygons
	Stream << "# polygons" << std::endl;
	for(polygons_t::iterator polygon = polygons.begin(); polygon != polygons.end(); ++polygon)
		{
			for(polygon_t::iterator point = polygon->begin(); point != polygon->end(); ++point)
				Stream << *point << " ";
			Stream << std::endl;
		}
}

/////////////////////////////////////////////////////////////////////////////
// raw_writer_implementation

class raw_writer_implementation :
	public k3d::ifile_format,
	public k3d::idocument_write_format,
	public k3d::ideletable
{
public:
	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& FilePath)
	{
		return k3d::filesystem::extension(FilePath).lowercase().raw() == ".raw";
	}

	bool write_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath);

	static k3d::iplugin_factory& get_factory()
	{
		static application_plugin_factory<raw_writer_implementation, k3d::interface_list<k3d::idocument_write_format> > factory(
			k3d::uuid(0xd3bf8f81, 0x44934ebc, 0x80e9c088, 0xe5c0c4fc),
			"RAWWriter",
			_("raw ASCII format ( .raw )"),
			"GeometryWriter");

		return factory;
	}
};

bool raw_writer_implementation::write_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
{
	k3d::log() << info << "Writing " << FilePath.native_console_string() << " with " << factory().name() << std::endl;

	// Try to open the output file ...
	k3d::filesystem::ofstream file(FilePath);
	return_val_if_fail(file.good(), false);
	file << "# Written by K-3D" << std::endl;

	// Get the set of available meshes
	detail::mesh_instances_t meshes;
	detail::get_mesh_instances(Document, meshes);

	// Write meshes to the file
	for(detail::mesh_instances_t::iterator mesh = meshes.begin(); mesh != meshes.end(); ++mesh)
		write_raw_mesh(*mesh->first, *mesh->second, file);

	return true;
}

k3d::iplugin_factory& raw_writer_factory()
{
	return raw_writer_implementation::get_factory();
}

} // namespace libk3dgeometry

