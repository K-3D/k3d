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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

namespace module
{

namespace mesh
{

namespace detail
{

/// Gets the edges to subdivide, and creates a mapping between the old edge index and the new index
class edge_index_calculator
{
public:
	edge_index_calculator(const k3d::mesh::polyhedra_t& Polyhedra,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::bools_t& BoundaryEdges,
			const k3d::uint_t SplitPointCount,
			const k3d::uint_t FirstNewPoint,
			k3d::mesh::indices_t& EdgeList,
			k3d::mesh::indices_t& IndexMap,
			k3d::mesh::indices_t& FirstMidPoint,
			k3d::mesh::bools_t& HasMidPoint) :
				edge_count(0),
				m_polyhedra(Polyhedra),
				m_companions(Companions),
				m_boundary_edges(BoundaryEdges),
				m_split_point_count(SplitPointCount),
				m_first_new_point(FirstNewPoint),
				m_edge_list(EdgeList),
				m_index_map(IndexMap),
				m_first_midpoint(FirstMidPoint),
				m_has_midpoint(HasMidPoint)
			{}

	void operator()(const k3d::uint_t Face)
	{
		const k3d::mesh::indices_t& face_first_loops = *m_polyhedra.face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *m_polyhedra.face_loop_counts;
		const k3d::mesh::selection_t& edge_selection = *m_polyhedra.edge_selection;
		const k3d::mesh::indices_t& loop_first_edges = *m_polyhedra.loop_first_edges;
		const k3d::mesh::indices_t& clockwise_edges = *m_polyhedra.clockwise_edges;

		const k3d::uint_t loop_begin = face_first_loops[Face];
		const k3d::uint_t loop_end = loop_begin + face_loop_counts[Face];
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const k3d::uint_t first_edge = loop_first_edges[loop];
			for(k3d::uint_t edge = first_edge; ; )
			{
				m_index_map[edge] = edge_count;
				++edge_count;
				if(!m_boundary_edges[edge] && edge_selection[m_companions[edge]] && !edge_selection[edge])
				{
					edge_count += m_split_point_count;
					m_first_midpoint[edge] = m_first_midpoint[m_companions[edge]];
				}

				if(edge_selection[edge])
				{
					edge_count += m_split_point_count;
					if(!m_boundary_edges[edge] && m_has_midpoint[m_companions[edge]])
					{
						m_first_midpoint[edge] = m_first_midpoint[m_companions[edge]];
					}
					else
					{
						m_first_midpoint[edge] = m_first_new_point + m_split_point_count * m_edge_list.size();
						m_edge_list.push_back(edge);
						m_has_midpoint[edge] = true;
					}
				}

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}

	/// The new total edge count
	k3d::uint_t edge_count;

private:
	const k3d::mesh::polyhedra_t& m_polyhedra;
	const k3d::mesh::indices_t& m_companions;
	const k3d::mesh::bools_t& m_boundary_edges;
	const k3d::uint_t m_split_point_count;
	const k3d::uint_t m_first_new_point;
	k3d::mesh::indices_t& m_edge_list;
	k3d::mesh::indices_t& m_index_map;
	k3d::mesh::indices_t& m_first_midpoint;
	k3d::mesh::bools_t& m_has_midpoint;
};

/// Adapts the topology so edges are split at the splitpoints
class edge_splitter
{
public:
	edge_splitter(const k3d::mesh::polyhedra_t& InputPolyhedra,
			const k3d::mesh::indices_t& EdgeList,
			const k3d::mesh::indices_t& IndexMap,
			const k3d::mesh::indices_t& FirstMidpoint,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::bools_t& BoundaryEdges,
			const k3d::uint_t SplitPointCount,
			k3d::mesh::indices_t& OutputEdgePoints,
			k3d::mesh::indices_t& OutputClockwiseEdges,
			k3d::attribute_array_copier& Copier) :
				m_input_polyhedra(InputPolyhedra),
				m_edge_list(EdgeList),
				m_index_map(IndexMap),
				m_first_midpoint(FirstMidpoint),
				m_companions(Companions),
				m_boundary_edges(BoundaryEdges),
				m_split_point_count(SplitPointCount),
				m_output_edge_points(OutputEdgePoints),
				m_output_clockwise_edges(OutputClockwiseEdges),
				m_copier(Copier)
	{
	}

