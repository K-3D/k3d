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
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/utility.h>

#include <set>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// join_points_implementation

class join_points_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	join_points_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
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

		// We keep track of the geometry we're going to delete ...
		std::set<k3d::legacy::point*> points;
		std::set<k3d::legacy::face*> faces;
		std::set<k3d::legacy::split_edge*> edges;

		// Get selected points
		k3d::legacy::point* point = 0;
		// sum of opsitions
		k3d::point3 total_position(0.0,0.0,0.0);
		for(k3d::legacy::mesh::points_t::iterator p = Mesh.points.begin(); p != Mesh.points.end(); ++p)
		{
			if((*p)->selection_weight)
			{
				total_position += k3d::to_vector((*p)->position);
				if(!point)
					point = *p;
				else
					points.insert(*p);
			}
		}

		// abort if nothing was selected
		if (points.empty())
			return;

		point->position = total_position / points.size();

		// Delete selected faces, taking their edges along with them ...
		for(k3d::legacy::mesh::polyhedra_t::iterator p = Mesh.polyhedra.begin(); p != Mesh.polyhedra.end(); ++p)
		{
			k3d::legacy::polyhedron& polyhedron = **p;

			for(k3d::legacy::polyhedron::faces_t::iterator f = polyhedron.faces.begin(); f != polyhedron.faces.end(); ++f)
			{
				k3d::legacy::face* face = *f;
				face->selection_weight = 0.0;
				for(k3d::legacy::split_edge* edge = face->first_edge; edge; edge = edge->face_clockwise)
				{
					edge->selection_weight = 0.0;
					if(edge->vertex->selection_weight)
						edges.insert(edge);
					if(edge->face_clockwise == face->first_edge)
						break;
				}
			}
		}

		point->selection_weight = 0.0;

		for(std::set<k3d::legacy::split_edge*>::iterator edge = edges.begin(); edge != edges.end(); ++edge)
			(*edge)->vertex = point;

		for(std::set<k3d::legacy::split_edge*>::iterator e = edges.begin(); e != edges.end(); ++e)
		{
			k3d::legacy::split_edge* edge = *e;
			if(edge->vertex == edge->face_clockwise->vertex)
			{
				edge->selection_weight = 1.0;
			}
		}

		edges.clear();

		// Delete selected edges, updating their owning faces and adjacent edges as needed.
		for(k3d::legacy::mesh::polyhedra_t::iterator p = Mesh.polyhedra.begin(); p != Mesh.polyhedra.end(); ++p)
		{
			k3d::legacy::polyhedron& polyhedron = **p;

			for(k3d::legacy::polyhedron::faces_t::iterator f = polyhedron.faces.begin(); f != polyhedron.faces.end(); )
			{
				k3d::legacy::face* face = *f;

				std::vector<k3d::legacy::split_edge*> remaining_edges;
				for(k3d::legacy::split_edge* edge = face->first_edge; edge; edge = edge->face_clockwise)
				{
					if(edge->selection_weight)
					{
						edges.insert(edge);
					}
					else
					{
						remaining_edges.push_back(edge);
					}

					if(edge->face_clockwise == face->first_edge)
						break;
				}

				// If there aren't any edges left over for this face zap it, and its little holes too ...
				if(remaining_edges.empty())
				{
					f = polyhedron.faces.erase(f);
					faces.insert(face);
					edges.clear();
				}
				else
				{
					// handle holes...
					for(k3d::legacy::face::holes_t::iterator hole = face->holes.begin(); hole != face->holes.end(); )
					{
						std::vector<k3d::legacy::split_edge*> remaining_hole_edges;
						for(k3d::legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
						{
							if(edge->selection_weight)
							{
								edges.insert(edge);
							}
							else
							{
								remaining_hole_edges.push_back(edge);
							}

							if(edge->face_clockwise == *hole)
								break;
						}

					// Fix hole with left-out edges
						if(!remaining_hole_edges.empty())
						{
							*hole = remaining_hole_edges.front();
							k3d::legacy::loop_edges(remaining_hole_edges.begin(), remaining_hole_edges.end());
							++hole;
						}
						else
						{
							hole = face->holes.erase(hole);
						}
					}

					face->first_edge = remaining_edges.front();
					k3d::legacy::loop_edges(remaining_edges.begin(), remaining_edges.end());
					std::for_each(edges.begin(), edges.end(), k3d::delete_object());
					edges.clear();

					++f;
				}
			}
		}

		// Delete points ...
		delete_geometry(Mesh.points, points, is_selected());

		// Delete faces ...
		std::for_each(faces.begin(), faces.end(), k3d::delete_object());

		// Leave the only remaining point selected
		point->selection_weight = 1.0;
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	/// Functor object that returns true iff the given geometry is selected
	struct is_selected
	{
		template<typename T>
		bool operator()(const T* const Geometry)
		{
			return Geometry->selection_weight;
		}

		bool operator()(const k3d::legacy::nucurve::control_point& ControlPoint)
		{
			return ControlPoint.position->selection_weight;
		}

		bool operator()(const k3d::legacy::nupatch::control_point& ControlPoint)
		{
			return ControlPoint.position->selection_weight;
		}
	};

	/// "Deletes" geometry by moving it from the Source container to the Disposal container, if the Functor tests true
	template<typename S, typename D, typename F>
	void delete_geometry(S& Source, D& Disposal, F Functor)
	{
		k3d::copy_if(Source.begin(), Source.end(), std::inserter(Disposal, Disposal.end()), Functor);
		Source.erase(std::remove_if(Source.begin(), Source.end(), Functor), Source.end());
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<join_points_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x915ba4d4, 0xd4154a12, 0x938bec97, 0x60f819f3),
				"JoinPoints",
				"Merge selected points into one point, positioned at the average position of the selected points.",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// join_points_factory

k3d::iplugin_factory& join_points_factory()
{
	return join_points_implementation::get_factory();
}

} // namespace mesh

} // namespace module

