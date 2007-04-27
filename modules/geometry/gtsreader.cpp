// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Implements GTSReader, which reads GNU Triangulated Surface (.gts) files
		\author Tom Browder (tbrowder@home.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/idag.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/idocument_read_format.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/material.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/utility.h>

#include "helpers.h"

#include <k3dsdk/fstream.h>
#include <boost/timer.hpp>

namespace libk3dgeometry
{

// Get a line from GTS file :
// - strip comments ('#' to end-of-line)
// - skip empty lines
bool gts_line(std::istream& file, std::string& linebuffer)
{
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

	// end-of-file reached
	return false;
}

// All GTS indices are 1-indexed
unsigned long k3d_index(const unsigned long& external_index)
{
	return external_index - 1;
}

// Define an edge type
typedef std::pair<unsigned long, unsigned long> edge_t;

/////////////////////////////////////////////////////////////////////////////
// gts_reader_implementation

class gts_reader_implementation :
	public k3d::ifile_format,
	public k3d::idocument_read_format,
	public k3d::ideletable
{
public:
	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& Path)
	{
		return k3d::filesystem::extension(Path).lowercase().raw() == ".gts";
	}

	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath);

	static k3d::iplugin_factory& get_factory()
	{
		static application_plugin_factory<gts_reader_implementation, k3d::interface_list<k3d::idocument_read_format> > factory(
			k3d::uuid(0xff12523c, 0x01324d97, 0xa3dab844, 0xeaacce90),
			"GTSReader",
			_("GNU Triangulated Surface ( .gts )"),
			"GeometryReader");

		return factory;
	}
};

bool gts_reader_implementation::read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
{
	k3d::log() << info << "Reading " << FilePath.native_console_string() << " with " << factory().name() << std::endl;

	// Try to open the input file ...
	k3d::filesystem::ifstream file(FilePath);
	if(!file.good())
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << FilePath.native_console_string() << "]" << std::endl;
			return false;
		}

	// Return if empty file ...
	std::string linebuffer;
	if(!gts_line(file, linebuffer))
		return true;

	// First object line: extract point, edge and triangle numbers
	std::istringstream stream(linebuffer);
	unsigned long np = 0;
	unsigned long ne = 0;
	unsigned long nt = 0;
	stream >> np >> ne >> nt;

	// Give objects unique names as we load them ...
	unsigned long object_number = 0;

	// Always use the default material
	k3d::imaterial* const default_material = dynamic_cast<k3d::imaterial*>(k3d::default_material(Document));

	// Importer handles multiple GTS objects in one file (not part of GTS format)
	k3d::inode* frozen_mesh;
	k3d::inode* mesh_instance;
	while(np && ne && nt)
		{
			// Create a new mesh ...
			k3d::legacy::mesh* const mesh = detail::create_mesh(Document, "GTS Object " + k3d::string_cast(++object_number), frozen_mesh, mesh_instance);
			return_val_if_fail(mesh, false);

			k3d::legacy::polyhedron* const polyhedron = new k3d::legacy::polyhedron();
			return_val_if_fail(polyhedron, false);
			mesh->polyhedra.push_back(polyhedron);

			// Setup storage for vertices and edges
			std::vector<k3d::legacy::point*> points;
			std::vector<edge_t> geometric_edges;

			// Import points ...
			unsigned long n;
			for(n = 0; n < np; n++)
				{
					// Get next data line
					if(!gts_line(file, linebuffer))
						{
							k3d::log() << error << "Unexpected end of input file \"" << FilePath.native_console_string() << "\", at point #" << n << "." << std::endl;
							return false;
						}

					// Extract a record type ...
					std::istringstream stream(linebuffer);

					k3d::point3 coords;
					stream >> coords;

					// Transform from GTS to K-3D rigth-handed system
					double x = coords[0];
					double y = coords[1];
					double z = coords[2];

					k3d::legacy::point* const point = new k3d::legacy::point(-x, z, -y);
					return_val_if_fail(point, false);

					points.push_back(point);
					mesh->points.push_back(point);
				}

			// Read edges
			for(n = 0; n < ne; n++)
				{
					if(!gts_line(file, linebuffer))
						{
							k3d::log() << error << "Unexpected end of input file \"" << FilePath.native_console_string() << "\", at edge #" << n << "." << std::endl;
							return false;
						}

					// Extract a record type ...
					std::istringstream stream(linebuffer);

					unsigned long edge_start;
					unsigned long edge_end;
					stream >> edge_start >> edge_end;

					// Transform indices from 1-base to 0-base
					geometric_edges.push_back(edge_t(k3d_index(edge_start), k3d_index(edge_end)));
				}

			// Import triangles
			for(n = 0; n < nt; n++)
				{
					if(!gts_line(file, linebuffer))
						{
							k3d::log() << error << "Unexpected end of input file \"" << FilePath.native_console_string() << "\", at triangle #" << n << "." << std::endl;
							return false;
						}

					// Extract a record type ...
					std::istringstream stream(linebuffer);

					// Get the 3 edges indices
					unsigned long edge0, edge1, edge2;
					stream >> edge0 >> edge1 >> edge2;

					// Transform indices from 1-based to 0-based
					edge_t e0 = geometric_edges[k3d_index(edge0)];
					edge_t e1 = geometric_edges[k3d_index(edge1)];

					unsigned long e0_lo = e0.first;
					unsigned long e0_hi = e0.second;
					unsigned long e1_lo = e1.first;
					unsigned long e1_hi = e1.second;

					// Determine triangle orientation
					unsigned long p0, p1, p2;
					if(e0_lo == e1_lo)
						{
							p0 = e0_hi;
							p1 = e0_lo;
							p2 = e1_hi;
						}
					else if(e0_lo == e1_hi)
						{
							p0 = e0_hi;
							p1 = e0_lo;
							p2 = e1_lo;
						}
					else if(e0_hi == e1_lo)
						{
							p0 = e0_lo;
							p1 = e0_hi;
							p2 = e1_hi;
						}
					else //if(e0_hi == e1_hi)
						{
							p0 = e0_lo;
							p1 = e0_hi;
							p2 = e1_lo;
						}

					// Create a triangle ...
					std::vector<k3d::legacy::split_edge*> edges;
					edges.push_back(new k3d::legacy::split_edge(points[p0]));
					edges.push_back(new k3d::legacy::split_edge(points[p1]));
					edges.push_back(new k3d::legacy::split_edge(points[p2]));

					k3d::legacy::loop_edges(edges.begin(), edges.end());

					k3d::legacy::face* const face = new k3d::legacy::face(*edges.begin(), default_material);
					return_val_if_fail(face, false);

					polyhedron->faces.push_back(face);
				}

			// Set companions
			k3d::legacy::set_companions(*polyhedron);

			// Is there another object to load?
			np = ne = nt = 0;
			if(gts_line(file, linebuffer))
				{
					std::istringstream stream(linebuffer);
					stream >> np >> ne >> nt;
				}
		}

	return true;
}

k3d::iplugin_factory& gts_reader_factory()
{
	return gts_reader_implementation::get_factory();
}

} // namespace libk3dgeometry