	void operator()(const k3d::uint_t EdgeIndex)
	{
		const k3d::double_t weight_step = 1.0 / static_cast<double>(m_split_point_count + 1);
		const k3d::uint_t edge = m_edge_list[EdgeIndex];
		{
			const k3d::uint_t old_clockwise = m_input_polyhedra.clockwise_edges->at(edge);
			const k3d::uint_t first_new_edge = m_index_map[edge] + 1;
			const k3d::uint_t indices[] = {edge, old_clockwise};
			for(k3d::uint_t i = 0; i != m_split_point_count; ++i)
			{
				const k3d::uint_t new_edge = first_new_edge + i;
				m_output_edge_points[new_edge] = m_first_midpoint[edge] + i;
				m_output_clockwise_edges[new_edge- 1] = new_edge;

				// copy varying data
				const k3d::double_t last_weight = weight_step * static_cast<k3d::double_t>(i+1);
				const k3d::double_t weights[] = {1 - last_weight, last_weight};
				m_copier.copy(2, indices, weights, new_edge);
			}
			m_output_clockwise_edges[first_new_edge + m_split_point_count - 1] = m_index_map[old_clockwise];
		}
		if(!m_boundary_edges[edge])
		{
			const k3d::uint_t companion = m_companions[edge];
			const k3d::uint_t old_clockwise = m_input_polyhedra.clockwise_edges->at(companion);
			const k3d::uint_t first_new_edge = m_index_map[companion] + 1;
			const k3d::uint_t indices[] = {companion, old_clockwise};
			for(k3d::uint_t i = 0; i != m_split_point_count; ++i)
			{
				const k3d::uint_t new_edge = first_new_edge + i;
				m_output_edge_points[new_edge] = m_first_midpoint[edge] + m_split_point_count - i - 1;
				m_output_clockwise_edges[new_edge - 1] = new_edge;

				// copy varying data
				const k3d::double_t last_weight = weight_step * static_cast<k3d::double_t>(i + 1);
				const k3d::double_t weights[] = {1 - last_weight, last_weight};
				m_copier.copy(2, indices, weights, new_edge);
			}
			m_output_clockwise_edges[first_new_edge + m_split_point_count - 1] = m_index_map[old_clockwise];
		}
	}

private:
	const k3d::mesh::polyhedra_t& m_input_polyhedra;
	const k3d::mesh::indices_t& m_edge_list;
	const k3d::mesh::indices_t& m_index_map;
	const k3d::mesh::indices_t& m_first_midpoint;
	const k3d::mesh::indices_t& m_companions;
	const k3d::mesh::bools_t& m_boundary_edges;
	const k3d::uint_t m_split_point_count;
	k3d::mesh::indices_t& m_output_edge_points;
	k3d::mesh::indices_t& m_output_clockwise_edges;
	k3d::attribute_array_copier& m_copier;
};

/// Updates edge indices using the mapping from old to new indices
class edge_index_updater
{
public:
	edge_index_updater(const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& InputClockwiseEdges,
			const k3d::mesh::indices_t& IndexMap,
			k3d::mesh::indices_t& OutputEdgePoints,
			k3d::mesh::indices_t& OutputClockwiseEdges,
			k3d::attribute_array_copier& Copier) :
				m_input_edge_points(InputEdgePoints),
				m_input_clockwise_edges(InputClockwiseEdges),
				m_index_map(IndexMap),
				m_output_edge_points(OutputEdgePoints),
				m_output_clockwise_edges(OutputClockwiseEdges),
				m_copier(Copier)
	{}

