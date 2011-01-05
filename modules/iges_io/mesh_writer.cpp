// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/ihint.h>
#include <k3dsdk/mesh_writer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/triangulator.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>

#include <iomanip>

namespace module
{

namespace iges
{

namespace io
{

namespace detail
{
/// Helper struct to write variable-length parameter data
struct parameter_section
{
	parameter_section(const char Label, const k3d::bool_t AddDictEntry = false) : m_column(0), m_line(0), m_width(AddDictEntry ? 64 : 72), m_label(Label), m_dict_entry(0)
	{
	}
	
	// Ends the current line
	void endline()
	{
		const k3d::uint_t padding = m_width - m_column;
		for(k3d::uint_t i = 0; i != padding; ++i)
			stream << " ";
		if(m_dict_entry != 0)
			stream << " " << std::setw(7) << std::right << m_dict_entry;
		stream << m_label << std::setw(7) << std::right << (++m_line) << "\n";
		m_column = 0;
	}
	
	// Ends the current record
	void endrecord()
	{
		stream << ";";
		++m_column;
		endline();
	}
	
	// Adds a formatted value to the stream
	void add_formatted_value(const std::string& Value)
	{
		if(Value.size() + m_column < m_width)
		{
			stream << Value;
			m_column += Value.size();
		}
		else
		{
			endline();
			stream << Value;
			m_column += Value.size();
		}
	}
	
	void set_dict_entry(const k3d::uint_t Dict)
	{
		m_dict_entry = Dict;
	}
	
	/// Number of lines stored
	k3d::uint_t lines() const
	{
		return m_line;
	}
	
	std::stringstream stream;
	
private:
	k3d::uint_t m_column;
	k3d::uint_t m_line;
	const k3d::uint_t m_width;
	
	const char m_label;
	
	k3d::uint_t m_dict_entry;
};

parameter_section& operator<<(parameter_section& ParamaterSection, const std::string& Value)
{
	if(Value.empty())
	{
		ParamaterSection.add_formatted_value(",");
		return ParamaterSection;
	}
	
	std::stringstream temp;
	temp << Value.size() << 'H' << Value << ',';
	ParamaterSection.add_formatted_value(temp.str());
	
	return ParamaterSection;
}

parameter_section& operator<<(parameter_section& ParamaterSection, const int Value)
{
	std::stringstream temp;
	temp << Value << ',';
	ParamaterSection.add_formatted_value(temp.str());
	
	return ParamaterSection;
}

parameter_section& operator<<(parameter_section& ParamaterSection, const k3d::double_t Value)
{
	std::stringstream temp;
	temp << std::setprecision(15) << std::fixed << Value << ',';
	ParamaterSection.add_formatted_value(temp.str());
	
	return ParamaterSection;
}

/// Represents an entry in a directory
struct directory_entry
{
	directory_entry(const int Type, const int ParameterData, const int ParameterLength, const int Index) :
		entity_type(Type),
		parameter_data(ParameterData),
		structure(0),
		line_font_pattern(0),
		level(0),
		view(0),
		transformation_matrix(0),
		label_display_associativity(0),
		status("00000000"),
		line_weight(0),
		color(0),
		parameter_line_count(ParameterLength),
		form(0),
		reserved1("        "),
		reserved2("        "),
		label("        "),
		entity_subscript(0)
	{
	}
	
	/// See file format spec for details on all these fields
	int entity_type;
	int parameter_data;
	int structure;
	int line_font_pattern;
	int level;
	int view;
	int transformation_matrix;
	int label_display_associativity;
	std::string status;
	int line_weight;
	int color;
	int parameter_line_count;
	int form;
	std::string reserved1;
	std::string reserved2;
	std::string label;
	int entity_subscript;
	
	/// Zero-based index of this entry (the 1-based line number as specified in the standard is derived from this)
	k3d::uint_t index;
	
	/// Write to an output stream
	void write(std::ostream& Output)
	{
		Output <<
			std::setw(8) << entity_type <<
			std::setw(8) << parameter_data <<
			std::setw(8) << structure <<
			std::setw(8) << line_font_pattern <<
			std::setw(8) << level <<
			std::setw(8) << view <<
			std::setw(8) << transformation_matrix <<
			std::setw(8) << label_display_associativity <<
			std::setw(8) << status <<
			'D' << std::setw(7) << 1 + 2*index << "\n" <<
			std::setw(8) << entity_type <<
			std::setw(8) << line_weight <<
			std::setw(8) << color <<
			std::setw(8) << parameter_line_count <<
			std::setw(8) << form <<
			reserved1 << reserved2 << label <<
			std::setw(8) << entity_subscript <<
			'D' << std::setw(7) << 2 + 2*index << "\n";
	}
};

/// Helper struct to simplify writing in the fixed-width format of IGES
struct iges_formatter
{
	iges_formatter(std::ostream& Output, const std::string& Header, const std::string& Name) : m_output(Output), m_parameters('P', true), m_directory_index(0)
	{
		// Get the creation time
		std::stringstream time_str;
		const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_facet* f = new boost::posix_time::time_facet("%Y%m%d.%H%M%S");
		time_str.imbue(std::locale(time_str.getloc(),f));
		time_str << now;
		
		// Write the start section
		m_output << std::setw(72) << std::left << Header << 'S' << std::setw(7) << std::right << 1 << "\n";
		
		// Write the global section
		parameter_section global_section('G');
		global_section << "" << "" << Name << Name + ".iges" << K3D_PACKAGE << K3D_VERSION << 32 << 38 << 7 << 307 << 15
			<< Name << 1. << 6 << "M" << 1 << 0.005 << time_str.str() << 0.000001 << "" << "" << "" << 11 << "" << "" << "";
		global_section.endrecord();
		m_global_size = global_section.lines();
		m_output << global_section.stream.str();
	}
	
