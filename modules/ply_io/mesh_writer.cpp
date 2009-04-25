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
*/

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/mesh_writer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/triangulator.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace ply
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_writer

class mesh_writer :
	public k3d::mesh_writer<k3d::node >
{
	typedef k3d::mesh_writer<k3d::node > base;

public:
	mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_writer, k3d::interface_list<k3d::imesh_sink > > factory(
			k3d::uuid(0x3b1829f4, 0xbc428bad, 0xa991e8b1, 0xcaaee7a5),
			"PLYMeshWriter",
			_("Mesh writer that saves external Stanford PLY (.ply) files"),
			"MeshWriter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	class triangulator :
		public k3d::triangulator
	{
		typedef k3d::triangulator base;

	public:
		triangulator(k3d::mesh::points_t& Points, k3d::mesh::indices_t& Vertices) :
			points(Points),
			vertices(Vertices)
		{
		}

		void process(const k3d::mesh& Mesh, const k3d::polyhedron::const_primitive& Polyhedron)
		{
			base::process(Mesh, Polyhedron);
		}

	private:
		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], double Weights[4], k3d::uint_t& NewVertex)
		{
			NewVertex = points.size();
			points.push_back(Coordinates);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			vertices.push_back(Vertices[0]);
			vertices.push_back(Vertices[1]);
			vertices.push_back(Vertices[2]);
		}

		k3d::mesh::points_t& points;
		k3d::mesh::indices_t& vertices;
	};

	void on_write_mesh(const k3d::mesh& Input, const k3d::filesystem::path& OutputPath, std::ostream& Output)
	{
		// Convert input polyhedra to triangles ...
		k3d::mesh::points_t points;
		k3d::mesh::indices_t vertices;

		if(Input.points)
			points = *Input.points;

		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
      boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
      if(polyhedron)
        triangulator(points, vertices).process(Input, *polyhedron);
    }

    // Write header data ...
    Output << "ply\n";
    Output << "format ascii 1.0\n";
    Output << "comment Written by K-3D " << K3D_VERSION << "\n";

    Output << "element vertex " << points.size() << "\n";
    Output << "property float x\n";
    Output << "property float y\n";
    Output << "property float z\n";

    Output << "element face " << vertices.size() / 3 << "\n";
    Output << "property list uchar int vertex_indices\n";
    
    Output << "end_header\n";

    // Write vertex data ...
    std::copy(points.begin(), points.end(), std::ostream_iterator<k3d::point3>(Output, "\n"));

    // Write face data ...
    const k3d::uint_t vertex_begin = 0;
    const k3d::uint_t vertex_end = vertex_begin + vertices.size();
    for(k3d::uint_t vertex = vertex_begin; vertex != vertex_end; ++vertex)
    {
      switch(vertex % 3)
      {
        case 0:
          Output << "3 " << vertices[vertex];
          break;
        case 1:
          Output << " " << vertices[vertex];
          break;
        case 2:
          Output << " " << vertices[vertex] << "\n";
          break;
      }
    }
  }
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

} // namespace io

} // namespace ply

} // namespace module

