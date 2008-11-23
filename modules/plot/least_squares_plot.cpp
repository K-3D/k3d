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
	\brief Implements the a Least Squares Approximation solver and plotter.
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
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
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

namespace detail
{

	class least_squares_solver_plotter
	{
		private:
			std::vector<k3d::expression::parser*> function_parsers;
			std::vector<k3d::double_t> coefficients;
		public:
			least_squares_solver_plotter()
			{
				
			}

			~least_squares_solver_plotter()
			{
				// Free parsers' memory
				for(k3d::uint32_t func_index=0; func_index< function_parsers.size(); ++func_index)
				{
					delete function_parsers[func_index];
				}
			}

			//solve the linear system for the Least Square Approximation and return the coefficients
			std::vector<k3d::double_t> solve(const k3d::mesh::points_t& input_points, std::vector<k3d::double_t> values)
			{
				// Evaluate each function on every point and create a new array for accesing the z values of input_points
				k3d::uint64_t points_size = input_points.size();

				boost::multi_array<k3d::double_t,1> points_z_value(boost::extents[points_size]);
				boost::multi_array<k3d::double_t, 2> function_eval_on_points(boost::extents[function_parsers.size()][points_size]);
				for(k3d::uint64_t point_index=0; point_index< points_size; ++point_index)
				{
					values[0] = input_points[point_index][0];
					values[1] = input_points[point_index][1];
					for(k3d::uint32_t func_index=0; func_index < function_parsers.size(); ++func_index)
					{
						function_eval_on_points[func_index][point_index] = function_parsers[func_index]->evaluate(&values[0]);
					}
					points_z_value[point_index] = input_points[point_index][2];
				}
				
				//Create the linear system for least squares
					//Create the matrix A
					boost::numeric::ublas::matrix<k3d::double_t> matrix_A(function_parsers.size(),function_parsers.size());
					for(k3d::uint32_t j_index = 0; j_index < function_parsers.size(); ++j_index)
					{
						for(k3d::uint32_t i_index = 0; i_index < function_parsers.size(); ++i_index)
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
					boost::numeric::ublas::vector<k3d::double_t> vector_b(function_parsers.size());
					for(k3d::uint32_t j_index=0; j_index < function_parsers.size(); ++j_index)
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
					boost::numeric::ublas::permutation_matrix<> piv(function_parsers.size());
					boost::numeric::ublas::lu_factorize(matrix_A, piv);
					boost::numeric::ublas::lu_substitute(matrix_A, piv, vector_b);
					boost::numeric::ublas::vector<k3d::double_t> & vector_x = vector_b;
				
				//Save the result
				for(k3d::uint32_t func_index=0; func_index < function_parsers.size() ; ++func_index)
				{
					coefficients.push_back(vector_x(func_index));
				}
				//return it
				return coefficients;
			}

			//Plot the function given a grid.
			void plot(
				k3d::mesh::points_t::iterator point,
				k3d::int32_t point_rows,
				k3d::int32_t point_columns,
				k3d::double_t width,
				k3d::double_t height,
				std::vector<k3d::double_t> values
				)
			{
				k3d::vector3 i, j, k;
				// Orientation k3d::PZ:
				i = k3d::vector3(1, 0, 0);
				j = k3d::vector3(0, 1, 0);
				k = k3d::vector3(0, 0, 1);
				
// 				k3d::mesh::points_t::iterator point = output_points.begin();
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
						for(k3d::uint32_t func_index=0; func_index < function_parsers.size(); ++func_index)
						{
							w += coefficients[func_index] *
												 function_parsers[func_index]->evaluate(&values[0]);
						}
						*point++ = k3d::to_point((u * i) + (v * j) + (w * k));
					}
				}				
			}
			
			// Function Parsers for the Leat Square Approximation
			void create_function_parsers(std::vector<k3d::string_t> functions, k3d::string_t variables, k3d::string_t factory)
			{
				for(k3d::uint32_t func_index=0; func_index< functions.size(); ++func_index)
				{
					k3d::expression::parser * temp_parser =  new k3d::expression::parser();
					function_parsers.push_back(temp_parser);
					if( !function_parsers[func_index]->parse(functions[func_index],variables) )
					{
						k3d::log() << error << factory << ": function [" << functions[func_index] << " ] parsing failed: " << function_parsers[func_index]->last_parse_error() << std::endl;
						throw std::exception();
					}
				}
			}
	};

}

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
		m_uv_rename(init_owner(*this) + init_name("uv_rename") + init_label(_("Rename u v")) + init_description(_("Renames the u,v variables to suit the variables from your function. Could be x,y for example")) + init_value(k3d::string_t(_("u,v")))),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number")) + init_value(15) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number")) + init_value(15) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Grid width")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Grid height")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_output_coefficients(init_owner(*this) + init_name("output_coefficients") + init_label(_("Coefficients")) + init_description(_("Output Values of the coficients")) + init_value<k3d::string_t>("No values yet")),
		m_user_property_changed_signal(*this)
	{
		m_columns.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_rows.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_input_mesh.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_width.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_height.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
  		Output = k3d::mesh();
		k3d::polyhedron::create_grid(Output, m_rows.pipeline_value(), m_columns.pipeline_value(), m_material.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		const k3d::mesh* const input_mesh = m_input_mesh.pipeline_value();
		if(!input_mesh)
			return;
		
		k3d::string_t variables = m_uv_rename.pipeline_value();
		std::vector<k3d::string_t> functions;
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
			else if((**property).property_type() == typeid(k3d::string_t))
			{
				functions.push_back(k3d::property::pipeline_value<k3d::string_t>(**property));
			}
			else
			{
				k3d::log() << warning << factory().name() << ": user property [" << (**property).property_name() << "] with unsupported type [" << k3d::demangle((**property).property_type()) << "] will be ignored" << std::endl;
			}
		}
		
		if(!functions.size())
			return;
		
		detail::least_squares_solver_plotter solver_and_plotter;
		try
		{ //Create the Solver and Parser the Functions
			solver_and_plotter.create_function_parsers(functions, variables, factory().name());
		}
		catch(...)
		{ //Couldn't parse the functions
			return;
		}
		std::vector<k3d::double_t> coefficients;
		try
		{ // Solve the linear system for Least Squares Approximation
			coefficients = solver_and_plotter.solve(*input_mesh->points.get(), values);
		}
		catch(...)
		{ // Singular Matrix
			k3d::log() << warning << factory().name() << ": Couldn't solve the system singular matrix." << std::endl;
			return;
		}
		
		//Plot the result
		solver_and_plotter.plot(const_cast<k3d::mesh::points_t&> (*Output.points).begin(),
														m_rows.pipeline_value() + 1,
														m_columns.pipeline_value() + 1,
														m_width.pipeline_value(),
														m_height.pipeline_value(),
														values
														);
		
	// Print the functions' coefficients
		k3d::string_t output_coefficients;
		for(k3d::uint32_t func_index=0; func_index < functions.size() ; ++func_index)
		{
			std::ostringstream o;
			if (!(o << coefficients[func_index] ))
				k3d::log() << error <<"Could Convert it " << std::endl;
			// Add every value
			output_coefficients += o.str() + " ";
		}
		m_output_coefficients.set_value(output_coefficients);
		
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
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_uv_rename;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_columns;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_rows;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::string_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_output_coefficients;
	
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

