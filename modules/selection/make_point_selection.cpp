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
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/bicubic_patch.h>
#include <k3dsdk/bilinear_patch.h>
#include <k3dsdk/cubic_curve.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/imesh_selection_algorithm.h>
#include <k3dsdk/linear_curve.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// make_point_selection

class make_point_selection :
	public k3d::imesh_selection_algorithm
{
	typedef k3d::imesh_selection_algorithm base;

public:
	const k3d::selection::set create_mesh_selection(const k3d::mesh& Mesh, const k3d::selection::set& CurrentSelection)
	{
    k3d::selection::set results;

		boost::scoped_ptr<k3d::geometry::point_selection::storage> point_selection(k3d::geometry::point_selection::create(results));

    // For each primitive in the mesh ...
		for(k3d::mesh::primitives_t::const_iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p)
		{
      boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **p));
      if(polyhedron)
      {
        // Convert edge selections to point selections ...
        const k3d::uint_t edge_begin = 0;
        const k3d::uint_t edge_end = edge_begin + polyhedron->edge_selections.size();
        for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
        {
          if(!polyhedron->edge_selections[edge])
            continue;

   		    k3d::geometry::point_selection::append(*point_selection, polyhedron->vertex_points[edge], polyhedron->vertex_points[edge]+1, 1.0);
   		    k3d::geometry::point_selection::append(*point_selection, polyhedron->vertex_points[polyhedron->clockwise_edges[edge]], polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]+1, 1.0);
        }

        // Convert face selections to point selections ...
        const k3d::uint_t face_begin = 0;
        const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
        for(k3d::uint_t face = face_begin; face != face_end; ++face)
        {
          if(!polyhedron->face_selections[face])
            continue;

          const k3d::uint_t face_loop_begin = polyhedron->face_first_loops[face];
          const k3d::uint_t face_loop_end = face_loop_begin + polyhedron->face_loop_counts[face];
          for(k3d::uint_t face_loop = face_loop_begin; face_loop != face_loop_end; ++face_loop)
          {
            const k3d::uint_t first_edge = polyhedron->loop_first_edges[face_loop];
            for(k3d::uint_t edge = first_edge; ; )
            {
   		        k3d::geometry::point_selection::append(*point_selection, polyhedron->vertex_points[edge], polyhedron->vertex_points[edge]+1, 1.0);

              edge = polyhedron->clockwise_edges[edge];
              if(edge == first_edge)
                break;
            }
          }
        } 

        continue;
      }

      // Convert linear curve selections to point selections ...
      boost::scoped_ptr<k3d::linear_curve::const_primitive> linear_curve(k3d::linear_curve::validate(Mesh, **p));
      if(linear_curve)
      {
        const k3d::uint_t curve_begin = 0;
        const k3d::uint_t curve_end = curve_begin + linear_curve->curve_first_points.size();
        for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
        {
          if(!linear_curve->curve_selections[curve])
            continue;

          const k3d::uint_t curve_point_begin = linear_curve->curve_first_points[curve];
          const k3d::uint_t curve_point_end = curve_point_begin + linear_curve->curve_point_counts[curve];
          for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
 		        k3d::geometry::point_selection::append(*point_selection, linear_curve->curve_points[curve_point], linear_curve->curve_points[curve_point]+1, 1.0);
        }

        continue;
      }

      // Convert cubic curve selections to point selections ...
      boost::scoped_ptr<k3d::cubic_curve::const_primitive> cubic_curve(k3d::cubic_curve::validate(Mesh, **p));
      if(cubic_curve)
      {
        const k3d::uint_t curve_begin = 0;
        const k3d::uint_t curve_end = curve_begin + cubic_curve->curve_first_points.size();
        for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
        {
          if(!cubic_curve->curve_selections[curve])
            continue;

          const k3d::uint_t curve_point_begin = cubic_curve->curve_first_points[curve];
          const k3d::uint_t curve_point_end = curve_point_begin + cubic_curve->curve_point_counts[curve];
          for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
 		        k3d::geometry::point_selection::append(*point_selection, cubic_curve->curve_points[curve_point], cubic_curve->curve_points[curve_point]+1, 1.0);
        }

        continue;
      }

      // Convert nurbs curve selections to point selections ...
      boost::scoped_ptr<k3d::nurbs_curve::const_primitive> nurbs_curve(k3d::nurbs_curve::validate(Mesh, **p));
      if(nurbs_curve)
      {
        const k3d::uint_t curve_begin = 0;
        const k3d::uint_t curve_end = curve_begin + nurbs_curve->curve_first_points.size();
        for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
        {
          if(!nurbs_curve->curve_selections[curve])
            continue;

          const k3d::uint_t curve_point_begin = nurbs_curve->curve_first_points[curve];
          const k3d::uint_t curve_point_end = curve_point_begin + nurbs_curve->curve_point_counts[curve];
          for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
 		        k3d::geometry::point_selection::append(*point_selection, nurbs_curve->curve_points[curve_point], nurbs_curve->curve_points[curve_point]+1, 1.0);
        }

        continue;
      }

      // Convert bilinear patch selections to point selections ...
      boost::scoped_ptr<k3d::bilinear_patch::const_primitive> bilinear_patch(k3d::bilinear_patch::validate(Mesh, **p));
      if(bilinear_patch)
      {
        const k3d::uint_t patch_begin = 0;
        const k3d::uint_t patch_end = patch_begin + bilinear_patch->patch_points.size() / 4;
        for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
        {
          if(!bilinear_patch->patch_selections[patch])
            continue;

          const k3d::uint_t patch_point_begin = patch * 4;
          const k3d::uint_t patch_point_end = patch_point_begin + 4;
          for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
 		        k3d::geometry::point_selection::append(*point_selection, bilinear_patch->patch_points[patch_point], bilinear_patch->patch_points[patch_point]+1, 1.0);
        }

        continue;
      }

      // Convert bicubic patch selections to point selections ...
      boost::scoped_ptr<k3d::bicubic_patch::const_primitive> bicubic_patch(k3d::bicubic_patch::validate(Mesh, **p));
      if(bicubic_patch)
      {
        const k3d::uint_t patch_begin = 0;
        const k3d::uint_t patch_end = patch_begin + bicubic_patch->patch_points.size() / 16;
        for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
        {
          if(!bicubic_patch->patch_selections[patch])
            continue;

          const k3d::uint_t patch_point_begin = patch * 16;
          const k3d::uint_t patch_point_end = patch_point_begin + 16;
          for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
 		        k3d::geometry::point_selection::append(*point_selection, bicubic_patch->patch_points[patch_point], bicubic_patch->patch_points[patch_point]+1, 1.0);
        }
      }

      // Convert nurbs patch selections to point selections ...
      boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs_patch(k3d::nurbs_patch::validate(Mesh, **p));
      if(nurbs_patch)
      {
        const k3d::uint_t patch_begin = 0;
        const k3d::uint_t patch_end = patch_begin + nurbs_patch->patch_first_points.size();
        for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
        {
          if(!nurbs_patch->patch_selections[patch])
            continue;

          const k3d::uint_t patch_point_begin = nurbs_patch->patch_first_points[patch];
          const k3d::uint_t patch_point_end = patch_point_begin + (nurbs_patch->patch_u_point_counts[patch] * nurbs_patch->patch_v_point_counts[patch]);
          for(k3d::uint_t patch_point = patch_point_begin; patch_point != patch_point_end; ++patch_point)
 		        k3d::geometry::point_selection::append(*point_selection, nurbs_patch->patch_points[patch_point], nurbs_patch->patch_points[patch_point]+1, 1.0);
        }
      }

