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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/shared_pointer.h>

#include <iostream>
#include <vector>

#include "nurbs_patch_modifier.h"

namespace module
{

	namespace nurbs
	{
		class extrude_curve :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			extrude_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_distance(init_owner(*this) + init_name(_("distance")) + init_label(_("Distance")) + init_description(_("How far to extrude the curve")) + init_step_increment(0.5)+ init_units(typeid(k3d::measurement::scalar)) + init_value(1.0) ),
				m_segments(init_owner(*this) + init_name("segments") + init_label(_("segments")) + init_description(_("Segments")) + init_value(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
				m_along(init_owner(*this) + init_name("along") + init_label(_("Extrude along")) + init_description(_("Axis along which the curve gets extruded")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values())),
				m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the original curve")) + init_value(true) ),
				m_cap(init_owner(*this) + init_name(_("cap")) + init_label(_("Create Caps?")) + init_description(_("Create caps at both ends?")) + init_value(false) )
			{
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
				m_distance.changed_signal().connect(make_update_mesh_slot());
				m_segments.changed_signal().connect(make_update_mesh_slot());
				m_along.changed_signal().connect(make_update_mesh_slot());
				m_cap.changed_signal().connect(make_update_mesh_slot());
				m_delete_original.changed_signal().connect(make_update_mesh_slot());
			}

			void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				Output = Input;
			}

			void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				Output = Input;

				if(!k3d::validate_nurbs_curve_groups(Output))
					return;

				merge_selection(m_mesh_selection.pipeline_value(), Output);


				nurbs_curve_modifier mod(Output);
				int my_curve = mod.selected_curve();

				if(my_curve < 0)
				{
				    k3d::log() << error << nurbs_debug << "You need to select exactly one curve!" << std::endl;
				    return;
				}

                double distance = m_distance.pipeline_value();
                k3d::axis axis = m_along.pipeline_value();
                bool create_cap =  m_cap.pipeline_value();
                int segments = m_segments.pipeline_value();

                k3d::point3 delta;
                switch(axis)
                {
                    case k3d::X:
                        delta = k3d::point3(1.0,0.0,0.0);
                    break;
                    case k3d::Y:
                        delta = k3d::point3(0.0,1.0,0.0);
                    break;
                    case k3d::Z:
                        delta = k3d::point3(0.0,0.0,1.0);
                    break;
                }

                delta = delta * (distance / segments);

                nurbs_curve curve;
                curve.curve_knots.push_back(0);
                for(int i = 0; i <= segments; i++)
                {
                    curve.curve_knots.push_back(i);
                    curve.control_points.push_back(delta * i);
                    curve.curve_point_weights.push_back(1.0);
                }
                curve.curve_knots.push_back(segments);

                int curve_idx = mod.count_all_curves_in_groups();
                mod.add_curve(curve,false);
				mod.traverse_curve(my_curve,curve_idx,create_cap);
                mod.delete_curve(curve_idx);

                if(m_delete_original.pipeline_value())
                {
                    mod.delete_curve(my_curve);
                }

				assert_warning(k3d::validate_nurbs_curve_groups(Output));
				assert_warning(k3d::validate_nurbs_patches(Output));
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<extrude_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x93c6f141, 0x934f333a, 0x8a027ab4, 0x59b1510c),
					"NurbsExtrudeCurve",
					_("Extrudes the selected curve along a coordinate axis and lets you create caps if one of the curves is closed"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}

		private:
            k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_distance;
            k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_along;
            k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap;
            k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
            k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_segments;
		};

		//Create connect_curve factory
		k3d::iplugin_factory& extrude_curve_factory()
		{
			return extrude_curve::get_factory();
		}

	}//namespace nurbs
} //namespace module

