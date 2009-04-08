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

#include "nurbs_patch_modifier.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{
class add_trim_curve :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	add_trim_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_flip(init_owner(*this) + init_name(_("flip")) + init_label(_("FlipTrimmingCurve")) + init_description(_("A trimming curve erases everything that lies left of it when traversing from start to end, if you want it to cut the other side then enable this")) + init_value(false)),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the curve which was used to create the trimming curve")) + init_value(false)),
		m_patch(init_owner(*this) + init_name("patch") + init_label(_("Patch Number")) + init_description(_("The patch which will be trimmed")) + init_value(0) + init_constraint(constraint::minimum(0)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_curve(init_owner(*this) + init_name("curve") + init_label(_("Curve Number")) + init_description(_("The trimming curve")) + init_value(0) + init_constraint(constraint::minimum(0)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_scale(init_owner(*this) + init_name(_("scale")) + init_label(_("Scale the TrimCurve")) + init_description(_("The coordinates of the trim curve need to be in the interval [0,1]. When being created these are scaled to fit in, but you can adjust the scaling here")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0)) + init_value(1.0)),
		m_offset_u(init_owner(*this) + init_name(_("offset_u")) + init_label(_("TrimCurve offset in u direction")) + init_description(_("If you scaled down your trim curve you might need to position it on the surface")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(-1.0 , constraint::maximum(1.0))) + init_value(0.0)),
		m_offset_v(init_owner(*this) + init_name(_("offset_v")) + init_label(_("TrimCurve offset in v direction")) + init_description(_("If you scaled down your trim curve you might need to position it on the surface")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(-1.0 , constraint::maximum(1.0))) + init_value(0.0))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_flip.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
		m_patch.changed_signal().connect(make_update_mesh_slot());
		m_curve.changed_signal().connect(make_update_mesh_slot());
		m_scale.changed_signal().connect(make_update_mesh_slot());
		m_offset_u.changed_signal().connect(make_update_mesh_slot());
		m_offset_v.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		boost::scoped_ptr<k3d::nurbs_patch::primitive> nurbs(get_first_nurbs_patch(Output));
		boost::scoped_ptr<k3d::nurbs_curve::primitive> curves(get_first_nurbs_curve(Output));
		if(!nurbs || !curves)
			return;

		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);
		nurbs_patch_modifier mod(Output, *nurbs);
		
		
		nurbs_curve_modifier mod2(Output, *curves);

		int my_patch = m_patch.pipeline_value();
		int my_curve = m_curve.pipeline_value();

		if (my_patch >= mod.get_patch_count() || my_curve >= mod2.count_all_curves_in_groups())
		{
			k3d::log() << error << nurbs_debug << "The index to the patch or curve exceeds the number of existing ones!" << std::endl;
			return;
		}

		if (my_patch < 0 || my_curve < 0)
		{
			k3d::log() << error << nurbs_debug << "There are no patches and curves with negative indices!" << std::endl;
			return;
		}

		mod.select_patch(my_patch);
		mod2.select_curve(my_curve);

		if (m_flip.pipeline_value())
		{
			mod2.flip_curve(my_curve);
		}

		mod.add_trim_curve(my_patch, my_curve, m_scale.pipeline_value(), m_offset_u.pipeline_value(), m_offset_v.pipeline_value());

		if (m_delete_original.pipeline_value())
		{
			mod2.delete_curve(my_curve);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add_trim_curve, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xb76a0952, 0x7c467888, 0x172b1aa9, 0x99f7a455),
		  "NurbsAddTrimCurve",
		  _("Adds a trimming curve to the selected patch"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_flip;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_patch;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_curve;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_scale;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_offset_u;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_offset_v;
};

//Create connect_curve factory
k3d::iplugin_factory& add_trim_curve_factory()
{
	return add_trim_curve::get_factory();
}

}//namespace nurbs
}//namespace module