/*
			// Perform automatic conversion
			for(k3d::mesh::named_tables_t::const_iterator s = (**p).structure.begin(); s != (**p).structure.end(); ++s)
			{
        const k3d::mesh::selection_t* selection = 0;
        const k3d::mesh::indices_t* points = 0;

				for(k3d::mesh::named_arrays_t::const_iterator array = s->second.begin(); array != s->second.end(); ++array)
				{
					if(!selection)
					{
						if(array->second->get_metadata_value(k3d::metadata::key::role()) == k3d::metadata::value::selection_role())
						{
              selection = dynamic_cast<const k3d::mesh::selection_t*>(array->second.get());
						}
					}

					if(!points)
					{
            if(array->second->get_metadata_value(k3d::metadata::key::domain()) == k3d::metadata::value::point_indices_domain())
						{
              points = dynamic_cast<const k3d::mesh::indices_t*>(array->second.get());
						}
					}
				}

				// Select referenced points
				if(selection && points)
				{
					const k3d::uint_t begin = 0;
					const k3d::uint_t end = begin + selection->size();
					for(k3d::uint_t i = begin; i != end; ++i)
					{
						if((*selection)[i])
      		    k3d::geometry::point_selection::append(*point_selection, (*points)[i], (*points)[i]+1, 1.0);
					}
				}
			}
*/
		}

		boost::scoped_ptr<k3d::geometry::primitive_selection::storage> primitive_selection(k3d::geometry::primitive_selection::create(results));
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::CONSTANT, 0.0);
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::CURVE, 0.0);
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::EDGE, 0.0);
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::FACE, 0.0);
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::PATCH, 0.0);
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::SURFACE, 0.0);
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::VARYING, 0.0);
		k3d::geometry::primitive_selection::append(*primitive_selection, k3d::selection::VERTEX, 0.0);

    return results;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<make_point_selection,
			k3d::interface_list<k3d::imesh_selection_algorithm> > factory(
				k3d::uuid(0x0d1fd128, 0xf24f51c9, 0x175b158c, 0xf0a00996),
				"MakePointSelection",
				_("Converts primitive selections to point selections."),
				"Selection",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_point_selection_factory

k3d::iplugin_factory& make_point_selection_factory()
{
	return make_point_selection::get_factory();
}

} // namespace selection

} // namespace module

