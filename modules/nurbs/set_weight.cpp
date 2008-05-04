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
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/shared_pointer.h>

#include <iostream>

namespace module
{
	namespace nurbs
	{

		class set_weight :
			public  k3d::mesh_modifier<k3d::persistent<k3d::node> >
		{
			typedef k3d::mesh_modifier<k3d::persistent<k3d::node> > base;
		public:
			set_weight(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_weight(init_owner(*this) + init_name("weight") + init_label(_("weight")) + init_description(_("The new weight for the selected points")) + init_value(1.0) + init_constraint(constraint::minimum(1.0)) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
			{
				m_weight.changed_signal().connect(make_update_mesh_slot());
			}
			
			void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				Output = Input;
			}
			
			void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				Output = change_weight(Input);			
			}
			
			k3d::mesh change_weight(const k3d::mesh& Mesh)
			{
				k3d::mesh Output=Mesh;
				
				//k3d::deep_copy(Mesh,Output);
				
				//loop through all curves and check for selected points
				const size_t group_begin = 0;
				const size_t group_end = group_begin + (*Output.nurbs_curve_groups->first_curves).size();
				for(size_t group = group_begin; group != group_end; ++group)
				{
					const size_t curve_begin = (*Output.nurbs_curve_groups->first_curves)[group];
					const size_t curve_end = curve_begin + (*Output.nurbs_curve_groups->curve_counts)[group];
					for(size_t curve = curve_begin; curve != curve_end; ++curve)
					{
						const size_t curve_point_begin = (*Output.nurbs_curve_groups->curve_first_points)[curve];
						const size_t curve_point_end = curve_point_begin + (*Output.nurbs_curve_groups->curve_point_counts)[curve];

						const k3d::mesh::weights_t& orig_weights = *(boost::const_pointer_cast<k3d::typed_array<double> >(Output.nurbs_curve_groups->curve_point_weights));
						
						boost::shared_ptr<k3d::mesh::weights_t> curve_point_weights ( new k3d::mesh::weights_t() );
						
						for(size_t curve_point = curve_point_begin; curve_point != curve_point_end; ++curve_point)
						{
							if((*Output.point_selection)[(*Output.nurbs_curve_groups->curve_points)[curve_point]])
							{
								curve_point_weights->push_back(m_weight.pipeline_value());
							}
							else
								curve_point_weights->push_back(orig_weights[curve_point]);
						}
						
						k3d::mesh::nurbs_curve_groups_t& groups = *(boost::const_pointer_cast<k3d::mesh::nurbs_curve_groups_t>(Output.nurbs_curve_groups)).get();
						groups.curve_point_weights=curve_point_weights;
					}
				}
				
				assert_warning(k3d::validate_nurbs_curve_groups(Output));
				assert_warning(k3d::validate(Output));
				
				return Output;
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<set_weight, k3d::interface_list<k3d::imesh_source > > factory(
				k3d::uuid(0x6e08996d, 0xde494dd8, 0xb80bc3b0, 0x4d029bad),
					"NurbsSetWeight",
					_("Sets the weight of the selected Control Points"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}

		private:
			k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, no_serialization) m_weight;
		};

		//Create connect_curve factory
		k3d::iplugin_factory& set_weight_factory()
		{
			return set_weight::get_factory();
		}

	}//namespace nurbs
} //namespace module
