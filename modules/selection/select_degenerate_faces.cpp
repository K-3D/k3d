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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// select_degenerate_faces

class select_degenerate_faces :
	public k3d::mesh_selection_modifier<k3d::node>
{
	typedef k3d::mesh_selection_modifier<k3d::node> base;

public:
	select_degenerate_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(polyhedron)
			{
				const k3d::uint_t face_begin = 0;
				const k3d::uint_t face_end = polyhedron->face_first_loops.size();
				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					const k3d::normal3 normal = k3d::polyhedron::normal(polyhedron->vertex_points, polyhedron->clockwise_edges, *Output.points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]);
					polyhedron->face_selections[face] = normal.length2() ? 0.0 : 1.0;
					if(!normal.length2())
						k3d::log() << debug << "found degenerate face: " << face << std::endl;
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_degenerate_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xeb11cc78, 0xd345dad0, 0xc87e38b1, 0xe87b09a6),
				"SelectDegenerateFaces",
				_("Selects all faces with a zero-length normal"),
				"Selection",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// select_degenerate_faces_factory

k3d::iplugin_factory& select_degenerate_faces_factory()
{
	return select_degenerate_faces::get_factory();
}

} // namespace selection

} // namespace module

