// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/bicubic_patch.h>
#include <k3dsdk/bilinear_patch.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/imesh_selection_algorithm.h>
#include <k3dsdk/nurbs_patch.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// make_patch_selection

class make_patch_selection :
	public k3d::imesh_selection_algorithm
{
	typedef k3d::imesh_selection_algorithm base;

public:
	const k3d::selection::set create_mesh_selection(const k3d::mesh& Mesh)
	{
		k3d::selection::set results;

		boost::scoped_ptr<k3d::geometry::primitive_selection::storage> primitive_selection(k3d::geometry::primitive_selection::create(results));

		// For each primitive in the mesh ...
		k3d::uint_t primitive = 0;
		for(k3d::mesh::primitives_t::const_iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p, ++primitive)
		{
			// Convert point selections to bilinear patch selections ...
			boost::scoped_ptr<k3d::bilinear_patch::const_primitive> bilinear_patch(k3d::bilinear_patch::validate(Mesh, **p));
			if(bilinear_patch)
			{
				const k3d::uint_t patch_begin = 0;
				const k3d::uint_t patch_end = patch_begin + bilinear_patch->patch_selections.size();
				for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
				{
					const k3d::uint_t patch_point_begin = patch * 4;
					const k3d::uint_t patch_point_end = patch_point_begin + 4;
					for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
					{
						if(!(*Mesh.point_selection)[bilinear_patch->patch_points[patch_point]])
							continue;

						k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::PATCH, patch, patch+1, 1.0);
						break;
					}
				}

				continue;
			}

			// Convert point selections to cubic patch selections ...
			boost::scoped_ptr<k3d::bicubic_patch::const_primitive> bicubic_patch(k3d::bicubic_patch::validate(Mesh, **p));
			if(bicubic_patch)
			{
				const k3d::uint_t patch_begin = 0;
				const k3d::uint_t patch_end = patch_begin + bicubic_patch->patch_selections.size();
				for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
				{
					const k3d::uint_t patch_point_begin = patch * 16;
					const k3d::uint_t patch_point_end = patch_point_begin + 16;
					for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
					{
						if(!(*Mesh.point_selection)[bicubic_patch->patch_points[patch_point]])
							continue;
						
						k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::PATCH, patch, patch+1, 1.0);
						break;
					}
				}

				continue;
			}

			// Convert point selections to nurbs patch selections ...
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs_patch(k3d::nurbs_patch::validate(Mesh, **p));
			if(nurbs_patch)
			{
				const k3d::uint_t patch_begin = 0;
				const k3d::uint_t patch_end = patch_begin + nurbs_patch->patch_first_points.size();
				for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
				{
					const k3d::uint_t patch_point_begin = nurbs_patch->patch_first_points[patch];
					const k3d::uint_t patch_point_end = patch_point_begin + (nurbs_patch->patch_u_point_counts[patch] * nurbs_patch->patch_v_point_counts[patch]);
					for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
					{
						if(!(*Mesh.point_selection)[nurbs_patch->patch_points[patch_point]])
							continue;
						
						k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::PATCH, patch, patch+1, 1.0);
						break;
					}
				}

				continue;
			}
		}

		return results;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<make_patch_selection,
			k3d::interface_list<k3d::imesh_selection_algorithm> > factory(
				k3d::uuid(0x80b8e0f2, 0x394b9d2f, 0x6a21da90, 0x4399215e),
				"MakePatchSelection",
				_("Converts point selections to patch selections."),
				"Selection",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_patch_selection_factory

k3d::iplugin_factory& make_patch_selection_factory()
{
	return make_patch_selection::get_factory();
}

} // namespace selection

} // namespace module

