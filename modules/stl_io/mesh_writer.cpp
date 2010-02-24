// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/ihint.h>
#include <k3dsdk/mesh_writer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/triangulator.h>

#include <boost/scoped_ptr.hpp>

#include <set>

namespace module
{

namespace stl
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_writerio

class mesh_writer :
	public k3d::mesh_writer<k3d::node >
{
	typedef k3d::mesh_writer<k3d::node > base;

public:
	mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_group_solids(init_owner(*this) + init_name("group_solids") + init_label(_("Group Solids")) + init_description(_("Group solids using a per-face array.")) + init_value(false)),
		m_group_array(init_owner(*this) + init_name("group_array") + init_label(_("Group Array")) + init_description(_("Name of a per-face array containing solid labels (as used in OpenFOAM).")) + init_value(std::string("solids")))
	{
		m_group_solids.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_write_file_slot()));
		m_group_array.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_write_file_slot()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_writer, k3d::interface_list<k3d::imesh_sink > > factory(
			k3d::uuid(0x1de22bd7, 0xee49eca0, 0x827240b3, 0xaf18e521),
			"STLMeshWriter",
			_("Mesh writer that saves external Stereo Triangulation Lithograph (.stl) files"),
			"MeshWriter",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	class triangulator :
		public k3d::triangulator
	{
		typedef k3d::triangulator base;

	public:
		triangulator(const k3d::mesh::strings_t* const FaceGroups, k3d::mesh::points_t& TrianglePoints, k3d::mesh::indices_t& TriangleIndices, k3d::mesh::strings_t& TriangleGroups) :
			face_groups(FaceGroups),
			triangle_points(TrianglePoints),
			triangle_indices(TriangleIndices),
			triangle_groups(TriangleGroups)
		{
		}

	private:
		void start_face(const k3d::uint_t Face)
		{
			current_face = Face;
		}

		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], double Weights[4], k3d::uint_t& NewVertex)
		{
			NewVertex = triangle_points.size();
			triangle_points.push_back(Coordinates);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			triangle_indices.push_back(Vertices[0]);
			triangle_indices.push_back(Vertices[1]);
			triangle_indices.push_back(Vertices[2]);

			triangle_groups.push_back(face_groups ? (*face_groups)[current_face] : "default");
		}

		const k3d::mesh::strings_t* const face_groups;
		k3d::mesh::points_t& triangle_points;
		k3d::mesh::indices_t& triangle_indices;
		k3d::mesh::strings_t& triangle_groups;
		k3d::uint_t current_face;
	};

	void on_write_mesh(const k3d::mesh& Input, const k3d::filesystem::path& OutputPath, std::ostream& Output)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input, **primitive));
			if(!polyhedron)
				continue;

			// Setup our inputs and outputs ...
			const k3d::mesh::strings_t* const face_groups = m_group_solids.pipeline_value() ? polyhedron->face_attributes.lookup<k3d::mesh::strings_t>(m_group_array.pipeline_value()) : 0;

			k3d::mesh::points_t triangle_points = *Input.points;
			k3d::mesh::indices_t triangle_indices;
			k3d::mesh::strings_t triangle_groups;

			// Triangulate the polyhedron ...
			triangulator(face_groups, triangle_points, triangle_indices, triangle_groups).process(Input, *polyhedron);

			// Get the set of unique face group names ...
			std::set<k3d::string_t> triangle_group_names(triangle_groups.begin(), triangle_groups.end());

			// For each group of triangles, write-out an STL solid ...
			const k3d::uint_t triangle_begin = 0;
			const k3d::uint_t triangle_end = triangle_begin + triangle_groups.size();
			for(std::set<k3d::string_t>::const_iterator triangle_group_name = triangle_group_names.begin(); triangle_group_name != triangle_group_names.end(); ++triangle_group_name)
			{
				Output << "solid " << *triangle_group_name << "\n";

				for(k3d::uint_t triangle = triangle_begin; triangle != triangle_end; ++triangle)
				{
					if(triangle_groups[triangle] != *triangle_group_name)
						continue;

					const k3d::point3 a = triangle_points[triangle_indices[(3 * triangle) + 0]];
					const k3d::point3 b = triangle_points[triangle_indices[(3 * triangle) + 1]];
					const k3d::point3 c = triangle_points[triangle_indices[(3 * triangle) + 2]];

					Output << "facet normal " << k3d::polyhedron::normal(a, b, c) << "\n";
					Output << "  outer loop\n";
					Output << "    vertex " << a << "\n";
					Output << "    vertex " << c << "\n"; // Note: STL stores faces with right-hand ordering!
					Output << "    vertex " << b << "\n";
					Output << "  endloop\n";
					Output << "endfacet\n";
				}
				Output << "endsolid\n";
			}
		}
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_group_solids;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_group_array;
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

} // namespace io

} // namespace stl

} // namespace module

