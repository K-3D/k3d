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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>

#include <iostream>

namespace module
{

	namespace nurbs
	{


		class create_curve :
			public k3d::material_sink<k3d::mesh_source<k3d::node > >
		{
			typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;
		public:
			create_curve(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document),
				m_control_points(init_owner(*this) + init_name("control_points") + init_label(_("control_points")) + init_description(_("Number of control points")) + init_value(4) + init_constraint(constraint::minimum(4)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) ),
				m_point_spacing(init_owner(*this) + init_name("point_spacing") + init_label(_("point_spacing")) + init_description(_("Space between points")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)) ),
				m_order(init_owner(*this) + init_name("order") + init_label(_("order")) + init_description(_("Order of the curve (2-linear - 4-cubic)")) + init_value(3) + init_constraint(constraint::minimum(2)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) )
			{
				m_control_points.changed_signal().connect(make_topology_changed_slot());
				m_point_spacing.changed_signal().connect(make_topology_changed_slot());
				m_order.changed_signal().connect(make_topology_changed_slot());
				m_material.changed_signal().connect(make_topology_changed_slot());
			}

			void on_create_mesh_topology(k3d::mesh& Mesh) 
			{
			}

			void on_update_mesh_geometry(k3d::mesh& Mesh)
			{
				//Add a new curve to our Mesh
				boost::shared_ptr<k3d::mesh::nurbs_curve_groups_t> curve_group( new k3d::mesh::nurbs_curve_groups_t() );
		
				const int control_points=m_control_points.pipeline_value();
		
				boost::shared_ptr<k3d::mesh::indices_t> first_curves( new k3d::mesh::indices_t() );
				boost::shared_ptr<k3d::mesh::counts_t> curve_counts( new k3d::mesh::counts_t() );
				boost::shared_ptr<k3d::mesh::materials_t> materials( new k3d::mesh::materials_t() );
				boost::shared_ptr<k3d::mesh::indices_t> curve_first_points( new k3d::mesh::indices_t() );
				boost::shared_ptr<k3d::mesh::counts_t> curve_point_counts( new k3d::mesh::counts_t() );
				boost::shared_ptr<k3d::mesh::orders_t> curve_orders( new k3d::mesh::orders_t() );
				boost::shared_ptr<k3d::mesh::indices_t> curve_first_knots( new k3d::mesh::indices_t() );
				boost::shared_ptr<k3d::mesh::selection_t> curve_selection( new k3d::mesh::selection_t() );
				boost::shared_ptr<k3d::mesh::indices_t> curve_points( new k3d::mesh::indices_t() );
				boost::shared_ptr<k3d::mesh::weights_t> curve_point_weights( new k3d::mesh::weights_t() );
				boost::shared_ptr<k3d::mesh::knots_t> curve_knots( new k3d::mesh::knots_t() ); //there are as much knots as controlpoints+degree+1
				boost::shared_ptr<k3d::mesh::points_t> points( new k3d::mesh::points_t() );
				boost::shared_ptr<k3d::mesh::selection_t> point_selection( new k3d::mesh::selection_t() );

				first_curves->push_back(0);
				curve_counts->push_back(1);
				materials->push_back(m_material.pipeline_value());
				curve_first_points->push_back(0);
				curve_first_knots->push_back(0);
				curve_point_counts->push_back(control_points);
				curve_orders->push_back(m_order.pipeline_value());
				curve_selection->push_back(0.0);
				
				
				//we need at least as much control points as the order is
				assert_warning(curve_orders->back() < control_points);
				
				for(size_t i = 0; i < curve_orders->back(); ++i) //first point order times
					curve_knots->push_back(0);
				
				for(size_t i = 0; i < control_points - curve_orders->back() + 1; ++i)
					curve_knots->push_back(curve_knots->back()+1); //curve_knots is not empty!
				
				for(size_t i = 0; i < curve_orders->back() - 1; ++i) //last point order times
					curve_knots->push_back(curve_knots->back());
				
				for(k3d::uint_t point = 0; point < control_points; point++ )
				{
					curve_points->push_back(point); //store the index of the point
					points->push_back(k3d::point3(static_cast<double>(point) * m_point_spacing.pipeline_value(), 0.0, 0.0) );//store the point (a straight line along x-axis)
					curve_point_weights->push_back(1.0);
					
					point_selection->push_back(0.0);
				}
				
				
				curve_group->first_curves = first_curves;
				curve_group->curve_counts = curve_counts;
				curve_group->materials = materials;
				curve_group->curve_first_points = curve_first_points;
				curve_group->curve_point_counts = curve_point_counts;
				curve_group->curve_orders = curve_orders;
				curve_group->curve_first_knots = curve_first_knots;
				curve_group->curve_selection = curve_selection;
				curve_group->curve_points = curve_points;
				curve_group->curve_point_weights = curve_point_weights;
				curve_group->curve_knots = curve_knots;
				
				Mesh.points = points;
				Mesh.point_selection = point_selection;

				Mesh.nurbs_curve_groups = curve_group;

				assert_warning(k3d::validate_nurbs_curve_groups(Mesh));
				assert_warning(k3d::validate(Mesh));
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<create_curve, k3d::interface_list<k3d::imesh_source > > factory(
				k3d::uuid(0x13c474c8, 0x5c4e278f, 0x70eb8c8c, 0xabe8bd2a),
					"NurbsCurve",
					_("Generates a NURBS curve"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}

		private:
			k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_control_points;
			k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_point_spacing;
			k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_order;
		};
		
		/////////////////////////////////////////////////////////////////////////////
		// create_curve_factory

		k3d::iplugin_factory& create_curve_factory()
		{
			return create_curve::get_factory();
		}		
	}
}
