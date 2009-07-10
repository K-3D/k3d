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
class skinned_surface :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	skinned_surface(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_along(init_owner(*this) + init_name("along") + init_label(_("Ordered along")) + init_description(_("Axis along which the curves are ordered")) + init_value(k3d::X) + init_enumeration(k3d::axis_values())),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curves")) + init_description(_("Delete the original curves")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_along.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		boost::scoped_ptr<k3d::nurbs_curve::primitive> nurbs(get_first_nurbs_curve(Output));
		if(!nurbs)
			return;

		k3d::geometry::merge_selection(m_mesh_selection.pipeline_value(), Output);

		std::vector<k3d::uint_t> curves;

		const k3d::uint_t curve_begin = 0;
		const k3d::uint_t curve_end = nurbs->curve_first_knots.size();
		for (k3d::uint_t curve = curve_begin; curve != curve_end; ++curve)
		{
			if (nurbs->curve_selections[curve] > 0.0)
				curves.push_back(curve);
		}

		if (curves.size() < 2)
		{
			k3d::log() << error << nurbs_debug << "You need to select at least 2 curves!\n" << std::endl;
			return;
		}
		else
		{
			nurbs_curve_modifier mod(Output, *nurbs);
			if (curves.size() == 2)
			{
				mod.ruled_surface(curves.at(0), curves.at(1));
			}
			else
			{
				mod.skinned_surface(curves, m_along.pipeline_value());
			}

			if (m_delete_original.pipeline_value())
			{
				for (int i = curves.size() - 1; i >= 0; i--)
					mod.delete_curve(curves[i]);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<skinned_surface, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xe6e5899a, 0x9c445204, 0xa7758297, 0x86cd38e4),
		  "NurbsSkinnedSurface",
		  _("Creates a NURBS surface stretched along all selected curves"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_along;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
};

k3d::iplugin_factory& skinned_surface_factory()
{
	return skinned_surface::get_factory();
}

} // namespace nurbs

} // namespace module

