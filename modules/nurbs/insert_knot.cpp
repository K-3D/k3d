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

namespace module
{

	namespace nurbs
	{
		extern void curve_knot_insertion(k3d::mesh& input, size_t curve, double u, size_t r);
		extern int selected_curve(k3d::mesh& Output);
		extern void normalize_knot_vector(k3d::mesh::nurbs_curve_groups_t& groups, k3d::mesh::knots_t& knots, size_t curve);
	
		class insert_knot :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			insert_knot(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_u_value(init_owner(*this) + init_name(_("u_value")) + init_label(_("u_value")) + init_description(_("Insert knot at [0,1]")) + init_step_increment(0.1) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.5) ),
				m_multiplicity(init_owner(*this) + init_name(_("multiplicity")) + init_label(_("multiplicity")) + init_description(_("Multiplicity")) + init_constraint(constraint::minimum(1 , constraint::maximum(3))) + init_value(1) )
			{
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
				m_u_value.changed_signal().connect(make_update_mesh_slot());
				m_multiplicity.changed_signal().connect(make_update_mesh_slot());
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
				
				int curve = selected_curve(Output);
				
				if( curve < 0 )
				{
					k3d::log() << error << "You need to select a curve!" << std::endl;
					return;
				}
				
				double u = m_u_value.pipeline_value();
				int multiplicity = m_multiplicity.pipeline_value();
				
				//call curve_knot_insertion
				
				k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(Output.nurbs_curve_groups);
				k3d::mesh::knots_t& curve_knots = *k3d::make_unique(groups.curve_knots);
				
				normalize_knot_vector(groups,curve_knots,curve);
				curve_knot_insertion(Output, curve, u, multiplicity);
				
				assert_warning(k3d::validate_nurbs_curve_groups(Output));
			}
			
			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<insert_knot, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x37e2f7e9, 0x7c44838e, 0x2227c981, 0x3018ab03),
					"NurbsCurveInsertKnot",
					_("Inserts a new Knot with the specified u-value"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}
		private:
			k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_u_value;
			k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_multiplicity;
		};
		
		//Create connect_curve factory
		k3d::iplugin_factory& insert_knot_factory()
		{
			return insert_knot::get_factory();
		}
		
	}//namespace nurbs
}//namespace module
