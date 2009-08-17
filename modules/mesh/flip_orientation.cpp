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
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/bicubic_patch.h>
#include <k3dsdk/bilinear_patch.h>
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

namespace mesh
{

namespace detail
{

void flip_polyhedron(k3d::polyhedron::primitive& Output, const k3d::polyhedron::const_primitive& Input)
{
	const k3d::uint_t edge_count = Input.edge_points.size();
	k3d::mesh::indices_t counter_clockwise_edges(edge_count);
	for(k3d::uint_t edge = 0; edge != edge_count; ++edge)
	{
		counter_clockwise_edges[Input.clockwise_edges[edge]] = edge;
	}
	for(k3d::uint_t face = 0; face != Input.face_first_loops.size(); ++face)
	{
		if(!Output.face_selections[face])
			continue;
		const k3d::uint_t loop_begin = Input.face_first_loops[face];
		const k3d::uint_t loop_end = loop_begin + Input.face_loop_counts[face];
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const k3d::uint_t first_edge = Input.loop_first_edges[loop];
			for(k3d::uint_t edge = first_edge; ;)
			{
				Output.edge_points[edge] = Input.edge_points[Input.clockwise_edges[edge]];
				Output.clockwise_edges[edge] = counter_clockwise_edges[edge];

				edge = Input.clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

void flip_bilinear_patch(k3d::bilinear_patch::primitive& Output)
{
	for(k3d::uint_t patch = 0; patch != Output.patch_selections.size(); ++patch)
	{
		if(!Output.patch_selections[patch])
			continue;
		const k3d::uint_t patch_start = 4*patch;
		std::swap(Output.patch_points[patch_start], Output.patch_points[patch_start + 1]);
		std::swap(Output.patch_points[patch_start + 2], Output.patch_points[patch_start + 3]);
	}
}

void flip_bicubic_patch(k3d::bicubic_patch::primitive& Output)
{
	for(k3d::uint_t patch = 0; patch != Output.patch_selections.size(); ++patch)
	{
		if(!Output.patch_selections[patch])
			continue;
		const k3d::uint_t patch_start = 16*patch;
		std::swap(Output.patch_points[patch_start], Output.patch_points[patch_start + 3]);
		std::swap(Output.patch_points[patch_start + 1], Output.patch_points[patch_start + 2]);
		std::swap(Output.patch_points[patch_start + 4], Output.patch_points[patch_start + 7]);
		std::swap(Output.patch_points[patch_start + 5], Output.patch_points[patch_start + 6]);
		std::swap(Output.patch_points[patch_start + 8], Output.patch_points[patch_start + 11]);
		std::swap(Output.patch_points[patch_start + 9], Output.patch_points[patch_start + 10]);
		std::swap(Output.patch_points[patch_start + 12], Output.patch_points[patch_start + 15]);
		std::swap(Output.patch_points[patch_start + 13], Output.patch_points[patch_start + 14]);
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// flip_orientation_implementation

class flip_orientation_implementation :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	flip_orientation_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh
		for(k3d::uint_t i = 0; i != Input.primitives.size(); ++i)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(Input, *Input.primitives[i]));
			boost::scoped_ptr<k3d::bilinear_patch::const_primitive> input_bilinear_patch(k3d::bilinear_patch::validate(Input, *Input.primitives[i]));
			boost::scoped_ptr<k3d::bicubic_patch::const_primitive> input_bicubic_patch(k3d::bicubic_patch::validate(Input, *Input.primitives[i]));
			if(input_polyhedron)
			{
				boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::validate(Output, Output.primitives[i]));
				return_if_fail(output_polyhedron);
				detail::flip_polyhedron(*output_polyhedron, *input_polyhedron);
			}
			else if(input_bilinear_patch)
			{
				boost::scoped_ptr<k3d::bilinear_patch::primitive> output_patch(k3d::bilinear_patch::validate(Output, Output.primitives[i]));
				return_if_fail(output_patch);
				detail::flip_bilinear_patch(*output_patch);
			}
			else if(input_bicubic_patch)
			{
				boost::scoped_ptr<k3d::bicubic_patch::primitive> output_patch(k3d::bicubic_patch::validate(Output, Output.primitives[i]));
				return_if_fail(output_patch);
				detail::flip_bicubic_patch(*output_patch);
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<flip_orientation_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid( 0x7d95cd84, 0xee5a4ad6, 0x86feab94, 0xcbea91ca),
				"FlipOrientation",
				_("Reverses the orientation of selected polygons and bicubic or bilinear patches"),
				"Polygon",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// flip_orientation_factory

k3d::iplugin_factory& flip_orientation_factory()
{
	return flip_orientation_implementation::get_factory();
}

} // namespace mesh

} // namespace module