	~iges_formatter()
	{
		m_output << m_parameters.stream.str();
		m_output << 'S' << std::setw(7) << 1 << 'G' << std::setw(7) << m_global_size << 'D' << std::setw(7) << 2*m_directory_index << 'P' << std::setw(7) << m_parameters.lines() << std::setw(40) << "" << 'T' << std::setw(7) << 1 << "\n";
	}
	
	/// K1: number of points in U direction - 1
	/// M1: degree in the U direction (= order - 1)
	void add_nurbs_patch(const int K1, const int K2, const int M1, const int M2, const k3d::mesh::knots_t& UKnots, const k3d::mesh::knots_t& VKnots, const k3d::mesh::weights_t Weights, const k3d::mesh::points_t& Points)
	{
		const int param_start = m_parameters.lines();
		m_parameters.set_dict_entry(m_directory_index + 1);
		
		// Set global patch prameters
		m_parameters << 128 << K1 << K2 << M1 << M2 << 0 << 0 << 0 << 0 << 0;
		
		// Add U knots
		const k3d::uint_t nb_u_knots = UKnots.size();
		for(k3d::uint_t i = 0; i != nb_u_knots; ++i)
			m_parameters << UKnots[i];
		
		// Add V knots
		const k3d::uint_t nb_v_knots = VKnots.size();
		for(k3d::uint_t i = 0; i != nb_v_knots; ++i)
			m_parameters << VKnots[i];
		
		// Weights
		const k3d::uint_t nb_weights = Weights.size();
		for(k3d::uint_t i = 0; i != nb_weights; ++i)
			m_parameters << Weights[i];
		
		// Points
		for(k3d::uint_t i = 0; i != nb_weights; ++i)
			m_parameters << Points[i][0] << Points[i][1] << Points[i][2];
		
		// Parameter limits
		m_parameters << UKnots.front() << UKnots.back() << VKnots.front() << VKnots.back();
		
		m_parameters.endrecord();
		const int param_end = m_parameters.lines();
		
		directory_entry entry(128, param_start+1, param_end - param_start, m_directory_index);
		entry.write(m_output);
		
		++m_directory_index;
	}
	
	private:
		std::ostream& m_output;
		std::stringstream m_directory;
		parameter_section m_parameters;
		k3d::uint_t m_directory_index;
		k3d::uint_t m_global_size;
};

} // detail
	
/////////////////////////////////////////////////////////////////////////////
// mesh_writerio

class mesh_writer :
	public k3d::mesh_writer<k3d::node >
{
	typedef k3d::mesh_writer<k3d::node > base;

public:
	mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_writer, k3d::interface_list<k3d::imesh_sink > > factory(
			k3d::uuid(0x95dc7909, 0xfa8e4c74, 0xab789866, 0xef89e707),
			"IGESMeshWriter",
			_("Mesh writer that saves IGES CAD (.iges) files"),
			"MeshWriter",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	
	void on_write_mesh(const k3d::mesh& Input, const k3d::filesystem::path& OutputPath, std::ostream& Output)
	{
		detail::iges_formatter iges(Output, "K-3D IGES export", OutputPath.leaf().raw().substr(0, OutputPath.leaf().raw().find_last_of(".")));
		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			// TODO: Only export NURBS patches for now
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs_patch(k3d::nurbs_patch::validate(Input, **primitive));
			if(!nurbs_patch)
				continue;
			
			const k3d::uint_t nb_patches = nurbs_patch->patch_first_points.size();
			for(k3d::uint_t patch_idx = 0; patch_idx != nb_patches; ++patch_idx)
			{
				const k3d::uint_t nb_u_points = nurbs_patch->patch_u_point_counts[patch_idx];
				const k3d::uint_t nb_v_points = nurbs_patch->patch_v_point_counts[patch_idx];
				const k3d::uint_t u_order = nurbs_patch->patch_u_orders[patch_idx];
				const k3d::uint_t v_order = nurbs_patch->patch_v_orders[patch_idx];
				
				const k3d::uint_t u_knots_begin = nurbs_patch->patch_u_first_knots[patch_idx];
				const k3d::uint_t u_knots_end = u_knots_begin + u_order + nb_u_points;
				const k3d::mesh::knots_t u_knots(nurbs_patch->patch_u_knots.begin() + u_knots_begin, nurbs_patch->patch_u_knots.begin() + u_knots_end);
				
				const k3d::uint_t v_knots_begin = nurbs_patch->patch_v_first_knots[patch_idx];
				const k3d::uint_t v_knots_end = v_knots_begin + v_order + nb_v_points;
				const k3d::mesh::knots_t v_knots(nurbs_patch->patch_v_knots.begin() + v_knots_begin, nurbs_patch->patch_v_knots.begin() + v_knots_end);
				
				k3d::mesh::points_t points;
				k3d::mesh::weights_t weights;
				const k3d::uint_t nb_points = nb_u_points * nb_v_points;
				points.reserve(nb_points);
				weights.reserve(nb_points);
				const k3d::uint_t points_begin = nurbs_patch->patch_first_points[patch_idx];
				const k3d::uint_t points_end = points_begin + nb_points;
				for(k3d::uint_t i = points_begin; i != points_end; ++i)
				{
					points.push_back(Input.points->at(nurbs_patch->patch_points[i]));
					weights.push_back(nurbs_patch->patch_point_weights[i]);
				}
				
				iges.add_nurbs_patch(nb_u_points-1, nb_v_points-1, u_order-1, v_order-1, u_knots, v_knots, weights, points);
			}
		}
	}
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

} // namespace io

} // namespace iges

} // namespace module

