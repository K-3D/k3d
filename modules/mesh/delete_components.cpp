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
*/

#include <k3dsdk/attribute_arrays.h>
#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/utility.h>

#include <boost/scoped_ptr.hpp>

#include <list>
#include <set>

namespace module
{

namespace mesh
{

namespace detail
{

/// For each point, create a fast lookup of the outbound edges
// TODO: Move to mesh_topology_data and provide an alternate create_edge_adjacency_lookup that uses the result from this.
void create_vertex_edge_lookup(const k3d::mesh::indices_t& EdgePoints, const k3d::mesh::counts_t& Valences, k3d::mesh::indices_t& PointFirstEdges, k3d::mesh::indices_t& PointEdges)
{
	k3d::mesh::counts_t found_edges(Valences.size(), 0);
	PointFirstEdges.resize(Valences.size(), 0);
	PointEdges.resize(EdgePoints.size(), 0);
	k3d::uint_t count = 0;
	for(k3d::uint_t point = 0; point != Valences.size(); ++point)
	{
		PointFirstEdges[point] = count;
		count += Valences[point];
	}
	
	const k3d::uint_t edge_begin = 0;
	const k3d::uint_t edge_end = edge_begin + EdgePoints.size();
	for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
	{
		const k3d::uint_t point = EdgePoints[edge];
		PointEdges[PointFirstEdges[point] + found_edges[point]] = edge;
		++found_edges[point];
	}
}

/// Return true for selected components
class selected
{
public:
	selected(const k3d::mesh::selection_t& Selection) : m_selection(Selection)
	{
	}
	
	const k3d::bool_t operator()(const k3d::uint_t Index) const
	{
		return m_selection[Index];
	}
	
private:
	const k3d::mesh::selection_t& m_selection;
};

/// Marks the components that will be removed
template<typename FunctorT>
class marker
{
public:
	marker(const k3d::mesh::indices_t& FaceFirstLoops,
			const k3d::mesh::counts_t& FaceLoopCounts,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& EdgePoints,
			const FunctorT& PointTest,
			const FunctorT& EdgeTest,
			const FunctorT& FaceTest,
			k3d::mesh::counts_t& AdjustedFaceLoopCounts,
			k3d::mesh::counts_t& EdgesToRemove,
			k3d::mesh::counts_t& LoopsToRemove,
			k3d::mesh::counts_t& FacesToRemove) :
				m_face_first_loops(FaceFirstLoops),
				m_face_loop_counts(FaceLoopCounts),
				m_loop_first_edges(LoopFirstEdges),
				m_clockwise_edges(ClockwiseEdges),
				m_edge_points(EdgePoints),
				m_point_test(PointTest),
				m_edge_test(EdgeTest),
				m_face_test(FaceTest),
				m_adjusted_face_loop_counts(AdjustedFaceLoopCounts),
				m_edges_to_remove(EdgesToRemove),
				m_loops_to_remove(LoopsToRemove),
				m_faces_to_remove(FacesToRemove)
	{}
	
	void operator()(const k3d::uint_t Face)
	{
		const k3d::uint_t loop_begin = m_face_first_loops[Face];
		const k3d::uint_t loop_end = loop_begin + m_face_loop_counts[Face];
		if(m_face_test(Face))
		{
			m_faces_to_remove[Face] = 1;
		}
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			k3d::uint_t edge_count = 0;
			k3d::uint_t removed_edge_count = 0;
			const k3d::uint_t first_edge = m_loop_first_edges[loop];
			for(k3d::uint_t edge = first_edge; ;)
			{
				++edge_count;
				if(m_edge_test(edge) || m_point_test(m_edge_points[edge]) || m_face_test(Face))
				{
					++removed_edge_count;
					m_loops_to_remove[loop] = 1;
					m_edges_to_remove[edge] = 1;
				}
				
				edge = m_clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
			
			if(m_loops_to_remove[loop])
				--m_adjusted_face_loop_counts[Face];
			
			if(loop == loop_begin && m_loops_to_remove[loop]) // We only remove the whole face if the first loop is removed
				m_faces_to_remove[Face] = 1;
			
			// second loop in case an edge triggered face removal
			if(m_loops_to_remove[loop] && edge_count != removed_edge_count)
			{
				for(k3d::uint_t edge = first_edge; ;)
				{
					m_edges_to_remove[edge] = 1;
					
					edge = m_clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}		
			}
		}
	}
private:
	const k3d::mesh::indices_t& m_face_first_loops;
	const k3d::mesh::counts_t& m_face_loop_counts;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_edge_points;
	const FunctorT& m_point_test;
	const FunctorT& m_edge_test;
	const FunctorT& m_face_test;
	k3d::mesh::counts_t& m_adjusted_face_loop_counts;
	k3d::mesh::counts_t& m_edges_to_remove;
	k3d::mesh::counts_t& m_loops_to_remove;
	k3d::mesh::counts_t& m_faces_to_remove;
};

/// Marks the points to remove
class point_marker
{
public:
	point_marker(const k3d::mesh::counts_t& Valences,
			const k3d::mesh::indices_t& PointFirstEdges,
			const k3d::mesh::indices_t& PointEdges,
			const k3d::mesh::counts_t& EdgesToRemove,
			k3d::mesh::counts_t& PointsToRemove) :
				m_valences(Valences),
				m_point_first_edges(PointFirstEdges),
				m_point_edges(PointEdges),
				m_edges_to_remove(EdgesToRemove),
				m_points_to_remove(PointsToRemove)
	{}
	
