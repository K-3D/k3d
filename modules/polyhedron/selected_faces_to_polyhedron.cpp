// K-3D
// Copyright (c) 2005-2010 Timothy M. Shead
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

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// selected_faces_to_polyhedron

class selected_faces_to_polyhedron :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	selected_faces_to_polyhedron(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::mesh tmp = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), tmp);

		Output.points = Input.points;
		Output.point_attributes = Input.point_attributes;
		Output.point_selection = Input.point_selection;

		boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::create(Output));

		output_polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);

		for(k3d::uint_t i = 0; i != Input.primitives.size(); ++i)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(tmp, *tmp.primitives[i]));
			if(!polyhedron)
				continue;

			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = polyhedron->face_selections.size();
			k3d::mesh::selection_t input_selected_faces(face_end - face_begin, 0.0);
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(polyhedron->face_selections[face])
				{
					output_polyhedron->face_materials.push_back(polyhedron->face_materials[face]);
					output_polyhedron->face_selections.push_back(polyhedron->face_selections[face]);
					output_polyhedron->face_first_loops.push_back(output_polyhedron->loop_first_edges.size());
					output_polyhedron->face_loop_counts.push_back(polyhedron->face_loop_counts[face]);
					output_polyhedron->face_shells.push_back(0);
					const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
					const k3d::uint_t loop_end = polyhedron->face_loop_counts[face] + loop_begin;
					for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
					{
						output_polyhedron->loop_first_edges.push_back(output_polyhedron->vertex_points.size());
						const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
						const k3d::uint_t new_first_edge = output_polyhedron->vertex_points.size();
						for(k3d::uint_t edge = first_edge; ;)
						{
							output_polyhedron->vertex_points.push_back(polyhedron->vertex_points[edge]);
							output_polyhedron->vertex_selections.push_back(polyhedron->vertex_selections[edge]);
							output_polyhedron->edge_selections.push_back(polyhedron->edge_selections[edge]);
							output_polyhedron->clockwise_edges.push_back(output_polyhedron->vertex_points.size());

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
							{
								output_polyhedron->clockwise_edges.back() = new_first_edge;
								break;
							}
						}
					}
				}
			}
		}
		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<selected_faces_to_polyhedron,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x335179a6, 0x1d42d12a, 0x4e5b8aac, 0x22ce5792),
				"SelectedFacesToPolyhedron",
				"Outputs all selected faces to a single polyhedron.",
				"Polyhedron",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// selected_faces_to_polyhedron_factory

k3d::iplugin_factory& selected_faces_to_polyhedron_factory()
{
	return selected_faces_to_polyhedron::get_factory();
}

} // namespace polyhedron

} // namespace module

