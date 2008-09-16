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

#include <iostream>
#include <vector>

#include "nurbs_patch_modifier.h"

namespace module
{

	namespace nurbs
	{
		class ruled_surface :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			ruled_surface(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_flip(init_owner(*this) + init_name(_("flip")) + init_label(_("flip")) + init_description(_("Flip one of the curves before creating the surface")) + init_value(false) ),
				m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curves")) + init_description(_("Delete the original curves")) + init_value(true) )
			{
			    m_flip.changed_signal().connect(make_update_mesh_slot());
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
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

				std::vector<k3d::uint_t> curves;

				const k3d::uint_t group_begin = 0;
				const k3d::uint_t group_end = group_begin + (*Output.nurbs_curve_groups->first_curves).size();
				for(k3d::uint_t group = group_begin; group != group_end; ++group)
				{
					const k3d::uint_t curve_begin = (*Output.nurbs_curve_groups->first_curves)[group];
					const k3d::uint_t curve_end = curve_begin + (*Output.nurbs_curve_groups->curve_counts)[group];
					for(k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
					{
						if((*Output.nurbs_curve_groups->curve_selection)[curve] > 0.0)
                            curves.push_back(curve);
					}
				}

				if( curves.size() != 2)
				{
					k3d::log() << error << nurbs_debug << "You need to select 2 curves!\n" << std::endl;
				}
				else
				{
                    nurbs_curve_modifier mod(Output);
                    if(m_flip.pipeline_value())
                        mod.flip_curve(curves[1]);
                    mod.ruled_surface(curves[0], curves[1]);

                    if(m_delete_original.pipeline_value())
                    {
                        mod.delete_curve(curves[0]);
                        mod.delete_curve(curves[1]);
                    }
				}

				assert_warning(k3d::validate_nurbs_curve_groups(Output));
				assert_warning(k3d::validate_nurbs_patches(Output));
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<ruled_surface, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x8114ebd9, 0xe74aa420, 0xf7503b98, 0x640f7e0d),
					"NurbsRuledSurface",
					_("Creates a NURBS surface stretched between the 2 curves"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}

		private:
            k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_flip;
            k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
		};

		//Create connect_curve factory
		k3d::iplugin_factory& ruled_surface_factory()
		{
			return ruled_surface::get_factory();
		}

	}//namespace nurbs
} //namespace module

