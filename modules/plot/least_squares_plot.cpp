// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
	\brief Implements a 3D function plotter able to be animated.
	\author Timothy M. Shead (tshead@k-3d.com) 
	\author Joaquín Duo (joaduo@lugmen.org.ar) 
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/expression/parser.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_property_changed_signal.h>

#include <boost/multi_array.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>


namespace module
{

namespace plot
{

/////////////////////////////////////////////////////////////////////////////
// least_squares_plot

class least_squares_plot :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
		typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	least_squares_plot(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Points to approximate")) + init_value<k3d::mesh*>(0)),
		m_uv_rename(init_owner(*this) + init_name("uv_rename") + init_label(_("Rename u v")) + init_description(_("Renames the u,v variables to suit the variables from your function. Could be x,y for example")) + init_value(std::string(_("u,v")))),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number")) + init_value(15) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number")) + init_value(15) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Grid width")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Grid height")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_output_coefficients(init_owner(*this) + init_name("output_coefficients") + init_label(_("Coefficients")) + init_description(_("Output Values of the coficients")) + init_value<std::string>("No values yet")),
		m_user_property_changed_signal(*this)
	{
		m_columns.changed_signal().connect(make_topology_changed_slot());
		m_rows.changed_signal().connect(make_topology_changed_slot());
		m_material.changed_signal().connect(make_topology_changed_slot());
		
		m_input_mesh.changed_signal().connect(make_geometry_changed_slot());
		m_width.changed_signal().connect(make_geometry_changed_slot());
		m_height.changed_signal().connect(make_geometry_changed_slot());

		m_user_property_changed_signal.connect(make_geometry_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
  	Mesh = k3d::create_grid(m_rows.pipeline_value(), m_columns.pipeline_value(), m_material.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
		const k3d::mesh* const input_mesh = m_input_mesh.pipeline_value();
		if(!input_mesh)
			return;
		
		std::string variables = m_uv_rename.pipeline_value();
		std::vector<std::string> functions;
		std::vector<k3d::double_t> values(2, 0.0);
		
	//Collect functions and variables
		const k3d::iproperty_collection::properties_t& properties = k3d::node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(!dynamic_cast<k3d::iuser_property*>(*property))
				continue;
				
			if((**property).property_type() == typeid(k3d::double_t))
			{
				variables += "," + (**property).property_name();
				values.push_back(k3d::property::pipeline_value<k3d::double_t>(**property));
			}
			else if((**property).property_type() == typeid(std::string))
			{
				functions.push_back(k3d::property::pipeline_value<std::string>(**property));
			}
			else
			{
				k3d::log() << warning << factory().name() << ": user property [" << (**property).property_name() << "] with unsupported type [" << k3d::demangle((**property).property_type()) << "] will be ignored" << std::endl;
			}
		}
		
		if(!functions.size())
			return;
		
	// Create the parsers
		std::vector<k3d::expression::parser*> function_parsers;
		for(k3d::uint32_t func_index=0; func_index< functions.size(); ++func_index)
		{
			k3d::expression::parser * temp_parser =  new k3d::expression::parser();
			function_parsers.push_back(temp_parser);
			if( !function_parsers[func_index]->parse(functions[func_index],variables) )
			{
				k3d::log() << error << factory().name() << ": function [" << functions[func_index] << " ] parsing failed: " << function_parsers[func_index]->last_parse_error() << std::endl;
				return;
			}
		}
		
	// Evaluate each function on every point and create a new array for accesing the z values of points
		const k3d::mesh::points_t& points = *input_mesh->points.get();
		k3d::uint64_t points_size = points.size(); 

		boost::multi_array<k3d::double_t,1> points_z_value(boost::extents[points_size]);
		boost::multi_array<k3d::double_t, 2> function_eval_on_points(boost::extents[functions.size()][points_size]);
		for(k3d::uint64_t point_index=0; point_index< points_size; ++point_index)
		{
			values[0] = points[point_index][0];
			values[1] = points[point_index][1];
			for(k3d::uint32_t func_index=0; func_index < functions.size(); ++func_index)
			{
				function_eval_on_points[func_index][point_index] = function_parsers[func_index]->evaluate(&values[0]);
			}
			points_z_value[point_index] = points[point_index][2];
		}
		
	//Create the linear system for least squares
		//Create the matrix A
		boost::numeric::ublas::matrix<k3d::double_t> matrix_A(functions.size(),functions.size());
		for(k3d::uint32_t j_index = 0; j_index < functions.size(); ++j_index)
		{
			for(k3d::uint32_t i_index = 0; i_index < functions.size(); ++i_index)
			{
				k3d::double_t scalar_product = 0;
				for(k3d::uint64_t point_index=0; point_index < points_size; ++point_index)
				{
					scalar_product += function_eval_on_points[i_index][point_index]*
														function_eval_on_points[j_index][point_index];
				}
				matrix_A(j_index,i_index) = scalar_product;
			}
		}
		
		//Create the vector b
		boost::numeric::ublas::vector<k3d::double_t> vector_b(functions.size());
		for(k3d::uint32_t j_index=0; j_index < functions.size(); ++j_index)
		{
			k3d::double_t scalar_product = 0;
			for(k3d::uint64_t point_index=0; point_index < points_size; ++point_index)
			{
				scalar_product +=                   points_z_value[point_index]*
													function_eval_on_points[j_index][point_index];
			}
			vector_b(j_index) = scalar_product;
		}
		
	// Solve the linear system
		boost::numeric::ublas::permutation_matrix<> piv(functions.size());
		boost::numeric::ublas::lu_factorize(matrix_A, piv);
		boost::numeric::ublas::lu_substitute(matrix_A, piv, vector_b);
		boost::numeric::ublas::vector<k3d::double_t> & vector_x = vector_b;
		
	// Plot it
		const unsigned long point_rows = m_rows.pipeline_value() + 1;
		const unsigned long point_columns = m_columns.pipeline_value() + 1;
		const k3d::double_t width = m_width.pipeline_value();
		const k3d::double_t height = m_height.pipeline_value();

		k3d::vector3 i, j, k;
		// Orientation k3d::PZ:
		i = k3d::vector3(1, 0, 0);
		j = k3d::vector3(0, 1, 0);
		k = k3d::vector3(0, 0, 1);


		k3d::mesh::points_t::iterator point = const_cast<k3d::mesh::points_t&> (*Mesh.points).begin();
		for(k3d::uint64_t row = 0; row != point_rows; ++row)
		{
			const k3d::double_t row_percent = static_cast<k3d::double_t>(row) / static_cast<k3d::double_t>(point_rows - 1);

			for(k3d::uint64_t column = 0; column != point_columns; ++column)
			{
				const k3d::double_t column_percent = static_cast<k3d::double_t>(column) / static_cast<k3d::double_t>(point_columns - 1);

				const k3d::double_t u = k3d::mix(-0.5 * width, 0.5 * width, column_percent);
				const k3d::double_t v = k3d::mix(-0.5 * height, 0.5 * height, row_percent);

				values[0] = u;
				values[1] = v;
				k3d::double_t w = 0;
				for(k3d::uint32_t func_index=0; func_index < functions.size(); ++func_index)
				{
					w += vector_x(func_index) *
										 function_parsers[func_index]->evaluate(&values[0]);
				}
				*point++ = k3d::to_point((u * i) + (v * j) + (w * k));
			}
		}
		
	// Print the functions' coefficients
		std::string output_coefficients;
		for(k3d::uint32_t func_index=0; func_index < functions.size() ; ++func_index)
		{
			std::ostringstream o;
			if (!(o << vector_x(func_index) ))
				k3d::log() << error <<"Could Convert it " << std::endl;
			// sumar cada valor
			output_coefficients += o.str() + " ";
		}
		m_output_coefficients.set_value(output_coefficients);
		
	// Free parsers' memory
		for(k3d::uint32_t func_index=0; func_index< functions.size(); ++func_index)
		{
			delete function_parsers[func_index];
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<least_squares_plot, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x0368fa56, 0xf1cb4ab7, 0xac1c22b3, 0xbbe53330),
			"LeastSquaresPlot",
			_("Aproximates points with a surface given a set of equations."),
			"Math",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_uv_rename;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_columns;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_rows;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_output_coefficients;
	
	k3d::user_property_changed_signal m_user_property_changed_signal;
	
};

/////////////////////////////////////////////////////////////////////////////
// least_squares_plot_factory

k3d::iplugin_factory& least_squares_plot_factory()
{
	return least_squares_plot::get_factory();
}

} // namespace plot

} // namespace module

