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

#include "nurbs_curves.h"
#include "nurbs_patches.h"
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
class sweep_surface :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	sweep_surface(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_delete_original(init_owner(*this) + init_name(_("delete_original")) + init_label(_("Delete the Curve")) + init_description(_("Delete the original curves")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_delete_original.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		if(!Output.points.get())
			return;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);
		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(Output));

		k3d::mesh selected_curves_mesh;
		selected_curves_mesh.points.create();
		selected_curves_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> paths(k3d::nurbs_curve::create(selected_curves_mesh));
		boost::scoped_ptr<k3d::nurbs_curve::primitive> sweep_curves(k3d::nurbs_curve::create(selected_curves_mesh));
		visit_selected_curves(Output, curve_extractor(selected_curves_mesh, *paths, *sweep_curves));

		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_paths(k3d::nurbs_curve::validate(selected_curves_mesh, *selected_curves_mesh.primitives.front()));
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_sweep_curves(k3d::nurbs_curve::validate(selected_curves_mesh, *selected_curves_mesh.primitives.back()));

		sweep(Output, *output_patches, selected_curves_mesh, *const_sweep_curves, *const_paths);

		if(m_delete_original.pipeline_value())
		{
			delete_selected_curves(Output);
		}
		delete_empty_primitives(Output);
		k3d::mesh::delete_unused_points(Output);
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
	/// Extracts the first curve as a path, and the rest as curves that sweep it
	struct curve_extractor
	{
		k3d::mesh& mesh;
		k3d::nurbs_curve::primitive& paths;
		k3d::nurbs_curve::primitive& sweep_curves;
		curve_extractor(k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Paths, k3d::nurbs_curve::primitive& SweepCurves) : mesh(Mesh), paths(Paths), sweep_curves(SweepCurves) {}
		void operator()(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t& Curve)
		{
			if(paths.curve_first_points.empty())
			{
				add_curve(mesh, paths, Mesh, Curves, Curve);
				paths.material = Curves.material;
			}
			else
			{
				add_curve(mesh, sweep_curves, Mesh, Curves, Curve);
				sweep_curves.material = Curves.material;
			}
		}
	};
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_original;
};

k3d::iplugin_factory& sweep_surface_factory()
{
	return sweep_surface::get_factory();
}

} // namespace nurbs

} // namespace module

