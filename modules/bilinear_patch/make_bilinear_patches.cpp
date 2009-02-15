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
#include <k3dsdk/bilinear_patch.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace bilinear_patch
{

/////////////////////////////////////////////////////////////////////////////
// make_bilinear_patches

class make_bilinear_patches:
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	make_bilinear_patches(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input));
		if(!polyhedron)
			return;

		Output.points = Input.points;
		Output.point_selection = Input.point_selection;
		Output.vertex_data = Input.vertex_data;

		boost::scoped_ptr<k3d::bilinear_patch::primitive> primitive(k3d::bilinear_patch::create(Output));

		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			std::vector<k3d::uint_t> edges;

			const k3d::uint_t first_edge = polyhedron->loop_first_edges[polyhedron->face_first_loops[face]];
			for(k3d::uint_t edge = first_edge; ;)
			{
				edges.push_back(edge);

				edge = polyhedron->clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}

			if(edges.size() != 4)
				continue;

			primitive->patch_selections.push_back(polyhedron->face_selections[face]);
			primitive->patch_materials.push_back(polyhedron->face_materials[face]);

			primitive->patch_points.push_back(polyhedron->edge_points[edges[0]]);
			primitive->patch_points.push_back(polyhedron->edge_points[edges[1]]);
			primitive->patch_points.push_back(polyhedron->edge_points[edges[3]]); // Bilinear patch control points *aren't* in clockwise order!
			primitive->patch_points.push_back(polyhedron->edge_points[edges[2]]);
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_bilinear_patches,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xe99f3672, 0x6e85457d, 0x9ce1f9dc, 0x6bb71691),
				"MakeBilinearPatches",
				_("Converts input quadrilaterals to bilinear patches"),
				"Patch",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_bilinear_patches_factory

k3d::iplugin_factory& make_bilinear_patches_factory()
{
	return make_bilinear_patches::get_factory();
}

} // namespace bilinear_patch

} // namespace module


