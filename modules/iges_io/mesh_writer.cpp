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
	parameter_section(const char Label) : m_column(0), m_line(0), m_width(72), m_label(Label)
	{
	}
	
	// Ends the current line
	void endline()
	{
		const k3d::uint_t padding = m_width - m_column;
		for(k3d::uint_t i = 0; i != padding; ++i)
			stream << " ";
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
	
	std::stringstream stream;
	
private:
	k3d::uint_t m_column;
	k3d::uint_t m_line;
	const k3d::uint_t m_width;
	
	const char m_label;
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

/// Helper struct to simplify writing in the fixed-width format of IGES
struct iges_formatter
{
	iges_formatter(std::ostream& Output, const std::string& Header, const std::string& Name) : m_output(Output)
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
		m_output << global_section.stream.str();
	}
	private:
		std::ostream& m_output;
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

