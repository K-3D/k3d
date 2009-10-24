#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/mesh_writer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace ogre
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
			k3d::uuid(0x1f8118fa, 0x4341982b, 0x642d068f, 0x0d3c70e4),
			"OgreXmlMeshWriter",
			_("Mesh writer that saves external Ogre XML Mesh files"),
			"MeshWriter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_write_mesh(const k3d::mesh& Input, const k3d::filesystem::path& OutputPath, std::ostream& Output)
	{
		std::stringstream sstream;
	
		k3d::xml::element top("mesh");
		
		// Store points ...
		if(Input.points)
		{
			const k3d::mesh::points_t& points = *Input.points;
			
			k3d::xml::element shared_geom("sharedgeometry");
			sstream << points.size();
			shared_geom.append(k3d::xml::attribute("vertexcount", sstream.str()));
			sstream.str("");
			
			k3d::xml::element vertexbuffer("vertexbuffer");
			vertexbuffer.append(k3d::xml::attribute("positions", "true"));
			
			const size_t point_begin = 0;
			const size_t point_end = point_begin + points.size();
			for(size_t point = point_begin; point != point_end; ++point) {
				k3d::xml::element vertex("vertex");
				k3d::xml::element position("position");
				
				sstream << points[point][0];
				position.append(k3d::xml::attribute("x", sstream.str()));
				sstream.str("");
				
				sstream << points[point][1];
				position.append(k3d::xml::attribute("y", sstream.str()));
				sstream.str("");
				
				sstream << points[point][2];
				position.append(k3d::xml::attribute("z", sstream.str()));
				sstream.str("");
				
				vertex.append(position);
				vertexbuffer.append(vertex);
			}
			
			
			shared_geom.append(vertexbuffer);
			top.append(shared_geom);
		}
		
		// Store polyhedra ...
		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input, **primitive));
			if(!polyhedron)
				continue;

			if(!k3d::polyhedron::is_triangles(*polyhedron))
			{
				k3d::log() << warning << factory().name() << " cannot export polyhedron containing non-triangles." << std::endl;
				continue;
			}

			const k3d::mesh::indices_t& face_first_loops = polyhedron->face_first_loops;
			const k3d::mesh::indices_t& loop_first_edges = polyhedron->loop_first_edges;
			const k3d::mesh::indices_t& clockwise_edges = polyhedron->clockwise_edges;
			const k3d::mesh::indices_t& vertex_points = polyhedron->vertex_points;

			k3d::xml::element submeshes("submeshes");
			k3d::xml::element submesh("submesh");
			k3d::xml::element faces("faces");
			
			sstream << face_first_loops.size();
			faces.append(k3d::xml::attribute("count", sstream.str()));
			sstream.str("");
			
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + face_first_loops.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				k3d::uint_t first_edge = loop_first_edges[face_first_loops[face]];
				k3d::uint_t edge = first_edge;
				k3d::xml::element face_xml("face");
				for(k3d::uint_t i = 1; i < 4; i++) {
					sstream << 'v' << i;
					std::string name(sstream.str());
					sstream.str("");
					sstream << vertex_points[edge];
					std::string value(sstream.str());
					sstream.str("");
					face_xml.append(k3d::xml::attribute(name, value));
					edge = clockwise_edges[edge];
				}
				faces.append(face_xml);
			}
			
			submesh.append(faces);
			submeshes.append(submesh);
			top.append(submeshes);
			
		}
		
		Output << k3d::xml::declaration() <<  top;
	}
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

} // namespace io

} // namespace ogre

} // namespace module

