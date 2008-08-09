// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/user_property_changed_signal.h>

#include <k3dsdk/parallel/blocked_range.h>
#include <k3dsdk/parallel/parallel_for.h>
#include <k3dsdk/parallel/threads.h>

namespace module
{

namespace mesh
{

namespace detail
{

template<typename ArrayT>
class merge_array_worker
{
public:
	merge_array_worker(const ArrayT& InputArray, ArrayT& OutputArray, const k3d::uint_t OriginalSize, const typename ArrayT::value_type Offset) :
		m_input_array(InputArray),
		m_output_array(OutputArray),
		m_original_size(OriginalSize),
		m_offset(Offset) {}
	void operator()(const k3d::parallel::blocked_range<k3d::uint_t>& range) const
	{
		const k3d::uint_t begin = range.begin();
		const k3d::uint_t end = range.end();
		
		for(k3d::uint_t index = begin; index != end; ++index)
		{
			m_output_array[m_original_size + index] = m_input_array[index] + m_offset; 
		}
	}
private:
	const ArrayT& m_input_array;
	ArrayT& m_output_array;
	const k3d::uint_t m_original_size;
	const typename ArrayT::value_type m_offset;
};

template<typename ArrayT>
void extend_array(const ArrayT& InputArray, ArrayT& OutputArray, const typename ArrayT::value_type Offset)
{
	const k3d::uint_t original_size = OutputArray.size();
	OutputArray.resize(original_size + InputArray.size());
	k3d::parallel::parallel_for(
		k3d::parallel::blocked_range<k3d::uint_t>(0, InputArray.size(), k3d::parallel::grain_size()),
		merge_array_worker<ArrayT>(InputArray, OutputArray, original_size, Offset));
}

/// Appends the polyhedra of mesh Input to those of mesh Output, putting everything in one single polyhedron if SinglePolyhedron is true.
void merge_polyhedra(k3d::mesh& Output, const k3d::mesh& Input, bool SinglePolyhedron = true)
{
	if(!k3d::validate_polyhedra(Input))
		return;
	k3d::mesh::polyhedra_t& output_polyhedra = *k3d::make_unique(Output.polyhedra);
	k3d::mesh::indices_t& output_first_faces = *k3d::make_unique(output_polyhedra.first_faces);
	k3d::mesh::counts_t& output_face_counts = *k3d::make_unique(output_polyhedra.face_counts);
	k3d::mesh::polyhedra_t::types_t& output_types = *k3d::make_unique(output_polyhedra.types);
	k3d::mesh::indices_t& output_face_first_loops = *k3d::make_unique(output_polyhedra.face_first_loops);
	k3d::mesh::counts_t& output_face_loop_counts = *k3d::make_unique(output_polyhedra.face_loop_counts);
	k3d::mesh::selection_t& output_face_selection = *k3d::make_unique(output_polyhedra.face_selection);
	k3d::mesh::materials_t& output_face_materials = *k3d::make_unique(output_polyhedra.face_materials);
	k3d::mesh::indices_t& output_loop_first_edges = *k3d::make_unique(output_polyhedra.loop_first_edges);
	k3d::mesh::indices_t& output_edge_points = *k3d::make_unique(output_polyhedra.edge_points);
	k3d::mesh::indices_t& output_clockwise_edges = *k3d::make_unique(output_polyhedra.clockwise_edges);
	k3d::mesh::selection_t& output_edge_selection = *k3d::make_unique(output_polyhedra.edge_selection);
	
	const k3d::mesh::polyhedra_t& input_polyhedra = *Input.polyhedra;
	const k3d::mesh::indices_t& input_first_faces = *input_polyhedra.first_faces;
	const k3d::mesh::counts_t& input_face_counts = *input_polyhedra.face_counts;
	const k3d::mesh::polyhedra_t::types_t& input_types = *input_polyhedra.types;
	const k3d::mesh::indices_t& input_face_first_loops = *input_polyhedra.face_first_loops;
	const k3d::mesh::counts_t& input_face_loop_counts = *input_polyhedra.face_loop_counts;
	const k3d::mesh::selection_t& input_face_selection = *input_polyhedra.face_selection;
	const k3d::mesh::materials_t& input_face_materials = *input_polyhedra.face_materials;
	const k3d::mesh::indices_t& input_loop_first_edges = *input_polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& input_edge_points = *input_polyhedra.edge_points;
	const k3d::mesh::indices_t& input_clockwise_edges = *input_polyhedra.clockwise_edges;
	const k3d::mesh::selection_t& input_edge_selection = *input_polyhedra.edge_selection;
	
  extend_array(input_face_selection, output_face_selection, 0);
  extend_array(input_face_first_loops, output_face_first_loops, output_loop_first_edges.size());
  output_face_loop_counts.insert(output_face_loop_counts.end(), input_face_loop_counts.begin(), input_face_loop_counts.end());
  output_face_materials.insert(output_face_materials.end(), input_face_materials.begin(), input_face_materials.end());
  extend_array(input_edge_selection, output_edge_selection, 0);
  extend_array(input_loop_first_edges, output_loop_first_edges, output_edge_points.size());
  extend_array(input_clockwise_edges, output_clockwise_edges, output_edge_points.size());
  extend_array(input_edge_points, output_edge_points, Output.points->size());
  
  if (output_types.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_polyhedra.constant_data = input_polyhedra.constant_data;
	  output_polyhedra.uniform_data = input_polyhedra.uniform_data;
	  output_polyhedra.face_varying_data = input_polyhedra.face_varying_data;
  }
  
  if (!SinglePolyhedron)
  {
    output_types.insert(output_types.end(), input_types.begin(), input_types.end());
    extend_array(input_first_faces, output_first_faces, output_face_loop_counts.size() - input_face_first_loops.size());
    output_face_counts.insert(output_face_counts.end(), input_face_counts.begin(), input_face_counts.end());
    
  }
  else if (output_types.empty())
  {
  	output_first_faces.push_back(0);
  	output_face_counts.push_back(output_face_first_loops.size());
  	output_types.push_back(k3d::mesh::polyhedra_t::POLYGONS);
  }
  else
  {
  	output_face_counts[0] = output_face_first_loops.size();
  }
  
  // Take care of named arrays
  k3d::attribute_array_copier constant_data_copier(input_polyhedra.constant_data, output_polyhedra.constant_data);
  k3d::attribute_array_copier uniform_data_copier(input_polyhedra.uniform_data, output_polyhedra.uniform_data);
  k3d::attribute_array_copier face_varying_data_copier(input_polyhedra.face_varying_data, output_polyhedra.face_varying_data);
  for (k3d::uint_t polyhedron = 0; polyhedron != input_first_faces.size(); ++polyhedron)
  	constant_data_copier.push_back(polyhedron);
  for (k3d::uint_t face = 0; face != input_face_first_loops.size(); ++face)
  	uniform_data_copier.push_back(face);
  for (k3d::uint_t edge = 0; edge != input_edge_points.size(); ++edge)
  	face_varying_data_copier.push_back(edge);
}

void merge_point_groups(k3d::mesh& Output, const k3d::mesh& Input)
{
	if(!k3d::validate_point_groups(Input))
		return;
	k3d::mesh::point_groups_t& output_point_groups = *k3d::make_unique(Output.point_groups);
	k3d::mesh::indices_t& output_first_points = *k3d::make_unique(output_point_groups.first_points);
	k3d::mesh::counts_t& output_point_counts = *k3d::make_unique(output_point_groups.point_counts);
	k3d::mesh::materials_t& output_materials = *k3d::make_unique(output_point_groups.materials);
	k3d::mesh::indices_t& output_point_indices = *k3d::make_unique(output_point_groups.points);
	
	const k3d::mesh::point_groups_t& input_point_groups = *Input.point_groups;
	const k3d::mesh::indices_t& input_first_points = *input_point_groups.first_points;
	const k3d::mesh::counts_t& input_point_counts = *input_point_groups.point_counts;
	const k3d::mesh::materials_t& input_materials = *input_point_groups.materials;
	const k3d::mesh::indices_t& input_point_indices = *input_point_groups.points;
	
	if (output_point_indices.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_point_groups.constant_data = input_point_groups.constant_data;
  	output_point_groups.varying_data = input_point_groups.varying_data;
  }
	
	k3d::uint_t point_offset = output_point_indices.size();
	extend_array(input_first_points, output_first_points, point_offset);
	extend_array(input_point_counts, output_point_counts, 0);
	output_materials.insert(output_materials.end(), input_materials.begin(), input_materials.end());
	extend_array(input_point_indices, output_point_indices, Output.points->size());
	
	// Named arrays
	k3d::attribute_array_copier constant_data_copier(input_point_groups.constant_data, output_point_groups.constant_data);
	k3d::attribute_array_copier varying_data_copier(input_point_groups.varying_data, output_point_groups.varying_data);
	for (k3d::uint_t point_group = 0; point_group != input_first_points.size(); ++point_group)
  	constant_data_copier.push_back(point_group);
	for (k3d::uint_t point = 0; point != input_point_indices.size(); ++point)
		varying_data_copier.push_back(point);
}

void merge_linear_curve_groups(k3d::mesh& Output, const k3d::mesh& Input)
{
	if(!k3d::validate_linear_curve_groups(Input))
		return;
	k3d::mesh::linear_curve_groups_t& output_linear_curve_groups = *k3d::make_unique(Output.linear_curve_groups);
	k3d::mesh::indices_t& output_first_curves = *k3d::make_unique(output_linear_curve_groups.first_curves);
	k3d::mesh::counts_t& output_curve_counts = *k3d::make_unique(output_linear_curve_groups.curve_counts);
	k3d::mesh::bools_t& output_periodic_curves = *k3d::make_unique(output_linear_curve_groups.periodic_curves);
	k3d::mesh::materials_t& output_materials = *k3d::make_unique(output_linear_curve_groups.materials);
	k3d::mesh::indices_t& output_curve_first_points = *k3d::make_unique(output_linear_curve_groups.curve_first_points);
	k3d::mesh::counts_t& output_curve_point_counts = *k3d::make_unique(output_linear_curve_groups.curve_point_counts);
	k3d::mesh::selection_t& output_curve_selection = *k3d::make_unique(output_linear_curve_groups.curve_selection);
	k3d::mesh::indices_t& output_curve_points = *k3d::make_unique(output_linear_curve_groups.curve_points);
	
	const k3d::mesh::linear_curve_groups_t& input_linear_curve_groups = *Input.linear_curve_groups;
	const k3d::mesh::indices_t& input_first_curves = *input_linear_curve_groups.first_curves;
	const k3d::mesh::counts_t& input_curve_counts = *input_linear_curve_groups.curve_counts;
	const k3d::mesh::bools_t& input_periodic_curves = *input_linear_curve_groups.periodic_curves;
	const k3d::mesh::materials_t& input_materials = *input_linear_curve_groups.materials;
	const k3d::mesh::indices_t& input_curve_first_points = *input_linear_curve_groups.curve_first_points;
	const k3d::mesh::counts_t& input_curve_point_counts = *input_linear_curve_groups.curve_point_counts;
	const k3d::mesh::selection_t& input_curve_selection = *input_linear_curve_groups.curve_selection;
	const k3d::mesh::indices_t& input_curve_points = *input_linear_curve_groups.curve_points;
	
	if (output_curve_points.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_linear_curve_groups.constant_data = input_linear_curve_groups.constant_data;
  	output_linear_curve_groups.uniform_data = input_linear_curve_groups.uniform_data;
  	output_linear_curve_groups.varying_data = input_linear_curve_groups.varying_data;
  }
	
	extend_array(input_first_curves, output_first_curves, output_curve_first_points.size());
	extend_array(input_curve_counts, output_curve_counts, 0);
	output_periodic_curves.insert(output_periodic_curves.end(), input_periodic_curves.begin(), input_periodic_curves.end());
	output_materials.insert(output_materials.end(), input_materials.begin(), input_materials.end());
	extend_array(input_curve_first_points, output_curve_first_points, output_curve_points.size());
	extend_array(input_curve_point_counts, output_curve_point_counts, 0);
	extend_array(input_curve_selection, output_curve_selection, 0);
	extend_array(input_curve_points, output_curve_points, Output.points->size());
	
	// Named arrays
	k3d::attribute_array_copier constant_data_copier(input_linear_curve_groups.constant_data, output_linear_curve_groups.constant_data);
	k3d::attribute_array_copier uniform_data_copier(input_linear_curve_groups.uniform_data, output_linear_curve_groups.uniform_data);
	k3d::attribute_array_copier varying_data_copier(input_linear_curve_groups.varying_data, output_linear_curve_groups.varying_data);
	for (k3d::uint_t curve_group = 0; curve_group != input_first_curves.size(); ++curve_group)
  	constant_data_copier.push_back(curve_group);
	for (k3d::uint_t curve = 0; curve != input_curve_first_points.size(); ++curve)
		uniform_data_copier.push_back(curve);
	for (k3d::uint_t point = 0; point != input_curve_points.size(); ++point)
		varying_data_copier.push_back(point);
}

void merge_cubic_curve_groups(k3d::mesh& Output, const k3d::mesh& Input)
{
	if(!k3d::validate_cubic_curve_groups(Input))
		return;
	k3d::mesh::cubic_curve_groups_t& output_cubic_curve_groups = *k3d::make_unique(Output.cubic_curve_groups);
	k3d::mesh::indices_t& output_first_curves = *k3d::make_unique(output_cubic_curve_groups.first_curves);
	k3d::mesh::counts_t& output_curve_counts = *k3d::make_unique(output_cubic_curve_groups.curve_counts);
	k3d::mesh::bools_t& output_periodic_curves = *k3d::make_unique(output_cubic_curve_groups.periodic_curves);
	k3d::mesh::materials_t& output_materials = *k3d::make_unique(output_cubic_curve_groups.materials);
	k3d::mesh::indices_t& output_curve_first_points = *k3d::make_unique(output_cubic_curve_groups.curve_first_points);
	k3d::mesh::counts_t& output_curve_point_counts = *k3d::make_unique(output_cubic_curve_groups.curve_point_counts);
	k3d::mesh::selection_t& output_curve_selection = *k3d::make_unique(output_cubic_curve_groups.curve_selection);
	k3d::mesh::indices_t& output_curve_points = *k3d::make_unique(output_cubic_curve_groups.curve_points);
	
	const k3d::mesh::cubic_curve_groups_t& input_cubic_curve_groups = *Input.cubic_curve_groups;
	const k3d::mesh::indices_t& input_first_curves = *input_cubic_curve_groups.first_curves;
	const k3d::mesh::counts_t& input_curve_counts = *input_cubic_curve_groups.curve_counts;
	const k3d::mesh::bools_t& input_periodic_curves = *input_cubic_curve_groups.periodic_curves;
	const k3d::mesh::materials_t& input_materials = *input_cubic_curve_groups.materials;
	const k3d::mesh::indices_t& input_curve_first_points = *input_cubic_curve_groups.curve_first_points;
	const k3d::mesh::counts_t& input_curve_point_counts = *input_cubic_curve_groups.curve_point_counts;
	const k3d::mesh::selection_t& input_curve_selection = *input_cubic_curve_groups.curve_selection;
	const k3d::mesh::indices_t& input_curve_points = *input_cubic_curve_groups.curve_points;
	
	if (output_curve_points.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_cubic_curve_groups.constant_data = input_cubic_curve_groups.constant_data;
  	output_cubic_curve_groups.uniform_data = input_cubic_curve_groups.uniform_data;
  	output_cubic_curve_groups.varying_data = input_cubic_curve_groups.varying_data;
  }
	
	extend_array(input_first_curves, output_first_curves, output_curve_first_points.size());
	extend_array(input_curve_counts, output_curve_counts, 0);
	output_periodic_curves.insert(output_periodic_curves.end(), input_periodic_curves.begin(), input_periodic_curves.end());
	output_materials.insert(output_materials.end(), input_materials.begin(), input_materials.end());
	extend_array(input_curve_first_points, output_curve_first_points, output_curve_points.size());
	extend_array(input_curve_point_counts, output_curve_point_counts, 0);
	extend_array(input_curve_selection, output_curve_selection, 0);
	extend_array(input_curve_points, output_curve_points, Output.points->size());
	
	// Named arrays
	k3d::attribute_array_copier constant_data_copier(input_cubic_curve_groups.constant_data, output_cubic_curve_groups.constant_data);
	k3d::attribute_array_copier uniform_data_copier(input_cubic_curve_groups.uniform_data, output_cubic_curve_groups.uniform_data);
	k3d::attribute_array_copier varying_data_copier(input_cubic_curve_groups.varying_data, output_cubic_curve_groups.varying_data);
	for (k3d::uint_t curve_group = 0; curve_group != input_first_curves.size(); ++curve_group)
  	constant_data_copier.push_back(curve_group);
	for (k3d::uint_t curve = 0; curve != input_curve_first_points.size(); ++curve)
		uniform_data_copier.push_back(curve);
	for (k3d::uint_t point = 0; point != input_curve_points.size(); ++point)
		varying_data_copier.push_back(point);
}

void merge_nurbs_curve_groups(k3d::mesh& Output, const k3d::mesh& Input)
{
	if(!k3d::validate_nurbs_curve_groups(Input))
		return;
	
	k3d::mesh::nurbs_curve_groups_t& output_nurbs_curve_groups = *k3d::make_unique(Output.nurbs_curve_groups);
	k3d::mesh::indices_t& output_first_curves = *k3d::make_unique(output_nurbs_curve_groups.first_curves);
	k3d::mesh::counts_t& output_curve_counts = *k3d::make_unique(output_nurbs_curve_groups.curve_counts);
	k3d::mesh::materials_t& output_materials = *k3d::make_unique(output_nurbs_curve_groups.materials);
	k3d::mesh::indices_t& output_curve_first_points = *k3d::make_unique(output_nurbs_curve_groups.curve_first_points);
	k3d::mesh::counts_t& output_curve_point_counts = *k3d::make_unique(output_nurbs_curve_groups.curve_point_counts);
	k3d::mesh::orders_t& output_curve_orders = *k3d::make_unique(output_nurbs_curve_groups.curve_orders);
	k3d::mesh::indices_t& output_curve_first_knots = *k3d::make_unique(output_nurbs_curve_groups.curve_first_knots);
	k3d::mesh::selection_t& output_curve_selection = *k3d::make_unique(output_nurbs_curve_groups.curve_selection);
	k3d::mesh::indices_t& output_curve_points = *k3d::make_unique(output_nurbs_curve_groups.curve_points);
	k3d::mesh::weights_t& output_curve_point_weights = *k3d::make_unique(output_nurbs_curve_groups.curve_point_weights);
	k3d::mesh::knots_t& output_curve_knots = *k3d::make_unique(output_nurbs_curve_groups.curve_knots);
	
	const k3d::mesh::nurbs_curve_groups_t& input_nurbs_curve_groups = *Input.nurbs_curve_groups;
	const k3d::mesh::indices_t& input_first_curves = *input_nurbs_curve_groups.first_curves;
	const k3d::mesh::counts_t& input_curve_counts = *input_nurbs_curve_groups.curve_counts;
	const k3d::mesh::materials_t& input_materials = *input_nurbs_curve_groups.materials;
	const k3d::mesh::indices_t& input_curve_first_points = *input_nurbs_curve_groups.curve_first_points;
	const k3d::mesh::counts_t& input_curve_point_counts = *input_nurbs_curve_groups.curve_point_counts;
	const k3d::mesh::orders_t& input_curve_orders = *input_nurbs_curve_groups.curve_orders;
	const k3d::mesh::indices_t& input_curve_first_knots = *input_nurbs_curve_groups.curve_first_knots;
	const k3d::mesh::selection_t& input_curve_selection = *input_nurbs_curve_groups.curve_selection;
	const k3d::mesh::indices_t& input_curve_points = *input_nurbs_curve_groups.curve_points;
	const k3d::mesh::weights_t& input_curve_point_weights = *input_nurbs_curve_groups.curve_point_weights;
	const k3d::mesh::knots_t& input_curve_knots = *input_nurbs_curve_groups.curve_knots;
	
	if (output_curve_points.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_nurbs_curve_groups.constant_data = input_nurbs_curve_groups.constant_data;
  	output_nurbs_curve_groups.uniform_data = input_nurbs_curve_groups.uniform_data;
  	output_nurbs_curve_groups.varying_data = input_nurbs_curve_groups.varying_data;
  }
	
	extend_array(input_first_curves, output_first_curves, output_curve_first_points.size());
	extend_array(input_curve_counts, output_curve_counts, 0);
	output_materials.insert(output_materials.end(), input_materials.begin(), input_materials.end());
	extend_array(input_curve_first_points, output_curve_first_points, output_curve_points.size());
	extend_array(input_curve_point_counts, output_curve_point_counts, 0);
	extend_array(input_curve_orders, output_curve_orders, 0);
	extend_array(input_curve_first_knots, output_curve_first_knots, output_curve_knots.size());
	extend_array(input_curve_selection, output_curve_selection, 0);
	extend_array(input_curve_points, output_curve_points, Output.points->size());
	extend_array(input_curve_point_weights, output_curve_point_weights, 0);
	output_curve_knots.insert(output_curve_knots.end(), input_curve_knots.begin(), input_curve_knots.end());
	
	// Named arrays
	k3d::attribute_array_copier constant_data_copier(input_nurbs_curve_groups.constant_data, output_nurbs_curve_groups.constant_data);
	k3d::attribute_array_copier uniform_data_copier(input_nurbs_curve_groups.uniform_data, output_nurbs_curve_groups.uniform_data);
	k3d::attribute_array_copier varying_data_copier(input_nurbs_curve_groups.varying_data, output_nurbs_curve_groups.varying_data);
	for (k3d::uint_t curve_group = 0; curve_group != input_first_curves.size(); ++curve_group)
  	constant_data_copier.push_back(curve_group);
	for (k3d::uint_t curve = 0; curve != input_curve_first_points.size(); ++curve)
		uniform_data_copier.push_back(curve);
	for (k3d::uint_t point = 0; point != input_curve_points.size(); ++point)
		varying_data_copier.push_back(point);
}

void merge_bilinear_patches(k3d::mesh& Output, const k3d::mesh& Input)
{
	if(!k3d::validate_bilinear_patches(Input))
		return;
	k3d::mesh::bilinear_patches_t& output_patches = *k3d::make_unique(Output.bilinear_patches);
	k3d::mesh::selection_t& output_patch_selection = *k3d::make_unique(output_patches.patch_selection);
	k3d::mesh::materials_t& output_patch_materials = *k3d::make_unique(output_patches.patch_materials);
	k3d::mesh::indices_t& output_patch_points = *k3d::make_unique(output_patches.patch_points);

	const k3d::mesh::bilinear_patches_t& input_patches = *Input.bilinear_patches;
	const k3d::mesh::selection_t& input_patch_selection = *input_patches.patch_selection;
	const k3d::mesh::materials_t& input_patch_materials = *input_patches.patch_materials;
	const k3d::mesh::indices_t& input_patch_points = *input_patches.patch_points;

	if (output_patch_points.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_patches.constant_data = input_patches.constant_data;
  	output_patches.uniform_data = input_patches.uniform_data;
  	output_patches.varying_data = input_patches.varying_data;
  }
	
	extend_array(input_patch_selection, output_patch_selection, 0);
	output_patch_materials.insert(output_patch_materials.end(), input_patch_materials.begin(), input_patch_materials.end());
	extend_array(input_patch_points, output_patch_points, Output.points->size());
	
	// Named arrays
	k3d::attribute_array_copier constant_data_copier(input_patches.constant_data, output_patches.constant_data);
	k3d::attribute_array_copier uniform_data_copier(input_patches.uniform_data, output_patches.uniform_data);
	k3d::attribute_array_copier varying_data_copier(input_patches.varying_data, output_patches.varying_data);
	for (k3d::uint_t patch = 0; patch != input_patch_selection.size(); ++patch)
	{
  	constant_data_copier.push_back(patch);
  	uniform_data_copier.push_back(patch);
	}
	for (k3d::uint_t corner = 0; corner != input_patch_points.size(); ++corner)
		varying_data_copier.push_back(corner);
}

void merge_bicubic_patches(k3d::mesh& Output, const k3d::mesh& Input)
{
	if(!k3d::validate_bicubic_patches(Input))
		return;
	k3d::mesh::bicubic_patches_t& output_patches = *k3d::make_unique(Output.bicubic_patches);
	k3d::mesh::selection_t& output_patch_selection = *k3d::make_unique(output_patches.patch_selection);
	k3d::mesh::materials_t& output_patch_materials = *k3d::make_unique(output_patches.patch_materials);
	k3d::mesh::indices_t& output_patch_points = *k3d::make_unique(output_patches.patch_points);

	const k3d::mesh::bicubic_patches_t& input_patches = *Input.bicubic_patches;
	const k3d::mesh::selection_t& input_patch_selection = *input_patches.patch_selection;
	const k3d::mesh::materials_t& input_patch_materials = *input_patches.patch_materials;
	const k3d::mesh::indices_t& input_patch_points = *input_patches.patch_points;

	if (output_patch_points.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_patches.constant_data = input_patches.constant_data;
  	output_patches.uniform_data = input_patches.uniform_data;
  	output_patches.varying_data = input_patches.varying_data;
  }
	
	extend_array(input_patch_selection, output_patch_selection, 0);
	output_patch_materials.insert(output_patch_materials.end(), input_patch_materials.begin(), input_patch_materials.end());
	extend_array(input_patch_points, output_patch_points, Output.points->size());
	
	// Named arrays
	k3d::attribute_array_copier constant_data_copier(input_patches.constant_data, output_patches.constant_data);
	k3d::attribute_array_copier uniform_data_copier(input_patches.uniform_data, output_patches.uniform_data);
	k3d::attribute_array_copier varying_data_copier(input_patches.varying_data, output_patches.varying_data);
	for (k3d::uint_t patch = 0; patch != input_patch_selection.size(); ++patch)
	{
  	constant_data_copier.push_back(patch);
  	uniform_data_copier.push_back(patch);
	}
	for (k3d::uint_t corner = 0; corner != input_patch_points.size(); ++corner)
		varying_data_copier.push_back(corner);
}

void merge_nurbs_patches(k3d::mesh& Output, const k3d::mesh& Input)
{
	if (!k3d::validate_nurbs_patches(Input))
		return;
	
	k3d::mesh::nurbs_patches_t& output_nurbs_patches = *k3d::make_unique(Output.nurbs_patches);
	k3d::mesh::indices_t& output_patch_first_points = *k3d::make_unique(output_nurbs_patches.patch_first_points);
	k3d::mesh::counts_t& output_patch_u_point_counts = *k3d::make_unique(output_nurbs_patches.patch_u_point_counts);
	k3d::mesh::counts_t& output_patch_v_point_counts = *k3d::make_unique(output_nurbs_patches.patch_v_point_counts);
	k3d::mesh::orders_t& output_patch_u_orders = *k3d::make_unique(output_nurbs_patches.patch_u_orders);
	k3d::mesh::orders_t& output_patch_v_orders = *k3d::make_unique(output_nurbs_patches.patch_v_orders);
	k3d::mesh::indices_t& output_patch_u_first_knots = *k3d::make_unique(output_nurbs_patches.patch_u_first_knots);
	k3d::mesh::indices_t& output_patch_v_first_knots = *k3d::make_unique(output_nurbs_patches.patch_v_first_knots);
	k3d::mesh::selection_t& output_patch_selection = *k3d::make_unique(output_nurbs_patches.patch_selection);
	k3d::mesh::materials_t& output_patch_materials = *k3d::make_unique(output_nurbs_patches.patch_materials);
	k3d::mesh::indices_t& output_patch_points = *k3d::make_unique(output_nurbs_patches.patch_points);
	k3d::mesh::weights_t& output_patch_point_weights = *k3d::make_unique(output_nurbs_patches.patch_point_weights);
	k3d::mesh::knots_t& output_patch_u_knots = *k3d::make_unique(output_nurbs_patches.patch_u_knots);
	k3d::mesh::knots_t& output_patch_v_knots = *k3d::make_unique(output_nurbs_patches.patch_v_knots);
	
	const k3d::mesh::nurbs_patches_t& input_nurbs_patches = *Input.nurbs_patches;
	const k3d::mesh::indices_t& input_patch_first_points = *input_nurbs_patches.patch_first_points;
	const k3d::mesh::counts_t& input_patch_u_point_counts = *input_nurbs_patches.patch_u_point_counts;
	const k3d::mesh::counts_t& input_patch_v_point_counts = *input_nurbs_patches.patch_v_point_counts;
	const k3d::mesh::orders_t& input_patch_u_orders = *input_nurbs_patches.patch_u_orders;
	const k3d::mesh::orders_t& input_patch_v_orders = *input_nurbs_patches.patch_v_orders;
	const k3d::mesh::indices_t& input_patch_u_first_knots = *input_nurbs_patches.patch_u_first_knots;
	const k3d::mesh::indices_t& input_patch_v_first_knots = *input_nurbs_patches.patch_v_first_knots;
	const k3d::mesh::selection_t& input_patch_selection = *input_nurbs_patches.patch_selection;
	const k3d::mesh::materials_t& input_patch_materials = *input_nurbs_patches.patch_materials;
	const k3d::mesh::indices_t& input_patch_points = *input_nurbs_patches.patch_points;
	const k3d::mesh::weights_t& input_patch_point_weights = *input_nurbs_patches.patch_point_weights;
	const k3d::mesh::knots_t& input_patch_u_knots = *input_nurbs_patches.patch_u_knots;
	const k3d::mesh::knots_t& input_patch_v_knots = *input_nurbs_patches.patch_v_knots;

	if (input_nurbs_patches.patch_trim_curve_loop_counts && input_nurbs_patches.trim_points)
	{
		k3d::mesh::counts_t& output_patch_trim_curve_loop_counts = *k3d::make_unique(output_nurbs_patches.patch_trim_curve_loop_counts);
		k3d::mesh::indices_t& output_patch_first_trim_curve_loops = *k3d::make_unique(output_nurbs_patches.patch_first_trim_curve_loops);
		k3d::mesh::points_2d_t& output_trim_points = *k3d::make_unique(output_nurbs_patches.trim_points);
		k3d::mesh::selection_t& output_trim_point_selection = *k3d::make_unique(output_nurbs_patches.trim_point_selection);
		k3d::mesh::indices_t& output_first_trim_curves = *k3d::make_unique(output_nurbs_patches.first_trim_curves);
		k3d::mesh::counts_t& output_trim_curve_counts = *k3d::make_unique(output_nurbs_patches.trim_curve_counts);
		k3d::mesh::selection_t& output_trim_curve_loop_selection = *k3d::make_unique(output_nurbs_patches.trim_curve_loop_selection);
		k3d::mesh::indices_t& output_trim_curve_first_points = *k3d::make_unique(output_nurbs_patches.trim_curve_first_points);
		k3d::mesh::counts_t& output_trim_curve_point_counts = *k3d::make_unique(output_nurbs_patches.trim_curve_point_counts);
		k3d::mesh::orders_t& output_trim_curve_orders = *k3d::make_unique(output_nurbs_patches.trim_curve_orders);
		k3d::mesh::indices_t& output_trim_curve_first_knots = *k3d::make_unique(output_nurbs_patches.trim_curve_first_knots);
		k3d::mesh::selection_t& output_trim_curve_selection = *k3d::make_unique(output_nurbs_patches.trim_curve_selection);
		k3d::mesh::indices_t& output_trim_curve_points = *k3d::make_unique(output_nurbs_patches.trim_curve_points);
		k3d::mesh::weights_t& output_trim_curve_point_weights = *k3d::make_unique(output_nurbs_patches.trim_curve_point_weights);
		k3d::mesh::knots_t& output_trim_curve_knots = *k3d::make_unique(output_nurbs_patches.trim_curve_knots);
		
		output_patch_first_trim_curve_loops.resize(output_patch_first_points.size(), 0);
		output_patch_trim_curve_loop_counts.resize(output_patch_first_points.size(), 0);
		
		const k3d::mesh::counts_t& input_patch_trim_curve_loop_counts = *input_nurbs_patches.patch_trim_curve_loop_counts;
		const k3d::mesh::indices_t& input_patch_first_trim_curve_loops = *input_nurbs_patches.patch_first_trim_curve_loops;
		const k3d::mesh::points_2d_t& input_trim_points = *input_nurbs_patches.trim_points;
		const k3d::mesh::selection_t& input_trim_point_selection = *input_nurbs_patches.trim_point_selection;
		const k3d::mesh::indices_t& input_first_trim_curves = *input_nurbs_patches.first_trim_curves;
		const k3d::mesh::counts_t& input_trim_curve_counts = *input_nurbs_patches.trim_curve_counts;
		const k3d::mesh::selection_t& input_trim_curve_loop_selection = *input_nurbs_patches.trim_curve_loop_selection;
		const k3d::mesh::indices_t& input_trim_curve_first_points = *input_nurbs_patches.trim_curve_first_points;
		const k3d::mesh::counts_t& input_trim_curve_point_counts = *input_nurbs_patches.trim_curve_point_counts;
		const k3d::mesh::orders_t& input_trim_curve_orders = *input_nurbs_patches.trim_curve_orders;
		const k3d::mesh::indices_t& input_trim_curve_first_knots = *input_nurbs_patches.trim_curve_first_knots;
		const k3d::mesh::selection_t& input_trim_curve_selection = *input_nurbs_patches.trim_curve_selection;
		const k3d::mesh::indices_t& input_trim_curve_points = *input_nurbs_patches.trim_curve_points;
		const k3d::mesh::weights_t& input_trim_curve_point_weights = *input_nurbs_patches.trim_curve_point_weights;
		const k3d::mesh::knots_t& input_trim_curve_knots = *input_nurbs_patches.trim_curve_knots;
		
		extend_array(input_patch_trim_curve_loop_counts, output_patch_trim_curve_loop_counts, 0);
		extend_array(input_patch_first_trim_curve_loops, output_patch_first_trim_curve_loops, output_first_trim_curves.size());
		extend_array(input_trim_point_selection, output_trim_point_selection, 0);
		extend_array(input_first_trim_curves, output_first_trim_curves, output_trim_curve_first_points.size());
		extend_array(input_trim_curve_counts, output_trim_curve_counts, 0);
		extend_array(input_trim_curve_loop_selection, output_trim_curve_loop_selection, 0);
		extend_array(input_trim_curve_first_points, output_trim_curve_first_points, output_trim_curve_points.size());
		extend_array(input_trim_curve_point_counts, output_trim_curve_point_counts, 0);
		extend_array(input_trim_curve_orders, output_trim_curve_orders, 0);
		extend_array(input_trim_curve_first_knots, output_trim_curve_first_knots, output_trim_curve_knots.size());
		extend_array(input_trim_curve_selection, output_trim_curve_selection, 0);
		extend_array(input_trim_curve_points, output_trim_curve_points, output_trim_points.size());
		extend_array(input_trim_curve_point_weights, output_trim_curve_point_weights, 0);
		extend_array(input_trim_curve_knots, output_trim_curve_knots, 0);
		output_trim_points.insert(output_trim_points.end(), input_trim_points.begin(), input_trim_points.end());
	}
	
	extend_array(input_patch_first_points, output_patch_first_points, output_patch_points.size());
	extend_array(input_patch_u_point_counts, output_patch_u_point_counts, 0);
	extend_array(input_patch_v_point_counts, output_patch_v_point_counts, 0);
	extend_array(input_patch_u_orders, output_patch_u_orders, 0);
	extend_array(input_patch_v_orders, output_patch_v_orders, 0);
	extend_array(input_patch_u_first_knots, output_patch_u_first_knots, output_patch_u_knots.size());
	extend_array(input_patch_v_first_knots, output_patch_v_first_knots, output_patch_v_knots.size());
	extend_array(input_patch_selection, output_patch_selection, 0);
	output_patch_materials.insert(output_patch_materials.end(), input_patch_materials.begin(), input_patch_materials.end());
	extend_array(input_patch_points, output_patch_points, Output.points->size());
	extend_array(input_patch_point_weights, output_patch_point_weights, 0);
	extend_array(input_patch_u_knots, output_patch_u_knots, 0);
	extend_array(input_patch_v_knots, output_patch_v_knots, 0);
	
	// Named arrays
	k3d::attribute_array_copier constant_data_copier(input_nurbs_patches.constant_data, output_nurbs_patches.constant_data);
	k3d::attribute_array_copier uniform_data_copier(input_nurbs_patches.uniform_data, output_nurbs_patches.uniform_data);
	k3d::attribute_array_copier varying_data_copier(input_nurbs_patches.varying_data, output_nurbs_patches.varying_data);
	for (k3d::uint_t patch = 0; patch != input_patch_selection.size(); ++patch)
	{
  	constant_data_copier.push_back(patch);
  	uniform_data_copier.push_back(patch);
  	varying_data_copier.push_back(patch);
  	varying_data_copier.push_back(patch);
  	varying_data_copier.push_back(patch);
  	varying_data_copier.push_back(patch);
	}
}

void merge_blobbies(k3d::mesh& Output, const k3d::mesh& Input)
{
	if (!k3d::validate_blobbies(Input))
		return;
	k3d::mesh::blobbies_t& output_blobbies = *k3d::make_unique(Output.blobbies);
	k3d::mesh::indices_t& output_first_primitives = *k3d::make_unique(output_blobbies.first_primitives);
	k3d::mesh::counts_t& output_primitive_counts = *k3d::make_unique(output_blobbies.primitive_counts);
	k3d::mesh::indices_t& output_first_operators = *k3d::make_unique(output_blobbies.first_operators);
	k3d::mesh::counts_t& output_operator_counts = *k3d::make_unique(output_blobbies.operator_counts);
	k3d::mesh::materials_t& output_materials = *k3d::make_unique(output_blobbies.materials);
	k3d::mesh::blobbies_t::primitives_t& output_primitives = *k3d::make_unique(output_blobbies.primitives);
	k3d::mesh::indices_t& output_primitive_first_floats = *k3d::make_unique(output_blobbies.primitive_first_floats);
	k3d::mesh::counts_t& output_primitive_float_counts = *k3d::make_unique(output_blobbies.primitive_float_counts);
	k3d::mesh::blobbies_t::operators_t& output_operators = *k3d::make_unique(output_blobbies.operators);
	k3d::mesh::indices_t& output_operator_first_operands = *k3d::make_unique(output_blobbies.operator_first_operands);
	k3d::mesh::counts_t& output_operator_operand_counts = *k3d::make_unique(output_blobbies.operator_operand_counts);
	k3d::mesh::blobbies_t::floats_t& output_floats = *k3d::make_unique(output_blobbies.floats);
	k3d::mesh::blobbies_t::operands_t& output_operands = *k3d::make_unique(output_blobbies.operands);
	
	const k3d::mesh::blobbies_t& input_blobbies = *Input.blobbies;
	const k3d::mesh::indices_t& input_first_primitives = *input_blobbies.first_primitives;
	const k3d::mesh::counts_t& input_primitive_counts = *input_blobbies.primitive_counts;
	const k3d::mesh::indices_t& input_first_operators = *input_blobbies.first_operators;
	const k3d::mesh::counts_t& input_operator_counts = *input_blobbies.operator_counts;
	const k3d::mesh::materials_t& input_materials = *input_blobbies.materials;
	const k3d::mesh::blobbies_t::primitives_t& input_primitives = *input_blobbies.primitives;
	const k3d::mesh::indices_t& input_primitive_first_floats = *input_blobbies.primitive_first_floats;
	const k3d::mesh::counts_t& input_primitive_float_counts = *input_blobbies.primitive_float_counts;
	const k3d::mesh::blobbies_t::operators_t& input_operators = *input_blobbies.operators;
	const k3d::mesh::indices_t& input_operator_first_operands = *input_blobbies.operator_first_operands;
	const k3d::mesh::counts_t& input_operator_operand_counts = *input_blobbies.operator_operand_counts;
	const k3d::mesh::blobbies_t::floats_t& input_floats = *input_blobbies.floats;
	const k3d::mesh::blobbies_t::operands_t& input_operands = *input_blobbies.operands;
	
	extend_array(input_first_primitives, output_first_primitives, output_primitives.size());
	extend_array(input_primitive_counts, output_primitive_counts, 0);
	extend_array(input_first_operators, output_first_operators, output_operators.size());
	extend_array(input_operator_counts, output_operator_counts, 0);
	output_materials.insert(output_materials.end(), input_materials.begin(), input_materials.end());
	output_primitives.insert(output_primitives.end(), input_primitives.begin(), input_primitives.end());
	extend_array(input_primitive_first_floats, output_primitive_first_floats, output_floats.size());
	extend_array(input_primitive_float_counts, output_primitive_float_counts, 0);
	output_operators.insert(output_operators.end(), input_operators.begin(), input_operators.end());
	extend_array(input_operator_first_operands, output_operator_first_operands, output_operands.size());
	extend_array(input_operator_operand_counts, output_operator_operand_counts, 0);
	output_floats.insert(output_floats.end(), input_floats.begin(), input_floats.end());
	output_operands.insert(output_operands.end(), input_operands.begin(), input_operands.end());
}

void merge_points(k3d::mesh& Output, const k3d::mesh& Input)
{
	if(!k3d::validate_points(Input))
		return;
	k3d::mesh::points_t& output_points = *k3d::make_unique(Output.points);
	k3d::mesh::selection_t& output_point_selection = *k3d::make_unique(Output.point_selection);
	const k3d::mesh::points_t& input_points = *Input.points;
	const k3d::mesh::selection_t& input_point_selection = *Input.point_selection;
	
	if (output_points.empty())
		Output.vertex_data = Input.vertex_data;
	
	extend_array(input_point_selection, output_point_selection, 0);
	output_points.insert(output_points.end(), input_points.begin(), input_points.end());
	
	k3d::attribute_array_copier vertex_data_copier(Input.vertex_data, Output.vertex_data);
	for (k3d::uint_t point = 0; point != input_points.size(); ++point)
		vertex_data_copier.push_back(point);
}

}
/////////////////////////////////////////////////////////////////////////////
// merge_mesh_implementation

class merge_mesh_implementation :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	merge_mesh_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_user_property_changed_signal(*this),
		m_same_polyhedron(init_owner(*this) + init_name("same_polyhedron") + init_label(_("Same polyhedron")) + init_description(_("Merge meshes in a single polyhedron")) + init_value(true))
	{
		m_user_property_changed_signal.connect(sigc::mem_fun(*this, &merge_mesh_implementation::mesh_changed));
		m_same_polyhedron.changed_signal().connect(sigc::mem_fun(*this, &merge_mesh_implementation::mesh_changed));
	}
	
