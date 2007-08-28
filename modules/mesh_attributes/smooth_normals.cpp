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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/named_array_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/shared_pointer.h>

#include <iterator>

namespace module
{

namespace mesh_attributes
{

/////////////////////////////////////////////////////////////////////////////
// smooth_normals

class smooth_normals :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	smooth_normals(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::replace_selection(m_mesh_selection.pipeline_value(), Output);

		if(!k3d::validate_polyhedra(Output))
			return;

		const k3d::mesh::indices_t& face_first_loops = *Output.polyhedra->face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *Output.polyhedra->face_loop_counts;
		const k3d::mesh::selection_t& face_selection = *Output.polyhedra->face_selection;
		const k3d::mesh::indices_t& loop_first_edges = *Output.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Output.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Output.polyhedra->clockwise_edges;
		const k3d::mesh::points_t& points = *Output.points;

		const size_t face_begin = 0;
		const size_t face_end = face_begin + face_first_loops.size();

		// Compute per-face normals ...
		boost::shared_ptr<k3d::mesh::normals_t> face_normal(new k3d::mesh::normals_t(face_first_loops.size()));
		for(size_t face = face_begin; face != face_end; ++face)
			(*face_normal)[face] = k3d::normalize(k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]));

		// Compute per-vertex normals as the sum of adjacent face normals ...
		boost::shared_ptr<k3d::mesh::normals_t> point_normal(new k3d::mesh::normals_t(points.size()));
//		k3d::mesh::counts_t point_normal_count(points.size(), 0);
		for(size_t face = face_begin; face != face_end; ++face)
		{
			const size_t loop_begin = face_first_loops[face];
			const size_t loop_end = loop_begin + face_loop_counts[face];
			for(size_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const size_t first_edge = loop_first_edges[loop];
				for(size_t edge = first_edge; ;)
				{
					(*point_normal)[edge_points[edge]] += (*face_normal)[face];
//					point_normal_count[edge_points[edge]] += 1;

					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}

		// Assign per-vertex normals to each edge (i.e. as facevarying data) ...
		boost::shared_ptr<k3d::mesh::normals_t> edge_normal(new k3d::mesh::normals_t(edge_points.size()));
		const size_t loop_begin = 0;
		const size_t loop_end = loop_begin + loop_first_edges.size();
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = loop_first_edges[loop];
			for(size_t edge = first_edge; ;)
			{
				(*edge_normal)[edge] = (*point_normal)[edge_points[edge]];

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}

		// Store the calculated normals in the output mesh ...
		k3d::mesh::polyhedra_t& polyhedra = *k3d::make_unique(Output.polyhedra);

		polyhedra.uniform_data["N"] = face_normal;
		polyhedra.face_varying_data["N"] = edge_normal;
		Output.vertex_data["N"] = point_normal;
/*

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			typedef std::map<k3d::legacy::split_edge*, k3d::ri::normal> edge_normal_map_t;
			edge_normal_map_t edge_normal_map;

			typedef std::map<k3d::legacy::point*, k3d::ri::normal> point_normal_map_t;
			point_normal_map_t point_normal_map;

			typedef std::map<k3d::legacy::point*, unsigned long> edge_count_map_t;
			edge_count_map_t edge_count_map;

			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				const k3d::normal3 face_normal = k3d::legacy::normal(**face);

				for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
				{
					edge_normal_map[edge] = face_normal;

					point_normal_map_t::iterator point = point_normal_map.insert(std::make_pair(edge->vertex, k3d::ri::normal(0, 0, 0))).first;
					point->second += face_normal;

					edge_count_map_t::iterator edge_count = edge_count_map.insert(std::make_pair(edge->vertex, 0)).first;
					edge_count->second += 1;

					if((*face)->first_edge == edge->face_clockwise)
						break;
				}
			}

			// Finish calculating average normals for all our points ...
			point_normal_map_t::iterator point = point_normal_map.begin();
			edge_count_map_t::const_iterator count = edge_count_map.begin();
			for(; point != point_normal_map.end() && count != edge_count_map.end(); ++point, ++count)
			{
				if(count->second != 0)
					point->second /= count->second;
			}

			// Now, store normals for every edge (i.e. facevarying) ... either from the face as a whole, or the average of all adjacent faces ...
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					set_normal(*edge, point_normal_map[edge->vertex], edge_normal_map[edge], edge->facevarying_data);
					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}
		}
*/
	}

/*
	template<typename T>
	void set_normal(T& Object, const k3d::ri::normal& NewNormal, const k3d::ri::normal& DefaultNormal, k3d::legacy::parameters_t& Parameters)
	{
		if(Object.selection_weight)
			Parameters["N"] = NewNormal;
		else if(0 == Parameters.count("N"))
			Parameters["N"] = DefaultNormal;
	}
*/

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<smooth_normals,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xa6d565ee, 0x6b4a065d, 0x2430ca88, 0xb0bd88a1),
				"SmoothNormals",
				_("Simulates smooth geometry by averaging polygon normals"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
};

/////////////////////////////////////////////////////////////////////////////
// smooth_normals_factory

k3d::iplugin_factory& smooth_normals_factory()
{
	return smooth_normals::get_factory();
}

} // namespace mesh_attributes

} // namespace module

