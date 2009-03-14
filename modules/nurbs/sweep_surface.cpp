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
class sweep_surface :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	sweep_surface(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_swap(init_owner(*this) + init_name(_("swap")) + init_label(_("Swap the curves?")) + init_description(_("Exchanges the profile curve and the traverse curve")) + init_value(false)),
		m_create_caps(init_owner(*this) + init_name(_("create_caps")) + init_label(_("Create caps?")) + init_description(_("Create caps at both ends of the revolved curve?")) + init_value(false)),
		m_segments(init_owner(*this) + init_name(_("segments")) + init_label(_("Segments")) + init_description(_("The more segments the better the result")) + init_value(10) + init_constraint(constraint::minimum(3))),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the original curves")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_create_caps.changed_signal().connect(make_update_mesh_slot());
		m_swap.changed_signal().connect(make_update_mesh_slot());
		m_segments.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		boost::scoped_ptr<k3d::nurbs_curve::primitive> nurbs(k3d::nurbs_curve::validate(Output));
		if(!nurbs)
			return;

		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);

		std::vector<k3d::uint_t> curves;

		const k3d::uint_t group_begin = 0;
		const k3d::uint_t group_end = group_begin + (*Output.nurbs_curve_groups->first_curves).size();
		for (k3d::uint_t group = group_begin; group != group_end; ++group)
		{
			const k3d::uint_t curve_begin = (*Output.nurbs_curve_groups->first_curves)[group];
			const k3d::uint_t curve_end = curve_begin + (*Output.nurbs_curve_groups->curve_counts)[group];
			for (k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
			{
				if ((*Output.nurbs_curve_groups->curve_selection)[curve] > 0.0)
					curves.push_back(curve);
			}
		}

		if (curves.size() != 2)
		{
			k3d::log() << error << nurbs_debug << "You need to select 2 curves!\n" << std::endl;
		}
		else
		{
			nurbs_curve_modifier mod(Output);
			if (m_swap.pipeline_value())
				mod.sweep_surface(curves[0], curves[1], m_segments.pipeline_value(), m_create_caps.pipeline_value());
			else
				mod.sweep_surface(curves[1], curves[0], m_segments.pipeline_value(), m_create_caps.pipeline_value());

			if (m_delete_original.pipeline_value())
			{
				mod.delete_curve(curves[0]);
				mod.delete_curve(curves[1]);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sweep_surface, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x20e5032b, 0x0344e9da, 0x39b3318e, 0x2507c6c2),
		  "NurbsSweepSurface",
		  _("Create a Surface by traversing one curve along another and orienting it tangential to the traverse curve"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_create_caps;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_segments;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_swap;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
};

k3d::iplugin_factory& sweep_surface_factory()
{
	return sweep_surface::get_factory();
}

} // namespace nurbs

} // namespace module

