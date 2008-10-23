#ifndef MODULES_DEFORMATION_LINEAR_TRANSFORMATION_WORKER_H
#define MODULES_DEFORMATION_LINEAR_TRANSFORMATION_WORKER_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include <k3dsdk/algebra.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/parallel/blocked_range.h>
#include <k3dsdk/parallel/parallel_for.h>
#include <k3dsdk/parallel/threads.h>


namespace module
{

namespace deformation
{

/// Helper class that can apply a linear transformation to a collection of points.
/// Designed for compatibility with k3d::parallel::parallel_for().
class linear_transformation_worker
{
public:

	linear_transformation_worker(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints, const k3d::matrix4& Transformation) :
		input_points(InputPoints),
		point_selection(PointSelection),
		output_points(OutputPoints),
		transformation(Transformation)
	{
	}
	
	void operator()(const k3d::parallel::blocked_range<k3d::uint_t>& range) const
	{
		const k3d::uint_t point_begin = range.begin();
		const k3d::uint_t point_end = range.end();
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
			output_points[point] = k3d::mix(input_points[point], transformation * input_points[point], point_selection[point]);
	}

private:
	const k3d::mesh::points_t& input_points;
	const k3d::mesh::selection_t& point_selection;
	k3d::mesh::points_t& output_points;
	const k3d::matrix4& transformation;
};

} // namespace deformation

} // namespace module

#endif // !MODULES_DEFORMATION_LINEAR_TRANSFORMATION_WORKER_H
