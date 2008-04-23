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
#include <k3dsdk/named_array_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
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
	k3d::mesh::points_t& output_points = *k3d::make_unique(Output.points);
	k3d::mesh::selection_t& output_point_selection = *k3d::make_unique(Output.point_selection);
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
	
	const k3d::mesh::points_t& input_points = *Input.points;
	const k3d::mesh::selection_t& input_point_selection = *Input.point_selection;
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
	
	extend_array(input_point_selection, output_point_selection, 0);
  extend_array(input_face_selection, output_face_selection, 0);
  extend_array(input_face_first_loops, output_face_first_loops, output_loop_first_edges.size());
  output_face_loop_counts.insert(output_face_loop_counts.end(), input_face_loop_counts.begin(), input_face_loop_counts.end());
  output_face_materials.insert(output_face_materials.end(), input_face_materials.begin(), input_face_materials.end());
  extend_array(input_edge_selection, output_edge_selection, 0);
  extend_array(input_loop_first_edges, output_loop_first_edges, output_edge_points.size());
  extend_array(input_clockwise_edges, output_clockwise_edges, output_edge_points.size());
  extend_array(input_edge_points, output_edge_points, output_points.size());
  output_points.insert(output_points.end(), input_points.begin(), input_points.end());
  
  if (output_types.empty()) // for the first appended mesh, we simply copy the named arrays
  {
  	output_polyhedra.constant_data = input_polyhedra.constant_data;
	  output_polyhedra.uniform_data = input_polyhedra.uniform_data;
	  output_polyhedra.face_varying_data = input_polyhedra.face_varying_data;
  }
  
  if (!SinglePolyhedron)
  {
    output_types.insert(output_types.end(), input_types.begin(), input_types.end());
    extend_array(input_first_faces, output_first_faces, output_face_loop_counts.size());
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
  k3d::named_array_copier constant_data_copier(input_polyhedra.constant_data, output_polyhedra.constant_data);
  k3d::named_array_copier uniform_data_copier(input_polyhedra.uniform_data, output_polyhedra.uniform_data);
  k3d::named_array_copier face_varying_data_copier(input_polyhedra.face_varying_data, output_polyhedra.face_varying_data);
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
	k3d::mesh::points_t& output_points = *k3d::make_unique(Output.points);
	k3d::mesh::selection_t& output_point_selection = *k3d::make_unique(Output.point_selection);
	k3d::mesh::point_groups_t& output_point_groups = *k3d::make_unique(Output.point_groups);
	k3d::mesh::indices_t& output_first_points = *k3d::make_unique(output_point_groups.first_points);
	k3d::mesh::counts_t& output_point_counts = *k3d::make_unique(output_point_groups.point_counts);
	k3d::mesh::materials_t& output_materials = *k3d::make_unique(output_point_groups.materials);
	k3d::mesh::indices_t& output_point_indices = *k3d::make_unique(output_point_groups.points);
	
	const k3d::mesh::points_t& input_points = *Input.points;
	const k3d::mesh::selection_t& input_point_selection = *Input.point_selection;
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
	
	extend_array(input_point_selection, output_point_selection, 0);
	k3d::uint_t point_offset = output_point_indices.size();
	extend_array(input_first_points, output_first_points, point_offset);
	extend_array(input_point_counts, output_point_counts, 0);
	output_materials.insert(output_materials.end(), input_materials.begin(), input_materials.end());
	extend_array(input_point_indices, output_point_indices, output_points.size());
	output_points.insert(output_points.end(), input_points.begin(), input_points.end());
	
	// Named arrays
	k3d::named_array_copier constant_data_copier(input_point_groups.constant_data, output_point_groups.constant_data);
	k3d::named_array_copier varying_data_copier(input_point_groups.varying_data, output_point_groups.varying_data);
	for (k3d::uint_t point_group = 0; point_group != input_first_points.size(); ++point_group)
  	constant_data_copier.push_back(point_group);
	for (k3d::uint_t point = 0; point != input_point_indices.size(); ++point)
		varying_data_copier.push_back(point);
}


}
/////////////////////////////////////////////////////////////////////////////
// merge_mesh_implementation

class merge_mesh_implementation :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	merge_mesh_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_user_property_changed_signal(*this),
		m_same_polyhedron(init_owner(*this) + init_name("same_polyhedron") + init_label(_("Same polyhedron")) + init_description(_("Merge meshes in a single polyhedron")) + init_value(true))
	{
		m_user_property_changed_signal.connect(sigc::mem_fun(*this, &merge_mesh_implementation::mesh_changed));
		m_same_polyhedron.changed_signal().connect(sigc::mem_fun(*this, &merge_mesh_implementation::mesh_changed));
	}
	
	void mesh_changed(k3d::iunknown* Hint)
	{
		if (dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint))
			mesh_geometry_changed(Hint);
		else
			mesh_topology_changed(Hint);
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::iproperty_collection::properties_t properties = k3d::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::mesh*))
			{
				const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property));
				if (!mesh)
					continue;
				detail::merge_polyhedra(Mesh, *mesh, m_same_polyhedron.pipeline_value());
				detail::merge_point_groups(Mesh, *mesh);
			}
		}	
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh) {}

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
