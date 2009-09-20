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

#include "nurbs_curve_modifier.h"

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
void connect_at_points(k3d::mesh& Mesh, k3d::uint_t curve1, k3d::uint_t curve2, k3d::uint_t point1, k3d::uint_t point2);

class connect_curves :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	connect_curves(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
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
		assert_not_implemented();

//		k3d::mesh::indices_t selected_primitives, selected_curves; // Keeps track of primitives and curves containing selected points
//		k3d::mesh::bools_t first_point_selections; // True if the first point of a curve was selected, false otherwise
//		for(k3d::uint_t prim_idx = 0; prim_idx != Input.primitives.size(); ++prim_idx)
//		{
//			boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves(k3d::nurbs_curve::validate(Input, *Input.primitives[prim_idx]));
//			if(!curves)
//				continue;
//			for(k3d::uint_t curve = 0; curve != curves->curve_first_points.size(); ++curve)
//			{
//				const k3d::uint_t first_curve_point = curves->curve_first_points[curve];
//				const k3d::uint_t last_curve_point = first_curve_point + curves->curve_point_counts[curve] - 1;
//				if(point_selections[curves->curve_points[first_curve_point]] && point_selecions[curves->curve_points[last_curve_point]])
//				{
//					k3d::log() << error << "NurbsConnectCurves: selected points must lie on different curves" << std::endl;
//					return;
//				}
//				if(point_selections[curves->curve_points[first_curve_point]] || point_selecions[curves->curve_points[last_curve_point]])
//				{
//					selected_primitives.push_back(prim_idx);
//					selected_curves.push_back(curve);
//					first_point_selections.push_back(point_selecions[curves->curve_points[first_curve_point]]);
//				}
//			}
//		}
//
//		if(selected_curves.size() != 2)
//		{
//			k3d::log() << error << "NurbsConnectCurves: you must select two end points on two different NURBS curves" << std::endl;
//			return;
//		}

		//module::nurbs::connect_curves(Output, output_curves, Input, selected_primitives[0], selected_curves[0], first_point_selections[0], selected_primitives[1], selected_curves[1], first_point_selections[1]);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<connect_curves, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x959eb84b, 0x544d0672, 0xd53d899b, 0x6a568e86),
		  "NurbsConnectCurves",
		  _("Connects a set of NURBS curves"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	/// Stores the curve point index each time one of the given points was encountered
//	};

	/// Get the curve the given CurvePoint belongs to, if it is an endpoint. Throw an exception otherwise
	const k3d::uint_t get_curve_endpoint(const k3d::nurbs_curve::const_primitive& NurbsCurves, const k3d::uint_t CurvePoint)
	{
		for(k3d::uint_t curve = 0; curve != NurbsCurves.curve_first_points.size(); ++curve)
		{
			const k3d::uint_t first_point = NurbsCurves.curve_first_points[curve];
			const k3d::uint_t last_point = first_point + NurbsCurves.curve_point_counts[curve] - 1;
			if(first_point == CurvePoint || last_point == CurvePoint)
			{
				return curve;
			}
		}
		std::stringstream errormsg;
		errormsg << "Curve point " << CurvePoint << " was not an end point of a curve";
		throw std::runtime_error(errormsg.str());
	}
};

//Create connect_curve factory
k3d::iplugin_factory& connect_curves_factory()
{
	return connect_curves::get_factory();
}

}//namespace nurbs

} //namespace module