	void operator()(const k3d::uint_t Point)
	{
		k3d::uint_t selected_edge_count = 0;
		const k3d::uint_t edge_begin = m_point_first_edges[Point];
		const k3d::uint_t edge_end = edge_begin + m_valences[Point];
		for(k3d::uint_t edge_index = edge_begin; edge_index != edge_end; ++edge_index)
			selected_edge_count += m_edges_to_remove[m_point_edges[edge_index]];
		if(selected_edge_count == m_valences[Point])
			m_points_to_remove[Point] = 1;
	}

private:
	const k3d::mesh::counts_t& m_valences;
	const k3d::mesh::indices_t& m_point_first_edges;
	const k3d::mesh::indices_t& m_point_edges;
	const k3d::mesh::counts_t& m_edges_to_remove;
	k3d::mesh::counts_t& m_points_to_remove;
};

/// Replace the elements of Array with their cumulative sum
template<class ArrayT> void cumulative_sum(ArrayT& Array)
{
	const k3d::uint_t array_begin = 0;
	const k3d::uint_t array_end = Array.size();
	for(k3d::uint_t i = array_begin + 1; i != array_end; ++i)
		Array[i] += Array[i-1];
}

class index_updater
{
public:
	/// Create a new functor to update an array of indices and the values of those indexes
	/**
	 * \param ToRemoveIndicesSum Each ToRemoveIndicesSum[i] contains the number of removed
	 * items from InputArray up until index i in InputArray
	 * \param ToRemoveValuesSum Is the ToRemoveIndicesSum for the array for which InputArray contains indices
	 */
	index_updater(const k3d::mesh::counts_t& ToRemoveIndicesSum,
			const k3d::mesh::counts_t& ToRemoveValuesSum,
			const k3d::mesh::indices_t& InputArray,
			k3d::mesh::indices_t& OutputArray) :
				m_to_remove_indices_sum(ToRemoveIndicesSum),
				m_to_remove_values_sum(ToRemoveValuesSum),
				m_input_array(InputArray),
				m_output_array(OutputArray)
	{}
	
	void operator()(const k3d::uint_t Index)
	{
		if((Index == 0 && m_to_remove_indices_sum[0] == 0) || (Index != 0 && m_to_remove_indices_sum[Index - 1] == m_to_remove_indices_sum[Index]))
		{
			const k3d::uint_t new_value = m_input_array[Index] < m_to_remove_values_sum[m_input_array[Index]] ? 0 : m_input_array[Index] - m_to_remove_values_sum[m_input_array[Index]];
			m_output_array[Index - m_to_remove_indices_sum[Index]] = new_value;
		}
	}
	
private:
	const k3d::mesh::counts_t& m_to_remove_indices_sum;
	const k3d::mesh::counts_t& m_to_remove_values_sum;
	const k3d::mesh::indices_t& m_input_array;
	k3d::mesh::indices_t& m_output_array;
	
};

/// Delete elements of an array
template<typename ArrayT>
class array_element_deleter
{
public:
	array_element_deleter(const k3d::mesh::counts_t& ToRemoveIndicesSum,
			const ArrayT& InputArray,
			ArrayT& OutputArray) :
				m_to_remove_indices_sum(ToRemoveIndicesSum),
				m_input_array(InputArray),
				m_output_array(OutputArray)
	{
		OutputArray.resize(InputArray.size() - ToRemoveIndicesSum.back());
	}
	
