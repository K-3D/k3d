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
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{

class extract_curve_point :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	extract_curve_point(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_parameter(init_owner(*this) + init_name(_("parameter")) + init_label(_("Parameter Value")) + init_description(_("Parameter value at which the point will be extracted")) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0.0 , constraint::maximum(1.0))) + init_value(0.0))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_parameter.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!Input.points)
			return;
		k3d::mesh tmp = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), tmp);

		k3d::mesh::points_t& points = Output.points.create();
		k3d::mesh::selection_t& point_selections = Output.point_selection.create();
		const k3d::double_t u = m_parameter.pipeline_value();
		visit_selected_curves(tmp, curve_point_extractor(points, Output.point_attributes, u));
		point_selections.resize(points.size(), 1.0);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<extract_curve_point, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x6d918270, 0xc84c904f, 0x1e078e8d, 0x661b8d72),
		  "NurbsExtractCurvePoint",
		  _("Extract the point at the given parameter value"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	// Extracts points at the given U value
	struct curve_point_extractor
	{
		curve_point_extractor(k3d::mesh::points_t& Points, k3d::table& PointAttributes, const k3d::double_t U) : points(Points), point_attributes(PointAttributes), u(U) {}
		void operator()(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t& Curve)
		{
			curve_arrays curve(Mesh, Curves, Curve, true);
			curve_arrays::curve_value value = curve.evaluate(u);
			points.push_back(value.position());
			k3d::table_copier copier(value.point_attributes, point_attributes);
			copier.push_back(0);
		}
		k3d::mesh::points_t& points;
		k3d::table& point_attributes;
		const k3d::double_t u;
	};
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_parameter;
};

//Create connect_curve factory
k3d::iplugin_factory& extract_curve_point_factory()
{
	return extract_curve_point::get_factory();
}

} //namespace nurbs

} //namespace module
