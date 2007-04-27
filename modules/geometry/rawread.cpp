// K-3D
// Copyright (c) 2004-2006, Romain Behar
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
		\brief Implements a RAW file importer (.raw polygonal meshes)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_read_format.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/material.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/utility.h>

#include "helpers.h"

#include <k3dsdk/fstream.h>

namespace libk3dgeometry
{

bool raw_line(std::istream& file, std::string& linebuffer)
{
	// Strip comments (lines beginning with '#') and empty lines,
	// return true if buffer has been filled, return false on end of file
	while(!file.eof())
	{
		k3d::getline(file, linebuffer);

		// Skip comments ...
		if(linebuffer[0] == '#')
			continue;

		// Skip blank lines
		if(!(k3d::trim(linebuffer)).size())
			continue;

		// Must qualify
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// raw_reader_implementation

class raw_reader_implementation :
	public k3d::ifile_format,
	public k3d::idocument_read_format,
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

	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath);

	static k3d::iplugin_factory& get_factory()
	{
		static application_plugin_factory<raw_reader_implementation, k3d::interface_list<k3d::idocument_read_format> > factory(
			k3d::uuid(0x49ca924e, 0x159023b3, 0x492fb90d, 0x9afb0555),
			"RAWReader",
			_("Raw ( .raw )"),
			"GeometryReader");

		return factory;
	}
};

bool raw_reader_implementation::read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
{
	k3d::log() << info << "Reading " << FilePath.native_console_string() << " with " << factory().name() << std::endl;

	// Try to open the input file ...
	k3d::filesystem::ifstream file(FilePath);
	if(!file.good())
	{
		k3d::log() << error << k3d_file_reference << ": error opening [" << FilePath.native_console_string() << "]" << std::endl;
		return false;
	}

	// Empty file check ...
	std::string linebuffer;
	if(!raw_line(file, linebuffer))
	{
		k3d::log() << error << k3d_file_reference << ": empty file [" << FilePath.native_console_string() << "]" << std::endl;
		return false;
	}

	// Extract point, polygon numbers
	std::istringstream stream(linebuffer);
	unsigned int npoints = 0;
	unsigned int npols = 0;
	stream >> npoints >> npols;

	if(!npoints && !npols)
	{
		k3d::log() << error << k3d_file_reference << ": malformed RAW file [" << FilePath.native_console_string() << "]" << std::endl;
		return false;
	}

	// File format for raw polygonal surfaces :
	// - all the lines beginning with '#' are ignored as well as empty lines
	// - all parameters are space-separated
	// - the first line contains two unsigned integers separated by spaces :
	//   the number of vertices (npoints) and the number of polygons (npols)
	// - then follow :
	//  * npoints lines each containing the x, y and z coordinates of the vertices
	//  * npols lines each containing the ordered indices of the points of a polygon
	// - points are 0-indexed
	// - a file can contain several raw objects

	// Give objects unique names as we build them ...
	unsigned long object_number = 0;

	k3d::imaterial* const material = dynamic_cast<k3d::imaterial*>(k3d::default_material(Document));

	// Read the stream ...
	k3d::inode* frozen_mesh;
	k3d::inode* mesh_instance;
	while(npoints && npols)
	{
		// Create a new mesh ...
		k3d::legacy::mesh* const mesh = detail::create_mesh(Document, "RAW Object " + k3d::string_cast(++object_number), frozen_mesh, mesh_instance);
		return_val_if_fail(mesh, false);

		// Create a polyhedron ...
		k3d::legacy::polyhedron* const polyhedron = new k3d::legacy::polyhedron();
		return_val_if_fail(polyhedron, false);
		mesh->polyhedra.push_back(polyhedron);

		// Read points
		std::vector<k3d::legacy::point*> points;
		for(unsigned int n = 0; n < npoints; n++)
		{
			// Get next data line
			if(!raw_line(file, linebuffer))
			{
				k3d::log() << error << k3d_file_reference << ": unexpected end of input file (file \"" << FilePath.native_console_string() << "\", point #" << n << "." << std::endl;
				return false;
			}

			// Extract a record type ...
			std::istringstream stream(linebuffer);

			k3d::point3 coords;
			stream >> coords;

			k3d::legacy::point* const point = new k3d::legacy::point(coords);
			return_val_if_fail(point, false);

			points.push_back(point);
			mesh->points.push_back(point);
		}

		// Read polygons
		for(unsigned int n = 0; n < npols; n++)
		{
			// Get next data line
			if(!raw_line(file, linebuffer))
			{
				k3d::log() << error << k3d_file_reference << ": Unexpected end of input file (file \"" << FilePath.native_console_string() << "\", triangle #" << n << "." << std::endl;
				return false;
			}

			// Create a face ...
			k3d::legacy::split_edge* previous_edge = 0;
			k3d::legacy::face* face = 0;

			// Extract a polygon ...
			std::istringstream stream(linebuffer);
			unsigned long geometric_index = 0;
			do
			{
				stream >> geometric_index;

				k3d::legacy::point* const point = points[geometric_index];
				return_val_if_fail(point, false);

				k3d::legacy::split_edge* edge = new k3d::legacy::split_edge(point);
				return_val_if_fail(edge, false);

				if(!face)
				{
					face = new k3d::legacy::face(edge, material);
					return_val_if_fail(face, false);
					polyhedron->faces.push_back(face);
				}
				else
				{
					previous_edge->face_clockwise = edge;
				}

				previous_edge = edge;
			}
			while(!stream.eof());

			// Close loop
			if(face)
			{
				previous_edge->face_clockwise = face->first_edge;
			}
		}

		// Set companions
		k3d::legacy::set_companions(*polyhedron);

		// Go on for another one ?
		npoints = npols = 0;
		if(raw_line(file, linebuffer))
		{
			std::istringstream stream(linebuffer);
			stream >> npoints >> npols;
		}
	}

	return true;
}

k3d::iplugin_factory& raw_reader_factory()
{
	return raw_reader_implementation::get_factory();
}

} // namespace libk3dgeometry

