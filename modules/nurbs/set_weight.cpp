// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
	\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>

namespace module
{

namespace nurbs
{

class set_weight :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	set_weight(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_weight(init_owner(*this) + init_name("weight") + init_label(_("Weight")) + init_description(_("The new weight for the selected points")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_weight.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		if(!Output.points.get())
			return;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		const k3d::double_t weight = m_weight.pipeline_value();
		const k3d::uint_t primitives_count = Output.primitives.size();
		for(k3d::uint_t prim_idx = 0; prim_idx != primitives_count; ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_patch::primitive> patch_prim(k3d::nurbs_patch::validate(Output, Output.primitives[prim_idx]));
			if(patch_prim)
			{
				for(k3d::uint_t patch = 0; patch != patch_prim->patch_first_points.size(); ++patch)
				{
					const k3d::uint_t points_begin = patch_prim->patch_first_points[patch];
					const k3d::uint_t points_end = points_begin + patch_prim->patch_u_point_counts[patch] * patch_prim->patch_v_point_counts[patch];
					for(k3d::uint_t i = points_begin; i != points_end; ++i)
					{
						patch_prim->patch_point_weights[i] = k3d::mix(patch_prim->patch_point_weights[i], weight, Output.point_selection->at(patch_prim->patch_points[i]));
					}
				}
			}
			else
			{
				boost::scoped_ptr<k3d::nurbs_curve::primitive> curve_prim(k3d::nurbs_curve::validate(Output, Output.primitives[prim_idx]));
				if(curve_prim)
				{
					for(k3d::uint_t curve = 0; curve != curve_prim->curve_first_points.size(); ++curve)
					{
						const k3d::uint_t points_begin = curve_prim->curve_first_points[curve];
						const k3d::uint_t points_end = points_begin + curve_prim->curve_point_counts[curve];
						for(k3d::uint_t i = points_begin; i != points_end; ++i)
						{
							curve_prim->curve_point_weights[i] = k3d::mix(curve_prim->curve_point_weights[i], weight, Output.point_selection->at(curve_prim->curve_points[i]));
						}
					}
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<set_weight, k3d::interface_list<k3d::imesh_source , k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x6e08996d, 0xde494dd8, 0xb80bc3b0, 0x4d029bad),
		  "NurbsSetWeight",
		  _("Sets the weight of the selected Control Points"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, no_serialization) m_weight;
};

k3d::iplugin_factory& set_weight_factory()
{
	return set_weight::get_factory();
}

} //namespace nurbs

} //namespace module

