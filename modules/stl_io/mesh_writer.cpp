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
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_write_mesh(const k3d::mesh& Input, const k3d::filesystem::path& OutputPath, std::ostream& Output)
	{
		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input, **primitive));
			if(!polyhedron)
				continue;
			
			if(!k3d::polyhedron::is_triangles(*polyhedron))
				continue;

			// Group faces together by default, allowing for optional user-specified groups ...
			k3d::mesh::strings_t face_groups(polyhedron->face_shells.size(), "default");
			if(m_group_solids.pipeline_value())
			{
				const k3d::mesh::strings_t* const group_array = polyhedron->face_attributes.lookup<k3d::mesh::strings_t>(m_group_array.pipeline_value());
				return_if_fail(group_array);

				face_groups.assign(group_array->begin(), group_array->end());
			}

			std::set<k3d::string_t> face_group_names(face_groups.begin(), face_groups.end());

			// Compute face normals, so we can write an STL file for maximum portability ...
			k3d::mesh::normals_t face_normals;
			k3d::polyhedron::create_face_normal_lookup(Input, *polyhedron, face_normals);

			// Compute counterclockwise edges, since STL faces use right-hand (counterclockwise) winding ...
			k3d::mesh::indices_t counterclockwise_edges;
			k3d::polyhedron::create_counterclockwise_edge_lookup(*polyhedron, counterclockwise_edges);

			// For each group of faces, write-out an STL solid ...
			const k3d::mesh::points_t& points = *Input.points;
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
			for(std::set<k3d::string_t>::const_iterator face_group_name = face_group_names.begin(); face_group_name != face_group_names.end(); ++face_group_name)
			{
				Output << "solid " << *face_group_name << "\n";

				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					if(face_groups[face] != *face_group_name)
						continue;

					Output << "facet normal " << face_normals[face] << "\n";
					Output << "  outer loop\n";

					const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
					for(k3d::uint_t edge = first_edge; ;)
					{
						Output << "    vertex " << points[polyhedron->vertex_points[edge]] << "\n";

						edge = counterclockwise_edges[edge];
						if(edge == first_edge)
							break;
					}

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

