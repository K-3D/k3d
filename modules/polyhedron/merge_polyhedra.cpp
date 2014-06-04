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
#include <k3dsdk/table_copier.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

namespace detail
{

template<typename ArrayT>
void append_array(const ArrayT& source, ArrayT& destination)
{
	destination.insert(destination.end(), source.begin(), source.end());
}

}

/////////////////////////////////////////////////////////////////////////////
// merge_polyhedra_implementation

class merge_polyhedra_implementation :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	merge_polyhedra_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output.points = Input.points;
		Output.point_selection = Input.point_selection;
		Output.point_attributes = Input.point_attributes;

		boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::create(Output));

		for(k3d::uint_t i = 0; i != Input.primitives.size(); ++i)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(Input, *Input.primitives[i]));
			if(input_polyhedron)
			{
				k3d::table_copier face_attributes(input_polyhedron->face_attributes, output_polyhedron->face_attributes);
				k3d::table_copier edge_attributes(input_polyhedron->edge_attributes, output_polyhedron->edge_attributes);
				k3d::table_copier vertex_attributes(input_polyhedron->vertex_attributes, output_polyhedron->vertex_attributes);
				k3d::table_copier constant_attributes(input_polyhedron->constant_attributes, output_polyhedron->constant_attributes);

				const k3d::uint_t face_offset = output_polyhedron->face_selections.size();
				const k3d::uint_t edge_offset = output_polyhedron->edge_selections.size();
				const k3d::uint_t vertex_offset = output_polyhedron->vertex_selections.size();
				const k3d::uint_t loop_offset = output_polyhedron->loop_first_edges.size();
				const k3d::uint_t shell_offset = output_polyhedron->shell_types.size();

				detail::append_array(input_polyhedron->shell_types, output_polyhedron->shell_types);
				const k3d::uint_t nb_shells = input_polyhedron->shell_types.size();
				for(k3d::uint_t shell_idx = 0; shell_idx != nb_shells; ++shell_idx)
				{
					constant_attributes.push_back(shell_idx);
				}


				const k3d::uint_t nb_faces = input_polyhedron->face_selections.size();
				for(k3d::uint_t face_idx = 0; face_idx != nb_faces; ++face_idx)
				{
					output_polyhedron->face_shells.push_back(input_polyhedron->face_shells[face_idx]+shell_offset);
					output_polyhedron->face_first_loops.push_back(input_polyhedron->face_first_loops[face_idx] + loop_offset);
					face_attributes.push_back(face_idx);
				}
				detail::append_array(input_polyhedron->face_loop_counts, output_polyhedron->face_loop_counts);
				detail::append_array(input_polyhedron->face_selections, output_polyhedron->face_selections);
				detail::append_array(input_polyhedron->face_materials, output_polyhedron->face_materials);

				const k3d::uint_t nb_loops = input_polyhedron->loop_first_edges.size();
				for(k3d::uint_t loop_idx = 0; loop_idx != nb_loops; ++loop_idx)
				{
					output_polyhedron->loop_first_edges.push_back(input_polyhedron->loop_first_edges[loop_idx] + edge_offset);
				}

				const k3d::uint_t nb_edges = input_polyhedron->edge_selections.size();
				for(k3d::uint_t edge_idx = 0; edge_idx != nb_edges; ++edge_idx)
				{
					output_polyhedron->clockwise_edges.push_back(input_polyhedron->clockwise_edges[edge_idx] + edge_offset);
					edge_attributes.push_back(edge_idx);
					vertex_attributes.push_back(edge_idx);
				}

				detail::append_array(input_polyhedron->edge_selections, output_polyhedron->edge_selections);
				detail::append_array(input_polyhedron->vertex_points, output_polyhedron->vertex_points);
				detail::append_array(input_polyhedron->vertex_selections, output_polyhedron->vertex_selections);
			}
			else
			{
				Output.primitives.push_back(Input.primitives[i]);
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<merge_polyhedra_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x6e7b898a, 0xc1554b80, 0xaaca338d, 0xffe04907),
				"MergePolyhedra",
				_("Merges all polyhedra in a mesh into a single polyhedron"),
				"Polyhedron",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// merge_polyhedra_factory

k3d::iplugin_factory& merge_polyhedra_factory()
{
	return merge_polyhedra_implementation::get_factory();
}

} // namespace polyhedron

} // namespace module


