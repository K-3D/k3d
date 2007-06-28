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
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>

#include <list>
#include <set>

namespace libk3dmesh
{

namespace detail
{

typedef std::set<k3d::legacy::point*> point_set_t;

/// Removes used k3d::legacy::point* elements from PointSet
class remove_used_points
{
public:
	remove_used_points(point_set_t& PointSet) :
		point_set(PointSet)
	{
	}

	void operator()(k3d::legacy::point&) { }
	void operator()(k3d::legacy::face&) { }

	void operator()(k3d::legacy::split_edge& Edge)
	{
		if(!point_set.size())
			return;

		point_set_t::iterator point = point_set.find(Edge.vertex);
		if(point != point_set.end())
			point_set.erase(point);
	}

	void operator()(k3d::legacy::linear_curve& Curve)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::linear_curve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
		{
			point_set_t::iterator point = point_set.find(*curve_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::linear_curve_group&) { }

	void operator()(k3d::legacy::cubic_curve& Curve)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::cubic_curve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
		{
			point_set_t::iterator point = point_set.find(*curve_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::cubic_curve_group&) { }

	void operator()(k3d::legacy::nucurve& Curve)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::nucurve::control_points_t::iterator curve_point = Curve.control_points.begin(); curve_point != Curve.control_points.end(); ++curve_point)
		{
			point_set_t::iterator point = point_set.find(curve_point->position);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::nucurve_group&) { }

	void operator()(k3d::legacy::bilinear_patch& Patch)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::bilinear_patch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
		{
			point_set_t::iterator point = point_set.find(*control_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::bicubic_patch& Patch)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::bicubic_patch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
		{
			point_set_t::iterator point = point_set.find(*control_point);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::nupatch& Patch)
	{
		if(!point_set.size())
			return;

		for(k3d::legacy::nupatch::control_points_t::iterator control_point = Patch.control_points.begin(); control_point != Patch.control_points.end(); ++control_point)
		{
			point_set_t::iterator point = point_set.find(control_point->position);
			if(point != point_set.end())
				point_set.erase(point);
		}
	}

	void operator()(k3d::legacy::polyhedron&) { }
	void operator()(k3d::legacy::mesh&) { }

private:
	point_set_t& point_set;
};

/// Returns whether k3d::legacy::point* is in given set
struct is_in_point_set
{
	is_in_point_set(point_set_t& PointSet) :
		point_set(PointSet)
	{
	}

	bool operator()(k3d::legacy::point* Point)
	{
		point_set_t::const_iterator point = point_set.find(Point);
		if(point != point_set.end())
			return true;

		return false;
	}

	point_set_t& point_set;
};

/// Deletes selected components and their parent components
struct delete_components
{
	delete_components(k3d::legacy::mesh& Mesh) :
		mesh(Mesh)
	{
	}

	void operator()(k3d::legacy::polyhedron* Polyhedron)
	{
		k3d::legacy::polyhedron& polyhedron = *Polyhedron;

		// Find selected faces and save touched points
		point_set_t touched_points;
		for(k3d::legacy::polyhedron::faces_t::iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); ++face)
		{
			// Find selected faces and faces containing selected edges or vertices
			bool delete_face = false;
			if((*face)->selection_weight)
				delete_face = true;
			else
			{
				// Check for selected edges
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					// If edge (or its companion) or its vertex is selected, delete face
					if(edge->selection_weight || (edge->companion && edge->companion->selection_weight) || edge->vertex->selection_weight)
					{
						delete_face = true;
						break;
					}

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}

			if(!delete_face)
				continue;

			// Save touched points and select companion edges
			k3d::legacy::split_edge* edge = (*face)->first_edge;
			do
			{
				touched_points.insert(edge->vertex);

				// Make sure companion is selected (edge selection doesn't currently select companion)
				if(edge->companion && edge->selection_weight)
					edge->companion->selection_weight = 1.0;

				edge = edge->face_clockwise;
			}
			while(edge != (*face)->first_edge);

			// Save holes touched points
			for(k3d::legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
			{
				for(k3d::legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
				{
					touched_points.insert(edge->vertex);

					if(edge->face_clockwise == (*hole))
						break;
				}
			}

			// Delete face
			delete *face;
			*face = 0;
		}

		// Erase deleted faces
		polyhedron.faces.erase(std::remove_if(polyhedron.faces.begin(), polyhedron.faces.end(), std::bind2nd(std::equal_to<k3d::legacy::face*>(), (k3d::legacy::face*)0)), polyhedron.faces.end());

		// Delete floating points
		k3d::legacy::for_each_component(mesh, remove_used_points(touched_points));
		if(touched_points.size())
		{
			mesh.points.erase(std::remove_if(mesh.points.begin(), mesh.points.end(), is_in_point_set(touched_points)), mesh.points.end());
			std::for_each(touched_points.begin(), touched_points.end(), k3d::delete_object());
		}

		// Sanity check
		assert_warning(is_valid(polyhedron));
	}

	k3d::legacy::mesh& mesh;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// delete_components

class delete_components :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	delete_components(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		k3d::replace_selection(m_mesh_selection.value(), Mesh);

		// Delete faces, edges and points
		std::for_each(Mesh.polyhedra.begin(), Mesh.polyhedra.end(), detail::delete_components(Mesh));

		// Delete patches and points
		detail::point_set_t touched_points;

		// Delete bilinear patches
		for(k3d::legacy::mesh::bilinear_patches_t::iterator patch = Mesh.bilinear_patches.begin(); patch != Mesh.bilinear_patches.end(); ++patch)
		{
			// Find selected patches and patches containing selected points
			bool delete_patch = false;
			if((*patch)->selection_weight)
				delete_patch = true;
			else
			{
				for(k3d::legacy::bilinear_patch::control_points_t::iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				{
					if((*point)->selection_weight)
					{
						delete_patch = true;
						break;
					}
				}
			}

			if(!delete_patch)
				continue;

			// Save touched points and select companion edges
			for(k3d::legacy::bilinear_patch::control_points_t::iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				touched_points.insert(*point);

			delete *patch;
			*patch = 0;
		}
		// Erase deleted patches
		Mesh.bilinear_patches.erase(std::remove_if(Mesh.bilinear_patches.begin(), Mesh.bilinear_patches.end(), std::bind2nd(std::equal_to<k3d::legacy::bilinear_patch*>(), (k3d::legacy::bilinear_patch*)0)), Mesh.bilinear_patches.end());


		// Delete bicubic patches
		for(k3d::legacy::mesh::bicubic_patches_t::iterator patch = Mesh.bicubic_patches.begin(); patch != Mesh.bicubic_patches.end(); ++patch)
		{
			// Find selected patches and patches containing selected points
			bool delete_patch = false;
			if((*patch)->selection_weight)
				delete_patch = true;
			else
			{
				for(k3d::legacy::bicubic_patch::control_points_t::iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				{
					if((*point)->selection_weight)
					{
						delete_patch = true;
						break;
					}
				}
			}

			if(!delete_patch)
				continue;

			// Save touched points and select companion edges
			for(k3d::legacy::bicubic_patch::control_points_t::iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				touched_points.insert(*point);

			delete *patch;
			*patch = 0;
		}
		// Erase deleted patches
		Mesh.bicubic_patches.erase(std::remove_if(Mesh.bicubic_patches.begin(), Mesh.bicubic_patches.end(), std::bind2nd(std::equal_to<k3d::legacy::bicubic_patch*>(), (k3d::legacy::bicubic_patch*)0)), Mesh.bicubic_patches.end());


		// Delete nupatches
		for(k3d::legacy::mesh::nupatches_t::iterator patch = Mesh.nupatches.begin(); patch != Mesh.nupatches.end(); ++patch)
		{
			// Find selected patches and patches containing selected points
			bool delete_patch = false;
			if((*patch)->selection_weight)
				delete_patch = true;
			else
			{
				for(k3d::legacy::nupatch::control_points_t::iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				{
					if(point->position->selection_weight)
					{
						delete_patch = true;
						break;
					}
				}
			}

			if(!delete_patch)
				continue;

			// Save touched points and select companion edges
			for(k3d::legacy::nupatch::control_points_t::iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				touched_points.insert(point->position);

			delete *patch;
			*patch = 0;
		}
		// Erase deleted patches
		Mesh.nupatches.erase(std::remove_if(Mesh.nupatches.begin(), Mesh.nupatches.end(), std::bind2nd(std::equal_to<k3d::legacy::nupatch*>(), (k3d::legacy::nupatch*)0)), Mesh.nupatches.end());


		// Delete floating points
		k3d::legacy::for_each_component(Mesh, detail::remove_used_points(touched_points));
		if(touched_points.size())
		{
			Mesh.points.erase(std::remove_if(Mesh.points.begin(), Mesh.points.end(), detail::is_in_point_set(touched_points)), Mesh.points.end());
			std::for_each(touched_points.begin(), touched_points.end(), k3d::delete_object());
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<delete_components,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xc98c37d5, 0xa50c43c7, 0xb88c3687, 0x73cd3c4d),
				"Delete",
				"Deletes selected faces, edges and vertices",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// delete_components_factory

k3d::iplugin_factory& delete_components_factory()
{
	return delete_components::get_factory();
}

} // namespace libk3dmesh

