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
#include <k3dsdk/point4.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>

#include <iostream>
#include <vector>

#include "nurbs_curve_modifier.h"

namespace module
{

	namespace nurbs
	{
		class split_curve :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			split_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_u_value(init_owner(*this) + init_name(_("u_value")) + init_label(_("U value (in [0,1])")) + init_description(_("Split Curve at u in ]0,1[")) + init_step_increment(0.01)+ init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.5) ),
				m_normalize_all(init_owner(*this) + init_name(_("normalize_all")) + init_label(_("Share Degree and KnotVector?")) + init_description(_("Make all selected curves have same Degree and KnotVector?")) + init_value(false) )
			{
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
				m_u_value.changed_signal().connect(make_update_mesh_slot());
				m_normalize_all.changed_signal().connect(make_update_mesh_slot());
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

				std::vector<k3d::uint_t> my_curves = mod.selected_curves();

				if( my_curves.size() == 0 )
				{
					k3d::log() << error << "You need to select at least one curve!" << std::endl;
					return;
				}

				double u = m_u_value.pipeline_value();

                if(my_curves.size() > 1 && m_normalize_all.pipeline_value())
                {
                    mod.knot_vector_adaption(my_curves);
                }

                for(int i = 0; i < my_curves.size(); i++)
                {
                    if(u == 0.0 || u == 1.0)
                    {
                        if(mod.is_closed(my_curves.at(i)+i))
                        {
                            //need to open up curve, double the point..
                            mod.open_up_curve(my_curves.at(i)+i);
                        }
                        else
                        {
                            k3d::log() << error << "Cannot split a curve at its end point" << std::endl;
                            return;
                        }
                    }
                    else
                    {
                        mod.split_curve_at(my_curves.at(i)+i, u, mod.is_closed(my_curves.at(i)+i));
                    }
                }

				assert_warning(k3d::validate_nurbs_curve_groups(Output));
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<split_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9f5aa916, 0x8248cdec, 0x6cfbab9d, 0xd7f5f46f),
					"NurbsSplitCurve",
					_("Breaks a NURBS curve at a given u value (0...1)"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}
		private:
			k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_value;
            k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_normalize_all;

		};

		//Create connect_curve factory
		k3d::iplugin_factory& split_curve_factory()
		{
			return split_curve::get_factory();
		}

	}//namespace nurbs
} //namespace module
