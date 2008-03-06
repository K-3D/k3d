// K-3D
// Copyright (c) 2006, Romain Behar
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "linear_transformation_worker.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// transform_points

class transform_points :
	public k3d::transformable<k3d::mesh_simple_deformation_modifier>
{
	typedef k3d::transformable<k3d::mesh_simple_deformation_modifier> base;

public:
	transform_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_input_matrix.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::matrix4 transformation = m_input_matrix.pipeline_value();

		k3d::parallel::parallel_for(
			k3d::parallel::blocked_range<k3d::uint_t>(0, OutputPoints.size(), k3d::parallel::grain_size()),
			linear_transformation_worker(InputPoints, PointSelection, OutputPoints, transformation));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<transform_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > > > factory(
				k3d::uuid(0x8543cf2d, 0x170c4edc, 0x9bf821bd, 0xe8567b6f),
				"TransformPoints",
				_("Transform mesh points using input matrix"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
};

/////////////////////////////////////////////////////////////////////////////
// transform_points_factory

k3d::iplugin_factory& transform_points_factory()
{
	return transform_points::get_factory();
}

} // namespace deformation

} // namespace module

