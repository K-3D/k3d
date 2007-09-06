// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/mesh_modifier.h>

#include <iterator>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// make_bilinear_patches_implementation

class make_bilinear_patches_implementation :
	public k3d::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	make_bilinear_patches_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!k3d::validate_polyhedra(Input))
			return;

		const k3d::mesh::indices_t& face_first_loops = *Input.polyhedra->face_first_loops;
		const k3d::mesh::selection_t& face_selection = *Input.polyhedra->face_selection;
		const k3d::mesh::materials_t& face_materials = *Input.polyhedra->face_materials;
		const k3d::mesh::indices_t& loop_first_edges = *Input.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Input.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Input.polyhedra->clockwise_edges;

		Output.points = Input.points;
		Output.point_selection = Input.point_selection;
		Output.vertex_data = Input.vertex_data;

		k3d::mesh::bilinear_patches_t* const bilinear_patches = new k3d::mesh::bilinear_patches_t();
		k3d::mesh::selection_t* const patch_selection = new k3d::mesh::selection_t();
		k3d::mesh::materials_t* const patch_materials = new k3d::mesh::materials_t();
		k3d::mesh::indices_t* const patch_points = new k3d::mesh::indices_t();

		const size_t face_begin = 0;
		const size_t face_end = face_begin + face_first_loops.size();
		for(size_t face = face_begin; face != face_end; ++face)
		{
			std::vector<size_t> edges;

			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ;)
			{
				edges.push_back(edge);

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}

			if(edges.size() != 4)
				continue;

			patch_selection->push_back(face_selection[face]);
			patch_materials->push_back(face_materials[face]);

			patch_points->push_back(edge_points[edges[0]]);
			patch_points->push_back(edge_points[edges[1]]);
			patch_points->push_back(edge_points[edges[3]]); // Bilinear patch control points *aren't* in clockwise order!
			patch_points->push_back(edge_points[edges[2]]);
		}

		bilinear_patches->patch_selection.reset(patch_selection);
		bilinear_patches->patch_materials.reset(patch_materials);
		bilinear_patches->patch_points.reset(patch_points);
		Output.bilinear_patches.reset(bilinear_patches);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_bilinear_patches_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xe99f3672, 0x6e85457d, 0x9ce1f9dc, 0x6bb71691),
				"MakeBilinearPatches",
				_("Converts input quadrilaterals to bilinear patches"),
				"Patches",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_bilinear_patches_factory

k3d::iplugin_factory& make_bilinear_patches_factory()
{
	return make_bilinear_patches_implementation::get_factory();
}

} // namespace libk3dmesh


