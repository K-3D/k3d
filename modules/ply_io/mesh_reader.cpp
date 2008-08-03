// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>

namespace module
{

namespace ply
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader

class mesh_reader :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("ply_files"))
	{
		m_file.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		const k3d::filesystem::path path = m_file.pipeline_value();
		if(path.empty())
			return;
		
		k3d::filesystem::ifstream file(path);
		if(!file)
			return;

		k3d::log() << info << "Loading Stanford PLY file: " << path.native_console_string() << std::endl;
		
		std::string magic_number;
		k3d::getline(file, magic_number);
		if(magic_number != "ply")
		{
			k3d::log() << error << "Not a Stanford PLY file: " << path.native_console_string() << std::endl;
			return;
		}

		std::string format;
		k3d::getline(file, format);
		if(format != "format ascii 1.0")
		{
			k3d::log() << error << "Not an ascii format PLY file: " << path.native_console_string() << std::endl;
			return;
		}

		std::vector<std::string> element_types;
		std::vector<size_t> element_counts;
	
		while(true)
		{
			std::stringstream line_buffer;
			k3d::getline(file, line_buffer);
			if(!file)
				break;

			std::string keyword;
			line_buffer >> keyword;

			if(keyword == "comment")
			{
			}
			else if(keyword == "element")
			{
				std::string element_type;
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

		for(size_t i = 0; i != element_types.size(); ++i)
		{
			boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t());
			boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t());

			const std::string element_type = element_types[i];
			const size_t element_count = element_counts[i];
			
			k3d::log() << info << "Reading " << element_count << " elements of type: " << element_type << std::endl;

			if(element_type == "vertex")
			{
				points->reserve(element_count);
				point_selection->reserve(element_count);

				for(size_t j = 0; j != element_count; ++j)
				{
					std::stringstream line_buffer;
					k3d::getline(file, line_buffer);
					if(!file)
					{
						k3d::log() << error << "Unexpected end-of-file: " << path.native_console_string() << std::endl;
						return;
					}

					k3d::point3 point;
					line_buffer >> point[0] >> point[1] >> point[2];
					
					points->push_back(point);
					point_selection->push_back(0.0);
				}

				Output.points = points;
				Output.point_selection = point_selection;
			}
			else if(element_type == "face")
			{
				boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra(new k3d::mesh::polyhedra_t());
				boost::shared_ptr<k3d::mesh::indices_t> first_faces(new k3d::mesh::indices_t());
				boost::shared_ptr<k3d::mesh::counts_t> face_counts(new k3d::mesh::counts_t());
				boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types(new k3d::mesh::polyhedra_t::types_t());
				boost::shared_ptr<k3d::mesh::indices_t> face_first_loops(new k3d::mesh::indices_t());
				boost::shared_ptr<k3d::mesh::counts_t> face_loop_counts(new k3d::mesh::counts_t());
				boost::shared_ptr<k3d::mesh::selection_t> face_selection(new k3d::mesh::selection_t());
				boost::shared_ptr<k3d::mesh::materials_t> face_materials(new k3d::mesh::materials_t());
				boost::shared_ptr<k3d::mesh::indices_t> loop_first_edges(new k3d::mesh::indices_t());
				boost::shared_ptr<k3d::mesh::indices_t> edge_points(new k3d::mesh::indices_t());
				boost::shared_ptr<k3d::mesh::indices_t> clockwise_edges(new k3d::mesh::indices_t());
				boost::shared_ptr<k3d::mesh::selection_t> edge_selection(new k3d::mesh::selection_t());
		
				first_faces->push_back(face_first_loops->size());
				face_counts->push_back(element_count);
				types->push_back(k3d::mesh::polyhedra_t::POLYGONS);

				face_first_loops->reserve(element_count);
				face_loop_counts->reserve(element_count);
				face_selection->reserve(element_count);
				face_materials->reserve(element_count);
				loop_first_edges->reserve(element_count);
				edge_points->reserve(3 * element_count);
				clockwise_edges->reserve(3 * element_count);
				edge_selection->reserve(3 * element_count);
				
				for(size_t j = 0; j != element_count; ++j)
				{
					std::stringstream line_buffer;
					k3d::getline(file, line_buffer);
					if(!file)
					{
						k3d::log() << error << "Unexpected end-of-file: " << path.native_console_string() << std::endl;
						return;
					}

					size_t point_count = 0;
					line_buffer >> point_count;

					face_first_loops->push_back(loop_first_edges->size());
					face_loop_counts->push_back(1);
					face_selection->push_back(0.0);
					face_materials->push_back(0);
					loop_first_edges->push_back(edge_points->size());
					
					for(size_t k = 0; k != point_count; ++k)
					{
						size_t point = 0;
						line_buffer >> point;

						edge_points->push_back(point);
						clockwise_edges->push_back(edge_points->size());
						edge_selection->push_back(0.0);
					}

					if(loop_first_edges->size() && clockwise_edges->size())
						clockwise_edges->back() = loop_first_edges->back();
				}

				polyhedra->first_faces = first_faces;
				polyhedra->face_counts = face_counts;
				polyhedra->types = types;
				polyhedra->face_first_loops = face_first_loops;
				polyhedra->face_loop_counts = face_loop_counts;
				polyhedra->face_selection = face_selection;
				polyhedra->face_materials = face_materials;
				polyhedra->loop_first_edges = loop_first_edges;
				polyhedra->edge_points = edge_points;
				polyhedra->clockwise_edges = clockwise_edges;
				polyhedra->edge_selection = edge_selection;

				Output.polyhedra = polyhedra;
			}
		}

		// Sanity check: make sure we have points if we have polyhedra ...
		if(!Output.points)
			Output.polyhedra.reset();
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
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

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader::get_factory();
}

} // namespace io

} // namespace ply

} // namespace module