	void operator()(const k3d::uint_t Index)
	{
		if((Index == 0 && m_to_remove_indices_sum[0] == 0) || (Index != 0 && m_to_remove_indices_sum[Index - 1] == m_to_remove_indices_sum[Index]))
			m_output_array[Index - m_to_remove_indices_sum[Index]] = m_input_array[Index];
	}
	
private:
	const k3d::mesh::counts_t& m_to_remove_indices_sum;
	const ArrayT& m_input_array;
	ArrayT& m_output_array;
};

/// Delete elements of an attribute array
template<>
class array_element_deleter<k3d::mesh::attribute_arrays_t>
{
public:
	array_element_deleter(const k3d::mesh::counts_t& ToRemoveIndicesSum,
			const k3d::mesh::attribute_arrays_t& InputArray,
			k3d::mesh::attribute_arrays_t& OutputArray) :
				m_to_remove_indices_sum(ToRemoveIndicesSum)
	{
		OutputArray = InputArray.clone_types();
		OutputArray.resize(ToRemoveIndicesSum.size() - ToRemoveIndicesSum.back());
		m_copier.reset(new k3d::attribute_array_copier(InputArray, OutputArray));
	}
	
	void operator()(const k3d::uint_t Index)
	{
		if((Index == 0 && m_to_remove_indices_sum[0] == 0) || (Index != 0 && m_to_remove_indices_sum[Index - 1] == m_to_remove_indices_sum[Index]))
			m_copier->copy(Index, Index - m_to_remove_indices_sum[Index]);
	}
	
private:
	const k3d::mesh::counts_t& m_to_remove_indices_sum;
	boost::scoped_ptr<k3d::attribute_array_copier> m_copier;
};



void update_indices(const k3d::mesh::counts_t& ToRemoveIndicesSum,
			const k3d::mesh::counts_t& ToRemoveValuesSum,
			const k3d::mesh::indices_t& InputArray,
			k3d::mesh::indices_t& OutputArray)
{
	OutputArray.resize(InputArray.size() - ToRemoveIndicesSum.back());
	index_updater updater(ToRemoveIndicesSum, ToRemoveValuesSum, InputArray, OutputArray);
	for(k3d::uint_t i = 0; i != ToRemoveIndicesSum.size(); ++i) updater(i);
}

template<typename ArrayT>
void delete_elements(const k3d::mesh::counts_t& ToRemoveIndicesSum,
			const ArrayT& InputArray,
			ArrayT& OutputArray)
{
	if(InputArray.size() == 0)
		return;
	array_element_deleter<ArrayT> deleter(ToRemoveIndicesSum, InputArray, OutputArray);
	for(k3d::uint_t i = 0; i != ToRemoveIndicesSum.size(); ++i) deleter(i);
}

} // namespace detail

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
		if(!k3d::validate_polyhedra(Input))
			return;
		
		Output = Input;
		const k3d::mesh_selection mesh_selection = m_mesh_selection.pipeline_value();
		k3d::mesh::selection_t point_selection = *Input.point_selection;
		k3d::merge_selection(mesh_selection.points, point_selection);
		k3d::mesh::selection_t edge_selection = *Input.polyhedra->edge_selection;
		k3d::merge_selection(mesh_selection.edges, edge_selection);
		k3d::mesh::selection_t face_selection = *Input.polyhedra->face_selection;
		k3d::merge_selection(mesh_selection.faces, face_selection);
		const k3d::uint_t point_count = Input.points->size();
		const k3d::uint_t edge_count = Input.polyhedra->edge_points->size();
		const k3d::uint_t face_count = Input.polyhedra->face_first_loops->size();
		const k3d::uint_t loop_count = Input.polyhedra->loop_first_edges->size();
		m_points_to_remove.assign(point_count, 0);
		k3d::mesh::counts_t adjusted_face_loop_counts = *Input.polyhedra->face_loop_counts;
		k3d::mesh::counts_t edges_to_remove(edge_count, 0);
		k3d::mesh::counts_t loops_to_remove(loop_count, 0);
		k3d::mesh::counts_t faces_to_remove(face_count, 0);
		detail::marker<detail::selected> marker(*Input.polyhedra->face_first_loops,
				*Input.polyhedra->face_loop_counts,
				*Input.polyhedra->loop_first_edges,
				*Input.polyhedra->clockwise_edges,
				*Input.polyhedra->edge_points,
				detail::selected(point_selection),
				detail::selected(edge_selection),
				detail::selected(face_selection),
				adjusted_face_loop_counts,
				edges_to_remove,
				loops_to_remove,
				faces_to_remove);
		for(k3d::uint_t face = 0; face != face_count; ++face) marker(face);
		
