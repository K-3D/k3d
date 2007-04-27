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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/utility.h>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// weld

class weld :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	weld(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("Distance")) + init_description(_("Maximum distance between points")) + init_value(0.001) + init_step_increment(0.0001) + init_units(typeid(k3d::measurement::distance)) + init_constraint(constraint::minimum(0.0)))
	{
		m_distance.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	typedef std::map<k3d::legacy::point*, k3d::legacy::point*> point_map_t;
	typedef std::vector<k3d::legacy::split_edge*> boundary_edges_t;

	class get_boundary_edges_t
	{
	public:
		get_boundary_edges_t(boundary_edges_t& BoundaryEdges) :
			boundary_edges(BoundaryEdges)
		{
		}

		void operator()(k3d::legacy::split_edge& edge)
		{
			if(edge.vertex && edge.face_clockwise && !edge.companion)
				boundary_edges.push_back(&edge);
		}

	private:
		boundary_edges_t& boundary_edges;
	};
	
	class remap_points_t
	{
	public:
		remap_points_t(point_map_t& PointMap) :
			point_map(PointMap)
		{
		}

		void operator()(k3d::legacy::mesh&) {}
		void operator()(k3d::legacy::point&) {}
		void operator()(k3d::legacy::polyhedron&) {}
		void operator()(k3d::legacy::linear_curve_group&) {}
		void operator()(k3d::legacy::cubic_curve_group&) {}
		void operator()(k3d::legacy::nucurve_group&) {}
		void operator()(k3d::legacy::face& face) {}

		void operator()(k3d::legacy::split_edge& edge)
		{
			edge.vertex = point_map[edge.vertex];
		}
		
		void operator()(k3d::legacy::linear_curve& curve)
		{
			for(size_t i = 0; i != curve.control_points.size(); ++i)
				curve.control_points[i] = point_map[curve.control_points[i]];
		}

		void operator()(k3d::legacy::cubic_curve& curve)
		{
			for(size_t i = 0; i != curve.control_points.size(); ++i)
				curve.control_points[i] = point_map[curve.control_points[i]];
		}

		void operator()(k3d::legacy::nucurve& curve)
		{
			for(size_t i = 0; i != curve.control_points.size(); ++i)
				curve.control_points[i].position = point_map[curve.control_points[i].position];
		}
		
		void operator()(k3d::legacy::bilinear_patch& patch)
		{
			for(size_t i = 0; i != 4; ++i)
				patch.control_points[i] = point_map[patch.control_points[i]];
		}

		void operator()(k3d::legacy::bicubic_patch& patch)
		{
			for(size_t i = 0; i != 16; ++i)
				patch.control_points[i] = point_map[patch.control_points[i]];
		}

		void operator()(k3d::legacy::nupatch& patch)
		{
			for(size_t i = 0; i != patch.control_points.size(); ++i)
				patch.control_points[i].position = point_map[patch.control_points[i].position];
		}

	private:
		point_map_t& point_map;
	};

	void on_create_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const double distance = m_distance.value();

		// Begin with a copy of the original mesh ...
		k3d::legacy::deep_copy(InputMesh, Mesh);
//k3d::log() << debug << "original point count: " << Mesh.points.size() << std::endl;
		
		// Get a list of "boundary" edges (edges without a neighbor) ...
		boundary_edges_t boundary_edges;
		get_boundary_edges_t get_boundary_edges(boundary_edges);
		k3d::legacy::for_each_edge(Mesh, get_boundary_edges);
//k3d::log() << debug << "original boundary edges: " << boundary_edges.size() << std::endl;

		// Create an identity mapping from each point to itself ...
		point_map_t point_map;
		point_map.insert(std::make_pair(static_cast<k3d::legacy::point*>(0), static_cast<k3d::legacy::point*>(0)));
		for(k3d::legacy::mesh::points_t::iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
			point_map.insert(std::make_pair(*point, *point));

		// Create a list of points to be eliminated ...
		for(boundary_edges_t::iterator edge1 = boundary_edges.begin(); edge1 != boundary_edges.end(); ++edge1)
		{
			if(point_map[(*edge1)->vertex] != (*edge1)->vertex)
				continue;

			for(boundary_edges_t::iterator edge2 = edge1 + 1; edge2 != boundary_edges.end(); ++edge2)
			{
				if(point_map[(*edge2)->vertex] != (*edge2)->vertex)
					continue;

				const k3d::point3 delta = (*edge2)->vertex->position - (*edge1)->vertex->position;
				if(std::fabs(delta[0]) < distance && std::fabs(delta[1]) < distance && std::fabs(delta[2]) < distance)
				{
					point_map[(*edge2)->vertex] = (*edge1)->vertex;
				}
			}
		}

		// Get rid of dangling references to the points that will be erased ...
		remap_points_t remap_points(point_map);
		k3d::legacy::for_each_component(Mesh, remap_points);

		// Stitch-together boundary edges that have been eliminated ...
//size_t stitched_edge_count = 0;
		for(boundary_edges_t::iterator edge1 = boundary_edges.begin(); edge1 != boundary_edges.end(); ++edge1)
		{
			k3d::legacy::split_edge& e1 = **edge1;
			for(boundary_edges_t::iterator edge2 = edge1 + 1; edge2 != boundary_edges.end(); ++edge2)
			{
				k3d::legacy::split_edge& e2 = **edge2;
				if(
					e1.vertex == e2.face_clockwise->vertex &&
					e2.vertex == e1.face_clockwise->vertex &&
					e1.companion == 0 &&
					e2.companion == 0)
				{
					k3d::legacy::join_edges(e1, e2);
//++stitched_edge_count;
				}
			}
		}
//k3d::log() << debug << "stitched edges: " << stitched_edge_count << std::endl;

		
		// Delete leftover unused points ...
		for(k3d::legacy::mesh::points_t::iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
		{
			if(point_map[*point] != (*point))
			{
				delete *point;
				*point = 0;
			}
		}
		Mesh.points.erase(std::remove(Mesh.points.begin(), Mesh.points.end(), static_cast<k3d::legacy::point*>(0)), Mesh.points.end());
//k3d::log() << debug << "new point count: " << Mesh.points.size() << std::endl;
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<weld,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xacfb8148, 0x4f404ca1, 0x937564e3, 0x2977984c),
				"Weld",
				_("Eliminates points with duplicate 3D coordinates"),
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_distance;
};

/////////////////////////////////////////////////////////////////////////////
// weld_factory

k3d::iplugin_factory& weld_factory()
{
	return weld::get_factory();
}

} // namespace libk3dmesh

