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
//
//		k3d::mesh::indices_t endpoints;
//		for(k3d::uint_t point = 0; point != Input.points->size(); ++point)
//		{
//			if(Output.point_selection->at(point))
//				endpoints.push_back(point);
//		}
//
//		if(endpoints.size() != 2)
//		{
//			k3d::log() << error << "NurbsConnectCurves: you must select two end points on two different NURBS curves" << std::endl;
//			return;
//		}
//
//		k3d::mesh::indices_t first_primitives, second_primitives, first_curve_points, second_curve_points;
//		for(k3d::uint_t i = 0; i != Input.primitives.size(); +i)
//		{
//			if(Input.primitives[i]->type == "nurbs_curve")
//			{
//				curve_locator visitor(endpoints[0], endpoints[1]);
//				k3d::mesh::visit_arrays(primitive, visitor);
//				if(visitor.first_curve_points.size())
//				{
//					first_primitives.push_back(i);
//				}
//				if(visitor.second_curve_points.size())
//				{
//					second_primitives.push_back(i);
//				}
//				first_curve_points.insert(first_curve_points.end(), visitor.first_curve_points.begin(), visitor.first_curve_points.end());
//			}
//		}
//
//		if(first_curve_points.size() != 1)
//		{
//			k3d::log() << error << "NurbsConnectCurves: First selected point " << endpoints[0] << " was owned by multiple curves" << std::endl;
//			return;
//		}
//		if(second_curve_points.size() != 1)
//		{
//			k3d::log() << error << "NurbsConnectCurves: Second selected point " << endpoints[1] << " was owned by multiple curves" << std::endl;
//			return;
//		}
//
//		return_if_fail(first_primitives.size() == 1);
//		return_if_fail(second_primitives.size() == 1);
//
//		boost::shared_ptr<k3d::nurbs_curve::const_primitive> input1 = k3d::nurbs_curve::validate(Input, *Input.primitives[first_primitives[0]]);
//		boost::shared_ptr<k3d::nurbs_curve::const_primitive> input2 = first_primitives[0] == second_primitives[0] ? input1 : k3d::nurbs_curve::validate(Input, *Input.primitives[second_primitives[0]]);
//
//		try
//		{
//			const k3d::uint_t curve1 = get_curve(*input1, first_curve_points[0]);
//			const k3d::uint_t curve2 = get_curve(*input2, second_curve_points[0]);
//			k3d::mesh temp_mesh;
//			temp_mesh.point_attributes = Input.point_attributes.clone_types();
//			boost::scoped_ptr<k3d::nurbs_curve::primitive> temp_curves = k3d::nurbs_curve::create(temp_mesh);
//			temp_curves->constant_attributes = input1->constant_attributes.clone();
//			temp_curves->curve_attributes = input1->curve_attributes.clone_types();
//			temp_curves->varying_attributes = input1->varying_attributes.clone_types();
//			temp_curves->vertex_attributes = input1->vertex_attributes.clone_types();
//			k3d::mesh::indices_t point_map(Input.points->size());
//			k3d::mesh::bools_t added_points(Input.points->size(), false);
//			add_curve(temp_mesh, *temp_curves, Input, *input1, curve1, point_map, added_points);
//			add_curve(temp_mesh, *temp_curves, Input, *input2, curve2, point_map, added_points);
//			// indices for the temp curves
//			const k3d::uint_t curve1_first = temp_curves->curve_first_points[0];
//			const k3d::uint_t curve1_last = temp_curves->curve_point_counts[0] - 1;
//			const k3d::uint_t curve2_first = curve1_last + 1;
//			const k3d::uint_t curve2_last = curve1_last + temp_curves->curve_point_counts[1];
//			const k3d::uint_t curve_point1 = point_map[endpoints[0]] ==  temp_curves->curve_points[curve1_first] ? curve1_first : curve1_last;
//			const k3d::uint_t curve_point2 = point_map[endpoints[1]] ==  temp_curves->curve_points[curve2_first] ? curve2_first : curve2_last;
//			module::nurbs::connect_curves(temp_curves, 0, curve_point1, 1, curve_point2);
//		}
//		catch(std::runtime_error& Error)
//		{
//			k3d::log() << error << "NurbsConnectCurves: " << Error.what() << std::endl;
//		}


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
