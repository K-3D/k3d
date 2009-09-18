
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
#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
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
class curve_degree_elevation :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	curve_degree_elevation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_degree(init_owner(*this) + init_name("degree") + init_label(_("The degree which will be added to the curve")) + init_description(_("The curve degree gets elevated to the amount you specify here")) + init_value(1) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_degree.changed_signal().connect(make_update_mesh_slot());
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);
		for(k3d::uint_t prim = 0; prim != Output.primitives.size(); ++prim)
		{
			boost::scoped_ptr<k3d::nurbs_curve::const_primitive> input_curves(k3d::nurbs_curve::validate(Input, *Input.primitives[prim]));
			if(!input_curves)
				continue;
			k3d::mesh::indices_t selected_curves;
			k3d::mesh::visit_arrays(*Output.primitives[prim], selected_curves_extractor(selected_curves));
			if(selected_curves.size())
			{
				boost::scoped_ptr<k3d::nurbs_curve::primitive> output_curves(k3d::nurbs_curve::create(Output.primitives[prim].create(new k3d::mesh::primitive("nurbs_curve"))));
				elevate_curve_degree(Output, *output_curves, Input, *input_curves, selected_curves);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<curve_degree_elevation, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x3f1c2279, 0xb94454e5, 0xc9484aa3, 0x54fb11c3),
		  "NurbsCurveDegreeElevation",
		  _("Takes a curve of degree p and turns it into a curve with degree p+degree"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	/// Creates a list of the selected curves in a primitive
	struct selected_curves_extractor
	{
		selected_curves_extractor(k3d::mesh::indices_t& SelectedCurves) : selected_curves(SelectedCurves) {}
		void operator()(const k3d::string_t& StructureName, const k3d::table& Structure, const k3d::string_t& ArrayName, const k3d::pipeline_data<k3d::array>& Array)
		{
			if(StructureName == "curve" && Array->get_metadata_value(k3d::metadata::key::role()) == k3d::metadata::value::selection_role())
			{
				k3d::log() << debug << "found a selection array" << std::endl;
				const k3d::mesh::selection_t* curve_selections = dynamic_cast<const k3d::mesh::selection_t*>(Array.get());
				for(k3d::uint_t curve = 0; curve != curve_selections->size(); ++curve)
				{
					k3d::log() << debug << "curve " << curve << " has selection " << curve_selections->at(curve) << std::endl;
					if(curve_selections->at(curve))
						selected_curves.push_back(curve);
				}
			}
		}
		k3d::mesh::indices_t& selected_curves;
	};

	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_degree;
};

//Create connect_curve factory
k3d::iplugin_factory& curve_degree_elevation_factory()
{
	return curve_degree_elevation::get_factory();
}

}//namespace nurbs
} //namespace module

