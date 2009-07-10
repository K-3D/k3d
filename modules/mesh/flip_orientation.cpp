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
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// flip_orientation_implementation

class flip_orientation_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	flip_orientation_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		k3d::geometry::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				if(!(*face)->selection_weight)
					continue;

				std::vector<k3d::legacy::split_edge*> edges;
				std::vector<k3d::legacy::point*> vertices;
				for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
				{
					edges.push_back(edge);
					vertices.push_back(edge->vertex);
					if(edge->face_clockwise == (*face)->first_edge)
						break;
				}

				for(unsigned long i = 0; i != edges.size(); ++i)
				{
					edges[i]->vertex = vertices[(i+1)%vertices.size()];
					edges[i]->face_clockwise = edges[(i+edges.size()-1)%edges.size()];
				}
			}
		}

		for(k3d::legacy::mesh::bilinear_patches_t::const_iterator patch = Mesh.bilinear_patches.begin(); patch != Mesh.bilinear_patches.end(); ++patch)
		{
			std::swap((*patch)->control_points[0], (*patch)->control_points[1]);
			std::swap((*patch)->control_points[2], (*patch)->control_points[3]);
		}

		for(k3d::legacy::mesh::bicubic_patches_t::const_iterator patch = Mesh.bicubic_patches.begin(); patch != Mesh.bicubic_patches.end(); ++patch)
		{
			std::swap((*patch)->control_points[0], (*patch)->control_points[3]);
			std::swap((*patch)->control_points[1], (*patch)->control_points[2]);
			std::swap((*patch)->control_points[4], (*patch)->control_points[7]);
			std::swap((*patch)->control_points[5], (*patch)->control_points[6]);
			std::swap((*patch)->control_points[8], (*patch)->control_points[11]);
			std::swap((*patch)->control_points[9], (*patch)->control_points[10]);
			std::swap((*patch)->control_points[12], (*patch)->control_points[15]);
			std::swap((*patch)->control_points[13], (*patch)->control_points[14]);
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<flip_orientation_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid( 0x7d95cd84, 0xee5a4ad6, 0x86feab94, 0xcbea91ca),
				"FlipOrientation",
				_("Reverses the orientation of selected polygons"),
				"Polygon",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// flip_orientation_factory

k3d::iplugin_factory& flip_orientation_factory()
{
	return flip_orientation_implementation::get_factory();
}

} // namespace mesh

} // namespace module


