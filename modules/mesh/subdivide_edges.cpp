// K-3D
// Copyright (c) 2005, Romain Behar
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
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <set>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// subdivide_edges

class subdivide_edges :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	subdivide_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_vertices(init_owner(*this) + init_name("vertices") + init_label(_("Vertices")) + init_description(_("Number of vertices to insert in each selected edge")) + init_value(1L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1)))
	{
		m_vertices.changed_signal().connect(make_reset_mesh_slot());
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		const double vertices = static_cast<double>(m_vertices.pipeline_value());

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			// Get selected egdes
			edge_list_t selected_edges;
			subdivide_edges::for_each_edge(**polyhedron, get_edges(selected_edges, true));

			// If no edge is selected, subdivide all of them
			if(!selected_edges.size())
				subdivide_edges::for_each_edge(**polyhedron, get_edges(selected_edges, false));

			// Subdivide edges
			std::for_each(selected_edges.begin(), selected_edges.end(), subdivide_edges::subdivide(vertices, Mesh));
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
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


	typedef std::vector<k3d::legacy::split_edge*> edge_list_t;

	/// Function template in the spirit of std::for_each that applies a functor to every edge in a polyhedron
	template<typename T>
	void for_each_edge(k3d::legacy::polyhedron& Polyhedron, T Functor)
	{
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
		{
			k3d::legacy::split_edge* edge = (*face)->first_edge;
			do
			{
				Functor(*edge);

				edge = edge->face_clockwise;
			}
			while(edge != (*face)->first_edge);
		}
	}

	/// Functor that gets selected edges, and only one from a companion pair
	struct get_edges
	{
		get_edges(edge_list_t& EdgeList, const bool Selected) :
			edge_list(EdgeList),
			selected(Selected)
		{
		}

		void operator()(k3d::legacy::split_edge& Edge)
		{
			if(selected && !Edge.selection_weight)
				return;

			// Test whether companion is already in the set
			if(Edge.companion)
			{
				if(edge_set.count(Edge.companion))
					return;
			}

			// Save edge
			edge_set.insert(&Edge);
			edge_list.push_back(&Edge);
		}

		std::set<k3d::legacy::split_edge*> edge_set;
		edge_list_t& edge_list;
		const bool selected;
	};

	/// Does the actual work of subdivision
	struct subdivide
	{
		subdivide(const double Subdivisions, k3d::legacy::mesh& Mesh) :
			subdivisions(Subdivisions),
			mesh(Mesh)
		{
		}

		void operator()(k3d::legacy::split_edge* Edge)
		{
			// Unselect edge
			Edge->selection_weight = 0;

			const k3d::point3 start_point = Edge->vertex->position;
			const k3d::point3 end_point = Edge->face_clockwise->vertex->position;

			// Create new points
			typedef std::vector<k3d::legacy::point*> points_t;
			points_t new_points;
			typedef std::vector<k3d::legacy::split_edge*> edges_t;
			edges_t new_edges;

			new_edges.push_back(Edge);

			const k3d::vector3 step = (end_point - start_point) / (subdivisions + 1);

			k3d::legacy::split_edge* edge = Edge;
			for(double n = 0; n < subdivisions; ++n)
			{
				const k3d::point3 new_position = start_point + (n + 1)*step;
				k3d::legacy::point* new_point = new k3d::legacy::point(new_position);
				mesh.points.push_back(new_point);

				new_points.push_back(new_point);

				// Select resulting points
				new_point->selection_weight = 1.0;

				k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(new_point);
				new_edge->face_clockwise = edge->face_clockwise;
				edge->face_clockwise = new_edge;

				new_edges.push_back(new_edge);

				edge = new_edge;
			}

			if(!Edge->companion)
				return;

			// Subdivide companion
			edge = Edge->companion;
			edge->selection_weight = 0;

			new_points.push_back(edge->vertex);

			points_t::reverse_iterator point = new_points.rbegin();
			edges_t::reverse_iterator companion = new_edges.rbegin();
			for(; point != new_points.rend(); ++point, ++companion)
			{
				k3d::legacy::split_edge* new_edge;

				if(point == new_points.rbegin())
				{
					new_edge = edge;
				}
				else
				{
					new_edge = new k3d::legacy::split_edge(*point);
					new_edge->face_clockwise = edge->face_clockwise;
					edge->face_clockwise = new_edge;
				}

				k3d::legacy::join_edges(*new_edge, **companion);

				edge = new_edge;
			}
		}

		const double subdivisions;
		k3d::legacy::mesh& mesh;
	};
};

/////////////////////////////////////////////////////////////////////////////
// subdivide_edges_factory

k3d::iplugin_factory& subdivide_edges_factory()
{
	return subdivide_edges::get_factory();
}

} // namespace mesh

} // namespace module

