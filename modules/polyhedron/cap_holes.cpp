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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// cap_holes

class cap_holes :
	public k3d::mesh_selection_sink<k3d::material_sink<k3d::mesh_modifier<k3d::node > > >
{
	typedef k3d::mesh_selection_sink<k3d::material_sink<k3d::mesh_modifier<k3d::node > > > base;

public:
	cap_holes(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_cap_polyhedra(init_owner(*this) + init_name("cap_polyhedra") + init_label(_("Cap polyhedra")) + init_description(_("Cap holes (missing faces) in polyhedra")) + init_value(true)),
		m_cap_faces(init_owner(*this) + init_name("cap_faces") + init_label(_("Cap faces")) + init_description(_("Cap holes in faces")) + init_value(true))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_cap_polyhedra.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_cap_faces.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::bool_t cap_polyhedra = m_cap_polyhedra.pipeline_value();
		const k3d::bool_t cap_faces = m_cap_faces.pipeline_value();

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// Create a lookup so we can detect boundary edges ...
			k3d::mesh::bools_t boundary_edges;
			k3d::mesh::indices_t adjacent_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->vertex_points, polyhedron->clockwise_edges, boundary_edges, adjacent_edges);

			// Create a lookup from edges to faces ...
			k3d::mesh::indices_t edge_faces;
			k3d::polyhedron::create_edge_face_lookup(polyhedron->face_first_loops, polyhedron->face_loop_counts, polyhedron->loop_first_edges, polyhedron->clockwise_edges, edge_faces);	

			// Begin with the set of selected edges ...
			const k3d::uint_t edge_begin = 0;
			const k3d::uint_t edge_end = edge_begin + polyhedron->clockwise_edges.size();
			k3d::mesh::bools_t cap_edges(polyhedron->clockwise_edges.size(), false);
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
				cap_edges[edge] = polyhedron->edge_selections[edge] ? true : false;

			// Remove interior edges ...
			for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
			{
				if(!boundary_edges[edge])
					cap_edges[edge] = false;
			}

			// If we aren't going to cap polyhedra, remove all face loops from the edge list ...
			if(!cap_polyhedra)
			{
				const k3d::uint_t face_begin = 0;
				const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					const k3d::uint_t loop_first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
					for(k3d::uint_t edge = loop_first_edge; ;)
					{
						cap_edges[edge] = false;

						edge = polyhedron->clockwise_edges[edge];
						if(edge == loop_first_edge)
							break;
					}
				}
			}

			// If we aren't going to cap faces, remove all face holes from the edge list ...
			if(!cap_faces)
			{
				const k3d::uint_t face_begin = 0;
				const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					const k3d::uint_t loop_end = polyhedron->face_loop_counts[face];
					for(k3d::uint_t loop = 1; loop < loop_end; ++loop)
					{
						const k3d::uint_t loop_first_edge = polyhedron->loop_first_edges[loop];
						for(k3d::uint_t edge = loop_first_edge; ;)
						{
							cap_edges[edge] = false;

							edge = polyhedron->clockwise_edges[edge];
							if(edge == loop_first_edge)
								break;
						}
					}
				}
			}

			// Get ready to copy attributes ...
			k3d::table_copier face_copier(polyhedron->face_attributes, polyhedron->face_attributes);
			k3d::table_copier edge_copier(polyhedron->edge_attributes, polyhedron->edge_attributes);
			k3d::table_copier vertex_copier(polyhedron->vertex_attributes, polyhedron->vertex_attributes);

			// For each remaining cap edge ...
			for(k3d::uint_t loop_first_edge = edge_begin; loop_first_edge != edge_end; ++loop_first_edge)
			{
				if(!cap_edges[loop_first_edge])
					continue;

				// Get the entire boundary loop ...
				k3d::mesh::indices_t loop;
				for(k3d::uint_t edge = loop_first_edge; ;)
				{
					cap_edges[edge] = false;
					loop.push_back(edge);

					// Get the next boundary edge in *counterclockwise* order ...
					edge = polyhedron->clockwise_edges[edge];
					while(!boundary_edges[edge])
						edge = polyhedron->clockwise_edges[adjacent_edges[edge]];
					
					if(edge == loop_first_edge)
						break;
				}

				// Reverse the loop so it's clockwise ...
				std::reverse(loop.begin(), loop.end());

				// Create a new face ...
				polyhedron->face_shells.push_back(polyhedron->face_shells[edge_faces[loop_first_edge]]);
				polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
				polyhedron->face_loop_counts.push_back(1);
				polyhedron->face_selections.push_back(1);
				polyhedron->face_materials.push_back(material);
				polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

				face_copier.push_back(edge_faces[loop_first_edge]);

				for(k3d::uint_t edge = 0; edge != loop.size(); ++edge)
				{
					polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
					polyhedron->edge_selections.push_back(1);
					polyhedron->vertex_points.push_back(polyhedron->vertex_points[loop[edge]]);
					polyhedron->vertex_selections.push_back(0);

					edge_copier.push_back(loop[edge]);
					vertex_copier.push_back(loop[edge]);
				}
				polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cap_holes,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xd9900dbf, 0x3d854a36, 0xbd05a3eb, 0x1379a89a),
				"CapHoles",
				_("Caps (fills) holes in a polyhedron"),
				"Polygon",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap_polyhedra;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap_faces;
};

/////////////////////////////////////////////////////////////////////////////
// cap_holes_factory

k3d::iplugin_factory& cap_holes_factory()
{
	return cap_holes::get_factory();
}

} // namespace polyhedron

} // namespace module