		k3d::mesh::counts_t valences;
		k3d::polyhedron::create_vertex_valence_lookup(Input.points->size(), *Input.polyhedra->edge_points, valences);
		k3d::mesh::indices_t point_first_edges, point_edges;
		detail::create_vertex_edge_lookup(*Input.polyhedra->edge_points, valences, point_first_edges, point_edges);
		
		detail::point_marker point_marker(valences, point_first_edges, point_edges, edges_to_remove, m_points_to_remove);
		for(k3d::uint_t point = 0; point != Input.points->size(); ++point) point_marker(point);
		
		detail::cumulative_sum(m_points_to_remove);
		detail::cumulative_sum(edges_to_remove);
		detail::cumulative_sum(loops_to_remove);
		detail::cumulative_sum(faces_to_remove);
		
		k3d::mesh::polyhedra_t& output_polyhedra = Output.polyhedra.writable();
		
		detail::update_indices(faces_to_remove, loops_to_remove, *Input.polyhedra->face_first_loops, output_polyhedra.face_first_loops.create());
		detail::delete_elements(faces_to_remove, face_selection, output_polyhedra.face_selection.create());
		detail::delete_elements(faces_to_remove, adjusted_face_loop_counts, output_polyhedra.face_loop_counts.create());
		detail::delete_elements(faces_to_remove, *Input.polyhedra->face_materials, output_polyhedra.face_materials.create());
		detail::update_indices(loops_to_remove, edges_to_remove, *Input.polyhedra->loop_first_edges, output_polyhedra.loop_first_edges.create());
		detail::update_indices(edges_to_remove, m_points_to_remove, *Input.polyhedra->edge_points, output_polyhedra.edge_points.create());
		detail::update_indices(edges_to_remove, edges_to_remove, *Input.polyhedra->clockwise_edges, output_polyhedra.clockwise_edges.create());
		detail::delete_elements(edges_to_remove, edge_selection, output_polyhedra.edge_selection.create());
		detail::delete_elements(m_points_to_remove, point_selection, Output.point_selection.writable());
		detail::delete_elements(m_points_to_remove, Input.vertex_data, Output.vertex_data);
		detail::delete_elements(edges_to_remove, Input.polyhedra->face_varying_data, output_polyhedra.face_varying_data);
		detail::delete_elements(faces_to_remove, Input.polyhedra->uniform_data, output_polyhedra.uniform_data);
		
		// Update the per-polyhedra arrays
		const k3d::mesh::indices_t& input_first_faces = *Input.polyhedra->first_faces;
		const k3d::mesh::counts_t& input_face_counts = *Input.polyhedra->face_counts;
		k3d::mesh::indices_t& output_first_faces = output_polyhedra.first_faces.create();
		k3d::mesh::counts_t& output_face_counts = output_polyhedra.face_counts.create();
		const k3d::uint_t polyhedra_count = input_first_faces.size();
		k3d::uint_t new_first_face = 0;
		k3d::uint_t total_removed_faces = 0;
		output_polyhedra.constant_data = Input.polyhedra->constant_data.clone_types();
		k3d::attribute_array_copier constant_copier(Input.polyhedra->constant_data, output_polyhedra.constant_data);
		k3d::mesh::polyhedra_t::types_t& polyhedra_types = output_polyhedra.types.create();
		for(k3d::uint_t polyhedron = 0; polyhedron < polyhedra_count; ++polyhedron)
		{
			k3d::uint_t deleted_face_count = faces_to_remove[input_first_faces[polyhedron] + input_face_counts[polyhedron] - 1] - total_removed_faces;
			total_removed_faces += deleted_face_count;
			k3d::uint_t new_face_count = input_face_counts[polyhedron] - deleted_face_count;
			if(new_face_count > 0)
			{
				output_face_counts.push_back(new_face_count);
				output_first_faces.push_back(new_first_face);
				constant_copier.push_back(polyhedron);
				polyhedra_types.push_back(Input.polyhedra->types->at(polyhedron));
			}
			new_first_face = output_first_faces.empty() ? 0 : output_first_faces.back() + output_face_counts.back();
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		detail::delete_elements(m_points_to_remove, *Input.points, Output.points.writable());
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<delete_components,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x2d738b7a, 0x6f473349, 0x53a066ad, 0xa857f734),
				"Delete",
				"Deletes selected faces, edges and vertices",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
private:
	k3d::mesh::counts_t m_points_to_remove;
};

/////////////////////////////////////////////////////////////////////////////
// delete_components_factory

k3d::iplugin_factory& delete_components_factory()
{
	return delete_components::get_factory();
}

} // namespace mesh

} // namespace module

