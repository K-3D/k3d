// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "nurbs_curves.h"

#include <k3dsdk/basic_math.h>
#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>

namespace module
{

namespace nurbs
{

class flip_curve :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	flip_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points.get())
			return;

		for(k3d::uint_t prim_idx = 0; prim_idx != Output.primitives.size(); ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_curve::primitive> curve_prim(k3d::nurbs_curve::validate(Output, Output.primitives[prim_idx]));
			if(!curve_prim)
				continue;

			for(k3d::uint_t curve = 0; curve != curve_prim->curve_first_points.size(); ++curve)
			{
				if(curve_prim->curve_selections[curve])
					module::nurbs::flip_curve(*curve_prim, curve);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<flip_curve, k3d::interface_list<k3d::imesh_source , k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xd69c221b, 0xdb49674c, 0x8ca63580, 0x517c0d23),
		  "NurbsFlipCurve",
		  _("Flips the order of the points for the selected curves"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

};

k3d::iplugin_factory& flip_curve_factory()
{
	return flip_curve::get_factory();
}

} //namespace nurbs

} //namespace module

