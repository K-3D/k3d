// K-3D
// Copyright (c) 2005-2008 Timothy M. Shead
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
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

namespace detail
{

void tag_edge(k3d::mesh::bools_t& TaggedEdges, k3d::mesh::indices_t& EdgeShells, k3d::mesh::indices_t& ShellEdges, const k3d::uint_t Shell, const k3d::uint_t Edge, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::indices_t& AdjacentEdges)
{
	if(TaggedEdges[Edge])
		return;

	TaggedEdges[Edge] = true;
	EdgeShells[Edge] = Shell;
	ShellEdges.push_back(Edge);
	tag_edge(TaggedEdges, EdgeShells, ShellEdges, Shell, AdjacentEdges[Edge], ClockwiseEdges, AdjacentEdges);
	tag_edge(TaggedEdges, EdgeShells, ShellEdges, Shell, ClockwiseEdges[Edge], ClockwiseEdges, AdjacentEdges);
}

void fix_shell(k3d::polyhedron::primitive& Output, const k3d::polyhedron::const_primitive& Input, const k3d::mesh::indices_t& ShellEdges, const k3d::mesh::indices_t& CounterClockwiseEdges)
{
	for(k3d::uint_t i = 0; i != ShellEdges.size(); ++i)
	{
		const k3d::uint_t edge = ShellEdges[i];
		Output.edge_points[edge] = Input.edge_points[Input.clockwise_edges[edge]];
		Output.clockwise_edges[edge] = CounterClockwiseEdges[edge];
	}
}

void fix_orientation(k3d::polyhedron::primitive& Output, const k3d::polyhedron::const_primitive& Input)
{
	k3d::mesh::bools_t boundary_edges;
	k3d::mesh::indices_t adjacent_edges;
	k3d::polyhedron::create_edge_adjacency_lookup(Input.edge_points, Input.clockwise_edges, boundary_edges, adjacent_edges);

	const k3d::uint_t edge_count = boundary_edges.size();

	// Collect counter-clockwise edges
	k3d::mesh::indices_t counter_clockwise_edges(edge_count);
	for(k3d::uint_t edge = 0; edge != edge_count; ++edge)
	{
		counter_clockwise_edges[Input.clockwise_edges[edge]] = edge;
	}

	// For each edge, tag all edges in its shell
	k3d::mesh::indices_t edge_shells(edge_count);
	k3d::mesh::bools_t tagged_edges(edge_count, false);
	std::vector<k3d::mesh::indices_t> shells;
	for(k3d::uint_t edge = 0; edge != edge_count; ++edge)
	{
		if(!tagged_edges[edge])
		{
			k3d::mesh::indices_t shell_edges;
			// recursively tag the entire shell
			tag_edge(tagged_edges, edge_shells, shell_edges, shells.size(), edge, Input.clockwise_edges, adjacent_edges);
			shells.push_back(shell_edges);
		}
	}

	const k3d::uint_t shell_count = shells.size();
	k3d::uint_t total = 0;
	for(k3d::uint_t shell = 0; shell != shell_count; ++shell)
	{
		total += shells[shell].size();
	}
	return_if_fail(total == edge_count);
	k3d::log() << debug << "FixOrientation: found " << shell_count << " shells" << std::endl;

	// Mark the shells with correct orientation, i.e. shells with selected faces
	k3d::mesh::bools_t valid_shells(shell_count, false);
	const k3d::uint_t face_count = Input.face_first_loops.size();
	for(k3d::uint_t face = 0; face != face_count; ++face)
	{
		if(Output.face_selections[face])
			valid_shells[edge_shells[Input.loop_first_edges[Input.face_first_loops[face]]]] = true;
	}

	for(k3d::uint_t shell = 0; shell != shell_count; ++shell)
	{
		if(!valid_shells[shell])
		{
			fix_shell(Output, Input, shells[shell], counter_clockwise_edges);
		}
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// fix_orientation

class fix_orientation :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	fix_orientation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// Shallow copy of the input (no data is copied, only shared pointers are)
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh

		for(k3d::uint_t i = 0; i != Input.primitives.size(); ++i)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(Input, *Input.primitives[i]));
			if(!input_polyhedron)
				continue;
			boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::validate(Output, Output.primitives[i]));
			return_if_fail(output_polyhedron);

			detail::fix_orientation(*output_polyhedron, *input_polyhedron);
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<fix_orientation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x919c2a62, 0x97488804, 0x1ff6b580, 0x4c890bb2),
				"FixOrientation",
				_("Fix orientation, based on one or more correctly-oriented selected faces. Any shell without any selected face will have the orientation of all of its faces flipped"),
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// fix_orientation_factory

k3d::iplugin_factory& fix_orientation_factory()
{
	return fix_orientation::get_factory();
}

} // namespace polyhedron

} // namespace module

