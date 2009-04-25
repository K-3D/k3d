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
#include <k3d-version-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/ihint.h>
#include <k3dsdk/mesh_writer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

#include <map>

namespace module
{

namespace stl
{

namespace io
{

namespace detail
{

typedef k3d::typed_array<k3d::string_t> strings_t;
typedef std::map<k3d::string_t, k3d::mesh::indices_t> solids_t;

/// Groups triangles into named solids, based on the given uniform array name 
void extract_solids(const k3d::polyhedron::const_primitive& Polyhedron, const k3d::string_t& ArrayName, solids_t& Solids)
{
	const strings_t* solid_names = Polyhedron.uniform_data.lookup<strings_t>(ArrayName); 
	for(k3d::uint_t face = 0; face != Polyhedron.face_first_loops.size(); ++face)
	{
		k3d::mesh::indices_t& indices = solid_names ? Solids[solid_names->at(face)] : Solids["default"];
		const k3d::uint_t first_edge = Polyhedron.loop_first_edges[Polyhedron.face_first_loops[face]]; 
		for(k3d::uint_t edge = first_edge; ; )
		{
			indices.push_back(Polyhedron.edge_points[edge]);

			edge = Polyhedron.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// mesh_writerio

class mesh_writer :
	public k3d::mesh_writer<k3d::node >
{
	typedef k3d::mesh_writer<k3d::node > base;

public:
	mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_solid_labels(init_owner(*this) + init_name("solid_labels") + init_label(_("Solid Labels")) + init_description(_("Name of a per-face array containing solid labels (as used in OpenFOAM)")) + init_value(std::string("solids")))
	{
		m_solid_labels.changed_signal().connect(k3d::hint::converter<
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
      boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
      if(!polyhedron)
        continue;
      
      if(!k3d::polyhedron::is_triangles(*polyhedron))
        continue;

      detail::solids_t solids;
      detail::extract_solids(*polyhedron, m_solid_labels.pipeline_value(), solids);
      const k3d::mesh::points_t& points = *Input.points;
      
      for(detail::solids_t::const_iterator solid = solids.begin(); solid != solids.end(); ++solid)
      {
        Output << "solid " << solid->first << "\n";
        const k3d::mesh::indices_t corner_indices = solid->second;
        for(k3d::uint_t corner = 0; corner != corner_indices.size(); ++corner)
        {
          if(corner % 3 == 0)
          {
            Output << "facet normal 0.0 0.0 0.0\n";
            Output << "  outer loop\n";
          }

          Output << "    vertex " << points[corner_indices[corner]] << "\n";

          if((corner+1) % 3 == 0)
          {
            Output << "  endloop\n";
            Output << "endfacet\n";
          }
        }
        Output << "endsolid\n";
      }
    }
	}

private:
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_solid_labels;
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

} // namespace io

} // namespace stl

} // namespace module

