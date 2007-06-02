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
	\brief Implements GTSMeshReader, which reads GNU Triangulated Surface (.gts) files
	\author Tom Browder (tbrowder@home.com)
	\author Romain Behar (romainbehar@yahoo.com)
	\author Tim Shead (tshead@k3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/string_modifiers.h>

namespace libk3dgtsio
{

/// Retrieve a line from a GTS file, skipping comments and empty lines
void gts_line(std::istream& Stream, std::string& Buffer)
{
	for(k3d::getline(Stream, Buffer); Stream; k3d::getline(Stream, Buffer))
	{
		if(Buffer[0] == '#')
			continue;

		if(k3d::trim(Buffer).empty())
			continue;

		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// gts_mesh_reader

class gts_mesh_reader :
	public k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	gts_mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("gts_files"))
	{
		m_file.changed_signal().connect(make_topology_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::filesystem::path path = m_file.value();
		if(path.empty())
			return;
		
		k3d::filesystem::igzstream file(path);
		if(!file)
			return;

		k3d::log() << info << "Loading GNU Triangulated Surface file: " << path.native_console_string() << std::endl;

		// Read point, edge, and triangle counts ...
		std::string buffer;
		gts_line(file, buffer);
		if(!file)
		{
			k3d::log() << error << "Empty GTS file: " << path.native_console_string() << std::endl;
			return;
		}

		size_t point_count = 0;
		size_t edge_count = 0;
		size_t triangle_count = 0;
		std::stringstream line_buffer(buffer);
		line_buffer >> point_count >> edge_count >> triangle_count;

		// Read points ...
		boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t());
		boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t());

		points->reserve(point_count);
		point_selection->reserve(point_count);

		for(size_t i = 0; i != point_count; ++i)
		{
			gts_line(file, buffer);
			if(!file)
			{
				k3d::log() << error << "Unexpected end-of-file: " << path.native_console_string() << std::endl;
				return;
			}

			k3d::point3 point;
			std::stringstream line_buffer(buffer);
			line_buffer >> point[0] >> point[1] >> point[2];

			points->push_back(point);
			point_selection->push_back(0.0);
		}

		Mesh.points = points;
		Mesh.point_selection = point_selection;

		// Read edges ...
		std::vector<size_t> edge_from;
		std::vector<size_t> edge_to;

		edge_from.reserve(edge_count);
		edge_to.reserve(edge_count);

		for(size_t i = 0; i != edge_count; ++i)
		{
			gts_line(file, buffer);
			if(!file)
			{
				k3d::log() << error << "Unexpected end-of-file: " << path.native_console_string() << std::endl;
				return;
			}

			size_t from;
			size_t to;
			std::stringstream line_buffer(buffer);
			line_buffer >> from >> to;

			edge_from.push_back(from - 1); // GTS indices are one-based
			edge_to.push_back(to - 1); // GTS indices are one-based
		}

		// Read faces ...
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
		face_counts->push_back(triangle_count);
		types->push_back(k3d::mesh::polyhedra_t::POLYGONS);

		face_first_loops->reserve(triangle_count);
		face_loop_counts->reserve(triangle_count);
		face_selection->reserve(triangle_count);
		face_materials->reserve(triangle_count);
		loop_first_edges->reserve(triangle_count);
		edge_points->reserve(3 * triangle_count);
		clockwise_edges->reserve(3 * triangle_count);
		edge_selection->reserve(3 * triangle_count);

		for(size_t i = 0; i != triangle_count; ++i)
		{
			gts_line(file, buffer);
			if(!file)
			{
				k3d::log() << error << "Unexpected end-of-file: " << path.native_console_string() << std::endl;
				return;
			}

			size_t edge1;
			size_t edge2;
//			size_t edge3; This is totally redundant ... strange file format!

			std::stringstream line_buffer(buffer);
			line_buffer >> edge1 >> edge2 /* >> edge3 */;

			// Calculate a consistent order for triangle points ...
			const size_t edge1_from = edge_from[edge1 - 1];
			const size_t edge1_to = edge_to[edge1 - 1];
			const size_t edge2_from = edge_from[edge2 - 1];
			const size_t edge2_to = edge_to[edge2 - 1];

			// Determine triangle orientation
			size_t point1;
			size_t point2;
			size_t point3;

			if(edge1_from == edge2_from)
			{
				point1 = edge1_to;
				point2 = edge1_from;
				point3 = edge2_to;
			}
			else if(edge1_from == edge2_to)
			{
				point1 = edge1_to;
				point2 = edge1_from;
				point3 = edge2_from;
			}
			else if(edge1_to == edge2_from)
			{
				point1 = edge1_from;
				point2 = edge1_to;
				point3 = edge2_to;
			}
			else //if(edge1_to == edge2_to)
			{
				point1 = edge1_from;
				point2 = edge1_to;
				point3 = edge2_from;
			}

			face_first_loops->push_back(loop_first_edges->size());
			face_loop_counts->push_back(1);
			face_selection->push_back(0.0);
			face_materials->push_back(0);
			loop_first_edges->push_back(edge_points->size());
			
			edge_points->push_back(point1);
			clockwise_edges->push_back(edge_points->size());
			edge_selection->push_back(0.0);

			edge_points->push_back(point2);
			clockwise_edges->push_back(edge_points->size());
			edge_selection->push_back(0.0);

			edge_points->push_back(point3);
			clockwise_edges->push_back(edge_points->size() - 3);
			edge_selection->push_back(0.0);
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

		Mesh.polyhedra = polyhedra;
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<gts_mesh_reader, k3d::interface_list<k3d::imesh_source> > factory(
			k3d::uuid(0x7dbddd53, 0xef4e9588, 0x2643acb3, 0x4ad62813),
			"GTSMeshReader",
			_("Mesh reader that reads external GNU Triangulated Surface (.gts) files"),
			"MeshReader");

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& gts_mesh_reader_factory()
{
	return gts_mesh_reader::get_factory();
}

} // namespace libk3dgtsio

