// K-3D
// Copyright (c) 2005-2009 Timothy M. Shead
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

#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/euler_operations.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
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

/////////////////////////////////////////////////////////////////////////////
// merge_coplanar_faces

class merge_coplanar_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	merge_coplanar_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_threshold(init_owner(*this) + init_name("threshold") + init_label(_("Threshold")) + init_description(_("Controls the sensitivity for deciding when two edges are collinear.")) + init_value(1e-8) + init_step_increment(1e-8) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_threshold.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(Input));
		if(!input_polyhedron)
			return;

		boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::validate(Output));
		
		k3d::mesh::selection_t input_face_selection = input_polyhedron->face_selections;
		const k3d::mesh_selection mesh_selection = m_mesh_selection.pipeline_value();
		k3d::mesh_selection::merge(mesh_selection.faces, input_face_selection);
		k3d::mesh_selection::merge(mesh_selection, Output);
		
		const k3d::mesh::points_t& points = *Input.points;
		
		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = face_begin + input_polyhedron->face_first_loops.size();
		
		k3d::mesh::bools_t boundary_edges;
		k3d::mesh::indices_t companions;
		k3d::polyhedron::create_edge_adjacency_lookup(input_polyhedron->edge_points, input_polyhedron->clockwise_edges, boundary_edges, companions);
		
		// Calculate the face normals
		k3d::mesh::normals_t face_normals(input_polyhedron->face_first_loops.size());
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			face_normals[face] = k3d::normalize(k3d::normal(input_polyhedron->edge_points, input_polyhedron->clockwise_edges, points, input_polyhedron->loop_first_edges[input_polyhedron->face_first_loops[face]]));
		}
		
		k3d::mesh::indices_t edge_faces;
		k3d::polyhedron::create_edge_face_lookup(input_polyhedron->face_first_loops, input_polyhedron->face_loop_counts, input_polyhedron->loop_first_edges, input_polyhedron->clockwise_edges, edge_faces);
		k3d::mesh::indices_t redundant_edges;
		k3d::polyhedron::mark_coplanar_edges(companions, boundary_edges, face_normals, edge_faces, input_face_selection, redundant_edges, m_threshold.pipeline_value());
	
		k3d::euler::kill_edge_make_loop(*output_polyhedron, redundant_edges, boundary_edges, companions, points, face_normals);
		k3d::mesh::delete_unused_points(Output);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<merge_coplanar_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xba2b777f, 0xad4d6bbb, 0x04277595, 0x88105177),
				"MergeCoplanarFaces",
				_("Merges faces that are coplanar, up to a given threshold"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_threshold;
};

/////////////////////////////////////////////////////////////////////////////
// merge_coplanar_faces_factory

k3d::iplugin_factory& merge_coplanar_faces_factory()
{
	return merge_coplanar_faces::get_factory();
}

} // namespace mesh

} // namespace module
