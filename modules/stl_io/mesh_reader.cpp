// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/mesh_reader.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace stl
{

namespace io
{

namespace detail
{

/// Extracts the STL topology information, merging points that are less than threshold apart
void get_stl_topology(std::istream& Stream, k3d::mesh::points_t& Points, k3d::mesh::counts_t& VertexCounts, k3d::mesh::indices_t& VertexIndices, const k3d::double_t Threshold = 1e-12)
{
	const double_t threshold = Threshold*Threshold;
	
	k3d::string_t line_buffer;
	k3d::uint_t line_number = 0;
	k3d::uint_t face_number = 0;
	for(k3d::getline(Stream, line_buffer); Stream; k3d::getline(Stream, line_buffer))
	{
		k3d::string_t keyword;
		std::istringstream line_stream(line_buffer);
		line_stream >> keyword;
		
		if(keyword == "facet")
		{
			VertexCounts.push_back(0);
		}
		if(keyword == "vertex")
		{
			k3d::double_t x, y, z;
			line_stream >> x;
			line_stream >> y;
			line_stream >> z;
			k3d::point3 new_point(x, y, z);
			k3d::uint_t point_index = Points.size();
			for(k3d::uint_t point = 0; point != Points.size(); ++point)
			{
				if((Points[point] - new_point).length2() < threshold)
				{
					point_index = point;
					break;
				}
			}
			if(point_index == Points.size())
			{
				Points.push_back(new_point);
			}
			VertexIndices.push_back(point_index);
			++VertexCounts.back();
		}
		if(keyword == "endfacet")
		{
			if(VertexCounts.back() != 3)
			{
				std::stringstream error_stream("Error: STL file had less than 3 vertices for face ending on line ");
				error_stream << line_number;  
				throw std::runtime_error(error_stream.str());
			}
		}
	}
}

}

/////////////////////////////////////////////////////////////////////////////
// mesh_reader

class mesh_reader :
	public k3d::mesh_reader<k3d::node >
{
	typedef k3d::mesh_reader<k3d::node > base;

public:
	mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_threshold(init_owner(*this) + init_name("threshold") + init_label(_("Threshold")) + init_description(_("Controls the sensitivity for deciding when two edges are collinear.")) + init_value(1e-8) + init_step_increment(1e-8) + init_units(typeid(k3d::measurement::scalar)))
	{
	}

	void on_load_mesh(const k3d::filesystem::path& Path, k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::log() << info << "STLMeshReader: Loading .stl file: " << Path.native_console_string() << std::endl;
		k3d::filesystem::ifstream file(Path);
		if(!file)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << Path.native_console_string() << "]" << std::endl;
			return;
		}
		
		k3d::mesh::points_t points;
		k3d::mesh::counts_t vertex_counts;
		k3d::mesh::indices_t vertex_indices;
		
		try
		{
			detail::get_stl_topology(file, points, vertex_counts, vertex_indices, m_threshold.pipeline_value());
			k3d::polyhedron::create(Output, points, vertex_counts, vertex_indices, static_cast<k3d::imaterial*>(0));
		}
		catch(std::runtime_error& E)
		{
			k3d::log() << error << "STLMeshReader: " << E.what() << std::endl;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader,
                k3d::interface_list<k3d::imesh_source,
                k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0x6518a3a6, 0x8147c354, 0x82bbc381, 0x82077bf8),
			"STLMeshReader",
			_("Mesh reader that loads external Wavefront (.stl) files into the document by reference"),
			"MeshReader");

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_threshold;
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader::get_factory();
}

} // namespace io

} // namespace stl

} // namespace module