	void mesh_changed(k3d::ihint* Hint)
	{
		if (dynamic_cast<k3d::hint::mesh_geometry_changed*>(Hint))
			m_output_mesh.update(Hint);
		else
			m_output_mesh.update(k3d::hint::mesh_topology_changed::instance());
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::mesh*))
			{
				const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property));
				if (!mesh)
					continue;
				
				// Make sure the points array is defined
				k3d::make_unique(Output.points);
				
				detail::merge_polyhedra(Output, *mesh, m_same_polyhedron.pipeline_value());
				detail::merge_point_groups(Output, *mesh);
				detail::merge_linear_curve_groups(Output, *mesh);
				detail::merge_cubic_curve_groups(Output, *mesh);
				detail::merge_nurbs_curve_groups(Output, *mesh);
				detail::merge_bilinear_patches(Output, *mesh);
				detail::merge_bicubic_patches(Output, *mesh);
				detail::merge_nurbs_patches(Output, *mesh);
				detail::merge_blobbies(Output, *mesh);
				detail::merge_points(Output, *mesh); // Must be last to calculate correct offsets in other methods
			}
		}	
	}

	void on_update_mesh_geometry(k3d::mesh& Output) {}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<merge_mesh_implementation, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imulti_mesh_sink> > > factory(
			k3d::uuid(0x50aef311, 0xf5264da2, 0x9c5995e8, 0xdc2e4ddf),
			"MergeMesh",
			_("Merges two meshes into one"),
			"Mesh",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d::user_property_changed_signal m_user_property_changed_signal;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_same_polyhedron;
};

/////////////////////////////////////////////////////////////////////////////
// merge_mesh_factory

k3d::iplugin_factory& merge_mesh_factory()
{
	return merge_mesh_implementation::get_factory();
}

} // namespace mesh

} // namespace module