	void operator()(const k3d::uint_t Edge)
	{
		const k3d::uint_t mapped_edge = m_index_map[Edge];
		m_output_edge_points[mapped_edge] = m_input_edge_points[Edge];
		m_output_clockwise_edges[mapped_edge] = m_index_map[m_input_clockwise_edges[Edge]];
		m_copier.copy(Edge, mapped_edge);
	}

private:
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_input_clockwise_edges;
	const k3d::mesh::indices_t& m_index_map;
	k3d::mesh::indices_t& m_output_edge_points;
	k3d::mesh::indices_t& m_output_clockwise_edges;
	k3d::attribute_array_copier& m_copier;
};

/// Calculates the split points positions for each edge
class split_point_calculator
{
public:
	split_point_calculator(const k3d::mesh::indices_t& EdgeList,
	const k3d::mesh::indices_t& InputEdgePoints,
	const k3d::mesh::indices_t& InputClockwiseEdges,
	const k3d::uint_t SplitPointCount,
	const k3d::uint_t FirstNewPoint,
	k3d::mesh::points_t& Points) :
		m_edge_list(EdgeList),
		m_input_edge_points(InputEdgePoints),
		m_input_clockwise_edges(InputClockwiseEdges),
		m_split_point_count(SplitPointCount),
		m_first_new_point(FirstNewPoint),
		m_points(Points)
	{
	}

	void operator()(const k3d::uint_t EdgeIndex)
	{
		const k3d::uint_t edge = m_edge_list[EdgeIndex];
		const k3d::point3& start_point = m_points[m_input_edge_points[edge]];
		const k3d::point3& end_point = m_points[m_input_edge_points[m_input_clockwise_edges[edge]]];
		const k3d::vector3 step = (end_point - start_point) / static_cast<double>(m_split_point_count + 1);
		for(k3d::uint_t split = 0; split != m_split_point_count; ++split)
		{
			m_points[(EdgeIndex * m_split_point_count) + m_first_new_point + split] = start_point + (split + 1) * step;
		}
	}

private:
	const k3d::mesh::indices_t& m_edge_list;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_input_clockwise_edges;
	const k3d::uint_t m_split_point_count;
	const k3d::uint_t m_first_new_point;
	k3d::mesh::points_t& m_points;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// subdivide_edges

class subdivide_edges :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	subdivide_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_vertices(init_owner(*this) + init_name("vertices") + init_label(_("Vertices")) + init_description(_("Number of vertices to insert in each selected edge")) + init_value(1L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1)))
	{
		m_vertices.changed_signal().connect(make_reset_mesh_slot());
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		m_edge_list.clear();

		// If there are no valid polyhedra, we give up
		document().pipeline_profiler().start_execution(*this, "Validate input");
		if(!k3d::validate_polyhedra(Input))
		{
			document().pipeline_profiler().finish_execution(*this, "Validate input");
			return;
		}
		document().pipeline_profiler().finish_execution(*this, "Validate input");

		// Shallow copy of the input (no data is copied, only shared pointers are)
		document().pipeline_profiler().start_execution(*this, "Merge selection");
		Output = Input;
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh
		document().pipeline_profiler().finish_execution(*this, "Merge selection");

		k3d::mesh::polyhedra_t& polyhedra = Output.polyhedra.writable();

		document().pipeline_profiler().start_execution(*this, "Calculate companions");
		k3d::mesh::bools_t boundary_edges;
		k3d::mesh::indices_t companions;
		k3d::polyhedron::create_edge_adjacency_lookup(*Input.polyhedra->edge_points, *Input.polyhedra->clockwise_edges, boundary_edges, companions);
		document().pipeline_profiler().finish_execution(*this, "Calculate companions");

		const k3d::uint_t split_point_count = m_vertices.pipeline_value();

		document().pipeline_profiler().start_execution(*this, "Calculate indices");
		const k3d::uint_t old_edge_count = Input.polyhedra->edge_points->size();
		k3d::mesh::indices_t index_map(old_edge_count);
		k3d::mesh::indices_t first_midpoint(old_edge_count);
		k3d::mesh::bools_t has_midpoint(old_edge_count);
		detail::edge_index_calculator edge_index_calculator(polyhedra,
				companions,
				boundary_edges,
				split_point_count,
				Input.points->size(),
				m_edge_list,
				index_map,
				first_midpoint,
				has_midpoint);
		for(k3d::uint_t face = 0; face != Input.polyhedra->face_first_loops->size(); ++face) edge_index_calculator(face);
		document().pipeline_profiler().finish_execution(*this, "Calculate indices");

