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

#include "nurbs_curves.h"
#include "nurbs_patches.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{
class create_cap :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	create_cap(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the curve which was used to create the cap")) + init_value(false)),
		m_radial_segments(init_owner(*this) + init_name("radial_segments") + init_label(_("Radial Segments")) + init_description(_("Number of segments in the radial (parametric 'v') direction")) + init_value(1) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
		m_radial_segments.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		const k3d::uint_t prim_count = Input.primitives.size();
		for(k3d::uint_t prim_idx = 0; prim_idx != prim_count; ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves(k3d::nurbs_curve::validate(Output, *Output.primitives[prim_idx]));
			if(curves.get())
			{
				k3d::mesh::bools_t is_in_loop, loop_selections;
				k3d::mesh::indices_t loop_first_curves, next_curves, curve_loops;
				k3d::mesh::points_t loop_centroids;
				find_loops(*Output.points, *curves, is_in_loop, loop_first_curves, curve_loops, next_curves, loop_selections, loop_centroids);
				boost::scoped_ptr<k3d::nurbs_patch::primitive> patches(k3d::nurbs_patch::create(Output));
				for(k3d::uint_t curve = 0; curve != curves->curve_selections.size(); ++curve)
				{
					const k3d::point3 centroid = is_in_loop[curve] ? loop_centroids[curve_loops[curve]] : module::nurbs::centroid(*Output.points, *curves, curve);
					module::nurbs::create_cap(Output, *patches, *Input.points, *curves, curve, centroid, m_radial_segments.pipeline_value());
					patches->patch_materials.push_back(curves->material.back());
				}
			}
		}

		if(m_delete_original.pipeline_value())
		{
			for(k3d::uint_t prim_idx = 0; prim_idx != prim_count; ++prim_idx)
			{
				boost::scoped_ptr<k3d::nurbs_curve::primitive> curves(k3d::nurbs_curve::validate(Output, Output.primitives[prim_idx]));
				if(curves.get())
				{
					for(k3d::uint_t curve = 0; ;)
					{
						if(curves->curve_selections[curve])
							delete_curve(*curves, curve);
						else
							++curve;
						if(curve == curves->curve_selections.size())
							break;
					}
				}
			}
		}
		delete_empty_primitives(Output);
		replace_duplicate_points(Output, Output.primitives.begin() + prim_count, Output.primitives.end());
		k3d::mesh::delete_unused_points(Output);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<create_cap, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x3f8d9448, 0x5d442835, 0x7d4d0f87, 0x4c0e0593),
		  "NurbsCreateCap",
		  _("Creates a NURBS surface filling out the given closed curve"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_radial_segments;
};

//Create connect_curve factory
k3d::iplugin_factory& create_cap_factory()
{
	return create_cap::get_factory();
}

}//namespace nurbs
} //namespace module

