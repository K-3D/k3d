#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/mesh_writer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace gmsh
{

namespace io
{

class mesh_writer :
	public k3d::mesh_writer<k3d::node>
{
	typedef k3d::mesh_writer<k3d::node> base;

public:
	mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_writer, k3d::interface_list<k3d::imesh_sink > > factory(
			k3d::uuid(0x01ce13c6, 0xca1e4d58, 0xbcc558a1, 0xf9780d73),
			"GmshGeoMeshWriter",
			_("Mesh writer that saves external Gmsh Geo geometry files"),
			"MeshWriter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_write_mesh(const k3d::mesh& Input, const k3d::filesystem::path& OutputPath, std::ostream& Output)
	{
    Output << "cl = 1e-2;\n";

		// Store points ...
		if(Input.points)
		{
			const k3d::mesh::points_t& points = *Input.points;
			const size_t point_begin = 0;
			const size_t point_end = point_begin + points.size();
			for(size_t point = point_begin; point != point_end; ++point) {
        Output << "Point(" << point+1 << ") = {" << points[point][0] << ", " << points[point][1] << ", " << points[point][2] << ", cl};\n";
			}
		}

		k3d::uint_t polyhedron_idx = 0;
    k3d::uint_t shell_offset = 0;
		k3d::int64_t gmsh_line_offset = 0;
		k3d::uint_t gmsh_loop_offset = 0;
		k3d::uint_t gmsh_face_offset = 0;
		k3d::uint_t gmsh_surface_loop_offset = 0;
		// Store polyhedra ...
		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input, **primitive));
			if(!polyhedron)
				continue;

      ++polyhedron_idx;

			const k3d::mesh::indices_t& face_first_loops = polyhedron->face_first_loops;
			const k3d::mesh::indices_t& loop_first_edges = polyhedron->loop_first_edges;
			const k3d::mesh::indices_t& clockwise_edges = polyhedron->clockwise_edges;
			const k3d::mesh::indices_t& vertex_points = polyhedron->vertex_points;

      k3d::mesh::bools_t boundary_edges;
      k3d::mesh::indices_t companions;
      k3d::polyhedron::create_edge_adjacency_lookup(vertex_points, clockwise_edges, boundary_edges, companions);

      const k3d::uint_t nb_edges = vertex_points.size();

      std::vector<k3d::int64_t> edge_to_gmsh_lines(nb_edges, 0);
      std::vector<k3d::uint_t> gmsh_line_starts; gmsh_line_starts.reserve(nb_edges);
      std::vector<k3d::uint_t> gmsh_line_ends; gmsh_line_ends.reserve(nb_edges);
      for(k3d::uint_t edge = 0; edge != nb_edges; ++edge)
      {
        if(edge_to_gmsh_lines[edge] != 0)
          continue;

        gmsh_line_starts.push_back(vertex_points[edge]+1);
        gmsh_line_ends.push_back(vertex_points[clockwise_edges[edge]]+1);
        k3d::int64_t line_idx = static_cast<k3d::int64_t>(gmsh_line_starts.size());
        edge_to_gmsh_lines[edge] = line_idx;
        if(!boundary_edges[edge])
          edge_to_gmsh_lines[companions[edge]] = -line_idx;
      }

      // Write the lines to file
      const k3d::uint_t nb_lines = gmsh_line_starts.size();
      for(k3d::uint_t line = 0; line != nb_lines; ++line)
      {
        Output << "Line(" << gmsh_line_offset+line+1 << ") = {" << gmsh_line_starts[line] << ", " << gmsh_line_ends[line] << "};\n";
      }


      std::vector< std::vector<k3d::uint_t> > gmsh_surface_loops(polyhedron->shell_types.size());

      const k3d::uint_t face_begin = 0;
      const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
			k3d::uint_t nb_loops = 0;
      for(k3d::uint_t face = face_begin; face != face_end; ++face)
      {
        std::stringstream face_str;
        face_str << "Plane Surface(" << gmsh_face_offset+face+1 << ") = {";
        const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
        const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
        for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
        {
					++nb_loops;
          face_str << gmsh_loop_offset+loop+1 << (loop == loop_end-1 ? "};\n" : ",");
          Output << "Line Loop(" << gmsh_loop_offset+loop+1 << ") = {";
          const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
          for(k3d::uint_t edge = first_edge; ; )
          {
						k3d::int64_t gmsh_line_idx = edge_to_gmsh_lines[edge];
            Output << gmsh_line_idx + (gmsh_line_idx > 0 ? gmsh_line_offset : -gmsh_line_offset);

            edge = polyhedron->clockwise_edges[edge];
            if(edge == first_edge)
            {
              Output << "};\n";
              break;
            }
            else
            {
              Output << ",";
            }
          }
        }
        Output << face_str.str();
        gmsh_surface_loops[polyhedron->face_shells[face]].push_back(face+1+gmsh_face_offset);
      }
      const k3d::uint_t nb_surface_loops = gmsh_surface_loops.size();
      for(k3d::uint_t loop = 0; loop != nb_surface_loops; ++loop)
      {
        const std::vector<k3d::uint_t>& loop_faces = gmsh_surface_loops[loop];
        if(loop_faces.empty())
          continue;
        Output << "Surface Loop(" << gmsh_surface_loop_offset+shell_offset+loop+1 << ") = {" << loop_faces.front();

        const k3d::uint_t loop_nb_faces = loop_faces.size();
        for(k3d::uint_t face = 1; face != loop_nb_faces; ++face)
          Output << ", " << loop_faces[face];
        Output << "};\n";
      }
      Output << "Volume(" << polyhedron_idx << ") = {" << gmsh_surface_loop_offset+shell_offset+1;
      for(k3d::uint_t loop = 1; loop != nb_surface_loops; ++loop)
      {
        Output << ", " << gmsh_surface_loop_offset+loop+shell_offset+1;
      }
      Output << "};\n";
			gmsh_line_offset += nb_lines;
			gmsh_face_offset += face_end;
			gmsh_loop_offset += nb_loops;
			gmsh_surface_loop_offset += nb_surface_loops;
		}
	}
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

} // namespace io

} // namespace gmsh

} // namespace module