		document().pipeline_profiler().start_execution(*this, "Allocate memory");
		k3d::mesh::indices_t& output_edge_points = polyhedra.edge_points.create(new k3d::mesh::indices_t(edge_index_calculator.edge_count));
		k3d::mesh::indices_t& output_clockwise_edges = polyhedra.clockwise_edges.create(new k3d::mesh::indices_t(edge_index_calculator.edge_count));
		k3d::mesh::selection_t& output_edge_selection = polyhedra.edge_selection.create(new k3d::mesh::selection_t(edge_index_calculator.edge_count, 0.0));
		k3d::mesh::points_t& output_points = Output.points.writable();
		k3d::mesh::selection_t& output_point_selection = Output.point_selection.writable();
		const k3d::uint_t new_point_count = m_edge_list.size() * split_point_count + Input.points->size();
		output_points.resize(new_point_count);
		output_point_selection.resize(new_point_count, 1.0);
		k3d::mesh::indices_t& output_loop_first_edges = polyhedra.loop_first_edges.writable();
		polyhedra.face_varying_data = Input.polyhedra->face_varying_data.clone_types();
		polyhedra.face_varying_data.resize(edge_index_calculator.edge_count);
		k3d::attribute_array_copier face_varying_data_copier(Input.polyhedra->face_varying_data, polyhedra.face_varying_data);
		document().pipeline_profiler().finish_execution(*this, "Allocate memory");

		document().pipeline_profiler().start_execution(*this, "Update indices");
		detail::edge_index_updater edge_index_updater(*Input.polyhedra->edge_points,
				*Input.polyhedra->clockwise_edges,
				index_map,
				output_edge_points,
				output_clockwise_edges,
				face_varying_data_copier);
		for(k3d::uint_t edge = 0; edge != index_map.size(); ++edge) edge_index_updater(edge);
		for(k3d::uint_t loop = 0; loop != output_loop_first_edges.size(); ++loop)
			output_loop_first_edges[loop] = index_map[output_loop_first_edges[loop]];
		document().pipeline_profiler().finish_execution(*this, "Update indices");

		document().pipeline_profiler().start_execution(*this, "Split edges");
		detail::edge_splitter edge_splitter(*Input.polyhedra,
					m_edge_list,
					index_map,
					first_midpoint,
					companions,
					boundary_edges,
					split_point_count,
					output_edge_points,
					output_clockwise_edges,
					face_varying_data_copier);
		for(k3d::uint_t edge_index = 0; edge_index != m_edge_list.size(); ++edge_index) edge_splitter(edge_index);
		document().pipeline_profiler().finish_execution(*this, "Split edges");
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		document().pipeline_profiler().start_execution(*this, "Validate input");
		if(!k3d::validate_polyhedra(Input))
		{
			document().pipeline_profiler().finish_execution(*this, "Validate input");
			return;
		}
		document().pipeline_profiler().finish_execution(*this, "Validate input");

		k3d::mesh::points_t& output_points = Output.points.writable();

		document().pipeline_profiler().start_execution(*this, "Calculate positions");
		detail::split_point_calculator split_point_calculator(m_edge_list,
				*Input.polyhedra->edge_points,
				*Input.polyhedra->clockwise_edges,
				m_vertices.pipeline_value(),
				Input.points->size(),
				output_points);
		for(k3d::uint_t edge_index = 0; edge_index != m_edge_list.size(); ++edge_index) split_point_calculator(edge_index);
		document().pipeline_profiler().finish_execution(*this, "Calculate positions");
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<subdivide_edges,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x62de51ff, 0xe70f423a, 0x953d899a, 0x1dbf5c38),
				"SubdivideEdges",
				"Subdivides edges by creating one or more vertices along",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_vertices;

	// Cache the midpoints, for fast updating
	k3d::mesh::indices_t m_edge_list;
};

/////////////////////////////////////////////////////////////////////////////
// subdivide_edges_factory

k3d::iplugin_factory& subdivide_edges_factory()
{
	return subdivide_edges::get_factory();
}

} // namespace mesh

} // namespace module

