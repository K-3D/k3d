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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "nurbs_curves.h"
#include "utility.h"

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

class merge_connected_curves :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	merge_connected_curves(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		k3d::mesh selected_curves_mesh;
		selected_curves_mesh.points.create();
		selected_curves_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> selected_curves(k3d::nurbs_curve::create(selected_curves_mesh));
		selected_curves->material.push_back(0);
		visit_selected_curves(Output, selected_curve_extractor(selected_curves_mesh, *selected_curves));
		delete_selected_curves(Output);
		boost::scoped_ptr<k3d::nurbs_curve::primitive> output_curves(k3d::nurbs_curve::create(Output));
		module::nurbs::merge_connected_curves(Output, *output_curves, selected_curves_mesh, 0);
		if(output_curves->material.empty())
			output_curves->material.push_back(selected_curves->material.back());
		delete_empty_primitives(Output);

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<merge_connected_curves, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x0f2af2c9, 0xe44d9e20, 0x3fcf769d, 0xb586ca27),
		  "NurbsMergeConnectedCurves",
		  _("Join selected curves with common endpoints into a single NURBS curve"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

k3d::iplugin_factory& merge_connected_curves_factory()
{
	return merge_connected_curves::get_factory();
}

} //namespace nurbs

} //namespace module
