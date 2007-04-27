// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/new_mesh_deformation_modifier.h>

namespace libk3ddeformation
{

/////////////////////////////////////////////////////////////////////////////
// smooth_points

class smooth_points :
	public k3d::dev::mesh_deformation_modifier
{
	typedef k3d::dev::mesh_deformation_modifier base;

public:
	smooth_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_smoothing(init_owner(*this) + init_name("smoothing") + init_label(_("Smoothing")) + init_description(_("Smoothing factor")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_smoothing.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::dev::mesh& Input, const k3d::dev::mesh::points_t& InputPoints, const k3d::dev::mesh::selection_t& PointSelection, k3d::dev::mesh::points_t& OutputPoints)
	{
		const double smoothing = m_smoothing.value();

        k3d::dev::mesh::points_t sums(InputPoints.size(), k3d::point3(0, 0, 0));
        std::vector<size_t> counts(InputPoints.size(), 0);

        if(Input.polyhedra && Input.polyhedra->edge_points && Input.polyhedra->clockwise_edges)
        {
            const k3d::dev::mesh::indices_t& edge_points = *Input.polyhedra->edge_points;
            const k3d::dev::mesh::indices_t& clockwise_edges = *Input.polyhedra->clockwise_edges;

            const size_t edge_begin = 0;
            const size_t edge_end = edge_begin + edge_points.size();
            for(size_t edge = edge_begin; edge != edge_end; ++edge)
            {
                sums[edge_points[edge]] += InputPoints[edge_points[clockwise_edges[edge]]];
                counts[edge_points[edge]] += 1;

                sums[edge_points[clockwise_edges[edge]]] += InputPoints[edge_points[edge]];
                counts[edge_points[clockwise_edges[edge]]] += 1;
            }
        }

        const size_t point_begin = 0;
        const size_t point_end = point_begin + OutputPoints.size();

        for(size_t point = point_begin; point != point_end; ++point)
        {
            if(counts[point])
                sums[point] /= counts[point];
            else
                sums[point] = InputPoints[point];
        }

        for(size_t point = point_begin; point != point_end; ++point)
            OutputPoints[point] = k3d::mix(InputPoints[point], sums[point], smoothing * PointSelection[point]);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<smooth_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x3d6d1c61, 0x706f4968, 0x93462489, 0x9a3134a7),
				"SmoothPoints",
				_("Smooths mesh points by averaging their positions with their neighbors'"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_smoothing;
};

/////////////////////////////////////////////////////////////////////////////
// smooth_points_factory

k3d::iplugin_factory& smooth_points_factory()
{
	return smooth_points::get_factory();
}

} // namespace libk3ddeformation
