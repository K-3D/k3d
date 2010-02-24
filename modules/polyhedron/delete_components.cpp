// K-3D
// Copyright (c) 2005-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/table.h>
#include <k3dsdk/table_copier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/utility.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// delete_components

class delete_components :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	delete_components(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points)
			return;
		if(!Output.point_selection)
			return;

		// Mark points to be explicitly removed by the user ...
		k3d::mesh::bools_t remove_points(Output.point_selection->begin(), Output.point_selection->end());
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + Output.point_selection->size();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Mark edges to be explicitly removed by the user ...
			k3d::mesh::bools_t remove_edges(polyhedron->edge_selections.begin(), polyhedron->edge_selections.end());

			// Keep track of loops to be deleted ...
			k3d::mesh::bools_t remove_loops(polyhedron->loop_first_edges.size(), false);

			// Mark faces to be explicitly removed by the user ...
			k3d::mesh::bools_t remove_faces(polyhedron->face_selections.begin(), polyhedron->face_selections.end());

			// Make it happen ...
			k3d::polyhedron::delete_components(Output, *polyhedron, remove_points, remove_edges, remove_loops, remove_faces);
		}

		// Mark points to be implicitly removed because they're no-longer used ...
		k3d::mesh::bools_t unused_point;
		k3d::mesh::lookup_unused_points(Output, unused_point);
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			if(!unused_point[point])
				continue;

			remove_points[point] = true;
		}	

		// Delete points ...
		k3d::mesh::delete_points(Output, remove_points);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<delete_components,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x2d738b7a, 0x6f473349, 0x53a066ad, 0xa857f734),
				"Delete",
				"Deletes selected faces, edges and vertices",
				"Polyhedron",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// delete_components_factory

k3d::iplugin_factory& delete_components_factory()
{
	return delete_components::get_factory();
}

} // namespace polyhedron

} // namespace module

