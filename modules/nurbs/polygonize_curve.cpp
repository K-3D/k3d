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
		class polygonize_curve :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			polygonize_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_segments(init_owner(*this) + init_name(_("segments")) + init_label(_("Segments")) + init_description(_("The more segments the better the result")) + init_value(10) + init_constraint(constraint::minimum(3)) ),
				m_delete_orig(init_owner(*this) + init_name(_("delete_orig")) + init_label(_("Delete original?")) + init_description(_("Delete original NURBS curve?")) + init_value(true) )
			{
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
				m_segments.changed_signal().connect(make_update_mesh_slot());
				m_delete_orig.changed_signal().connect(make_update_mesh_slot());
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

				mod.polygonize_curve(my_curve,m_segments.pipeline_value(), m_delete_orig.pipeline_value());

				assert_warning(k3d::validate_nurbs_curve_groups(Output));
				assert_warning(k3d::validate_linear_curve_groups(Output));
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<polygonize_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x24074c75, 0x8d45f52f, 0xbd470ea5, 0x05bc9752),
					"NurbsPolygonizeCurve",
					_("Creates a LinearCurve representation of the selected NURBS curve"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}

		private:
            k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_segments;
            k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_orig;
		};

		//Create connect_curve factory
		k3d::iplugin_factory& polygonize_curve_factory()
		{
			return polygonize_curve::get_factory();
		}

	}//namespace nurbs
} //namespace module

