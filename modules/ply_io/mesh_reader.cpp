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

#include <k3d-i18n-config.h>
#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_reader.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace ply
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader

class mesh_reader :
	public k3d::material_sink<k3d::mesh_reader<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_reader<k3d::node > > base;

public:
	mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_load_mesh(const k3d::filesystem::path& Path, k3d::mesh& Output)
	{
		k3d::filesystem::ifstream file(Path);
		if(!file)
			return;

		k3d::string_t magic_number;
		k3d::getline(file, magic_number);
		if(magic_number != "ply")
		{
			k3d::log() << error << "Not a Stanford PLY file: " << Path.native_console_string() << std::endl;
			return;
		}

		k3d::string_t format;
		k3d::getline(file, format);
		if(format != "format ascii 1.0")
		{
			k3d::log() << error << "Not an ascii format PLY file: " << Path.native_console_string() << std::endl;
			return;
		}

		std::vector<k3d::string_t> element_types;
		std::vector<k3d::uint_t> element_counts;
	
		while(true)
		{
			std::stringstream line_buffer;
			k3d::getline(file, line_buffer);
			if(!file)
				break;

			k3d::string_t keyword;
			line_buffer >> keyword;

			if(keyword == "comment")
			{
			}
			else if(keyword == "element")
			{
				k3d::string_t element_type;
				size_t element_count = 0;
				line_buffer >> element_type >> element_count;

				element_types.push_back(element_type);
				element_counts.push_back(element_count);
			}
			else if(keyword == "property")
			{
			}
			else if(keyword == "end_header")
			{
				break;
			}
		}

		for(k3d::uint_t i = 0; i != element_types.size(); ++i)
		{
			const k3d::string_t element_type = element_types[i];
			const k3d::uint_t element_count = element_counts[i];
			
			k3d::log() << info << "Reading " << element_count << " elements of type: " << element_type << std::endl;

			if(element_type == "vertex")
			{
				k3d::mesh::points_t& points = Output.points.create(new k3d::mesh::points_t(element_count));
				k3d::mesh::selection_t& point_selection = Output.point_selection.create(new k3d::mesh::selection_t(element_count, 0.0));

				for(k3d::uint_t j = 0; j != element_count; ++j)
				{
					std::stringstream line_buffer;
					k3d::getline(file, line_buffer);
					if(!file)
					{
						k3d::log() << error << "Unexpected end-of-file: " << Path.native_console_string() << std::endl;
						return;
					}

					k3d::point3 point;
					line_buffer >> point[0] >> point[1] >> point[2];
					
					points[j] = point;
				}
			}
			else if(element_type == "face")
			{
				boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));
		
				polyhedron->shell_first_faces.push_back(polyhedron->face_first_loops.size());
				polyhedron->shell_face_counts.push_back(element_count);
				polyhedron->shell_types.push_back(k3d::mesh::polyhedra_t::POLYGONS);

				polyhedron->face_first_loops.reserve(element_count);
				polyhedron->face_loop_counts.reserve(element_count);
				polyhedron->face_selections.reserve(element_count);
				polyhedron->face_materials.reserve(element_count);
				polyhedron->loop_first_edges.reserve(element_count);
				polyhedron->edge_points.reserve(3 * element_count);
				polyhedron->clockwise_edges.reserve(3 * element_count);
				polyhedron->edge_selections.reserve(3 * element_count);
				
				for(k3d::uint_t j = 0; j != element_count; ++j)
				{
					std::stringstream line_buffer;
					k3d::getline(file, line_buffer);
					if(!file)
					{
						k3d::log() << error << "Unexpected end-of-file: " << Path.native_console_string() << std::endl;
						return;
					}

					k3d::uint_t point_count = 0;
					line_buffer >> point_count;

					polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
					polyhedron->face_loop_counts.push_back(1);
					polyhedron->face_selections.push_back(0.0);
					polyhedron->face_materials.push_back(0);
					polyhedron->loop_first_edges.push_back(polyhedron->edge_points.size());
					
					for(k3d::uint_t k = 0; k != point_count; ++k)
					{
						k3d::uint_t point = 0;
						line_buffer >> point;

						polyhedron->edge_points.push_back(point);
						polyhedron->clockwise_edges.push_back(polyhedron->edge_points.size());
						polyhedron->edge_selections.push_back(0.0);
					}

					if(polyhedron->loop_first_edges.size() && polyhedron->clockwise_edges.size())
						polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x6b0ac805, 0xc3094636, 0x972b80aa, 0x61437211),
			"PLYMeshReader",
			_("Mesh source that reads external Stanford PLY (.ply) files"),
			"MeshReader");

		return factory;
	}
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader::get_factory();
}

} // namespace io

} // namespace ply

} // namespace module

