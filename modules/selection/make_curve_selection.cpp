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
#include <k3dsdk/cubic_curve.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/imesh_selection_algorithm.h>
#include <k3dsdk/linear_curve.h>
#include <k3dsdk/nurbs_curve.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// make_curve_selection

class make_curve_selection :
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
			// Convert point selections to linear curve selections ...
			boost::scoped_ptr<k3d::linear_curve::const_primitive> linear_curve(k3d::linear_curve::validate(Mesh, **p));
			if(linear_curve)
			{
				const k3d::uint_t curve_begin = 0;
				const k3d::uint_t curve_end = curve_begin + linear_curve->curve_first_points.size();
				for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
				{
					const k3d::uint_t curve_point_begin = linear_curve->curve_first_points[curve];
					const k3d::uint_t curve_point_end = curve_point_begin + linear_curve->curve_point_counts[curve];
					for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
					{
						if(!(*Mesh.point_selection)[linear_curve->curve_points[curve_point]])
							continue;

						k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::CURVE, curve, curve+1, 1.0);
						break;
					}
				}

				continue;
			}

			// Convert point selections to cubic curve selections ...
			boost::scoped_ptr<k3d::cubic_curve::const_primitive> cubic_curve(k3d::cubic_curve::validate(Mesh, **p));
			if(cubic_curve)
			{
				const k3d::uint_t curve_begin = 0;
				const k3d::uint_t curve_end = curve_begin + cubic_curve->curve_first_points.size();
				for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
				{
					const k3d::uint_t curve_point_begin = cubic_curve->curve_first_points[curve];
					const k3d::uint_t curve_point_end = curve_point_begin + cubic_curve->curve_point_counts[curve];
					for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
					{
						if(!(*Mesh.point_selection)[cubic_curve->curve_points[curve_point]])
							continue;
						
						k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::CURVE, curve, curve+1, 1.0);
						break;
					}
				}

				continue;
			}

			// Convert point selections to nurbs curve selections ...
			boost::scoped_ptr<k3d::nurbs_curve::const_primitive> nurbs_curve(k3d::nurbs_curve::validate(Mesh, **p));
			if(nurbs_curve)
			{
				const k3d::uint_t curve_begin = 0;
				const k3d::uint_t curve_end = curve_begin + nurbs_curve->curve_first_points.size();
				for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
				{
					const k3d::uint_t curve_point_begin = nurbs_curve->curve_first_points[curve];
					const k3d::uint_t curve_point_end = curve_point_begin + nurbs_curve->curve_point_counts[curve];
					for(k3d::uint_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
					{
						if(!(*Mesh.point_selection)[nurbs_curve->curve_points[curve_point]])
							continue;
						
						k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::CURVE, curve, curve+1, 1.0);
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
		static k3d::application_plugin_factory<make_curve_selection,
			k3d::interface_list<k3d::imesh_selection_algorithm> > factory(
				k3d::uuid(0x75b8aec4, 0xdc47b9f6, 0x799d8896, 0x16930c05),
				"MakeCurveSelection",
				_("Converts primitive selections to curve selections."),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_curve_selection_factory

k3d::iplugin_factory& make_curve_selection_factory()
{
	return make_curve_selection::get_factory();
}

} // namespace selection

} // namespace module

