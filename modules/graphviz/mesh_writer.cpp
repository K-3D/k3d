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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/mesh_writer.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace graphviz
{

/////////////////////////////////////////////////////////////////////////////
// mesh_writer

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
			k3d::uuid(0xb0fc811a, 0xd241b237, 0xd8922faf, 0x70e32542),
			"GraphVizMeshWriter",
			_("Mesh writer that saves meshes as GraphViz (.dot) files"),
			"MeshWriter",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	template<typename ContainerT>
	static const k3d::string_t dot_array_vertex(const ContainerT& Container)
	{
		std::ostringstream buffer;

		buffer << "v" << Container.get();
		buffer << " [";
		buffer << " shape=\"plaintext\"";
		buffer << " label=<";
		buffer << " <table border=\"0\" cellborder=\"1\" cellspacing=\"0\"><tr>";
		buffer << " <td";
		buffer << " port=\"array\"";
		if(Container->type_string() == "k3d::imaterial*")
			buffer << " color=\"#00dd00\"";
		else if(Container->get_metadata_value(k3d::metadata::key::role()) == k3d::metadata::value::selection_role())
			buffer << " color=\"#0000dd\"";
		else if(Container->get_metadata_value(k3d::metadata::key::domain()) == k3d::metadata::value::mesh_point_indices_domain())
			buffer << " color=\"#dd0000\"";
		buffer << ">";
		buffer << Container->type_string() << " array";
		buffer << "</td>";
		buffer << " </tr></table>";
		buffer << " >";
		buffer << "]";
		
		return buffer.str();
	}

	template<typename ContainerT>
	static const k3d::string_t dot_array_reference(const ContainerT& Container)
	{
		std::ostringstream buffer;
		buffer << "v" << Container.get() << ":array";
		return buffer.str();
	}

	static void table_vertex(const k3d::string_t& Name, const k3d::table& Table, std::ostream& Output)
	{
		Output << k3d::standard_indent << "v" << &Table;
		Output << " [";
		Output << " shape=\"plaintext\"";
		Output << " label=<";
		Output << " <table border=\"0\" cellborder=\"1\" cellspacing=\"0\">";
		Output << " <tr><td port=\"name\" bgcolor=\"#ddddff\">Table \"" << Name << "\"</td></tr>";

		for(k3d::mesh::table_t::const_iterator array = Table.begin(); array != Table.end(); ++array)
			Output << "<tr><td port=\"" << array->first << "\">" << "\"" << array->first << "\"" << "</td></tr>";
		Output << " </table>";
        	Output << " >";
		Output << "]\n";

		for(k3d::mesh::table_t::const_iterator array = Table.begin(); array != Table.end(); ++array)
		{
			Output << k3d::standard_indent << dot_array_vertex(array->second) << "\n";
			Output << k3d::standard_indent << "v" << &Table << ":" << array->first << ":e -> " << dot_array_reference(array->second) << ":w\n";
		}

		Output << k3d::pop_indent;
	}

	static const k3d::string_t table_reference(const k3d::table& Table)
	{
		std::ostringstream buffer;
		buffer << "v" << &Table << ":name:w";
		return buffer.str();
	}

	void on_write_mesh(const k3d::mesh& Input, const k3d::filesystem::path& OutputPath, std::ostream& Output)
	{
		Output << "// Written by K-3D " << K3D_VERSION << ", http://www.k-3d.org\n\n";

		Output << k3d::standard_indent << "digraph\n";
		Output << k3d::standard_indent << "{\n";
		Output << k3d::push_indent;

		Output << k3d::standard_indent << "graph [rankdir=\"LR\" ranksep=\"1.5\"]\n";
		Output << k3d::standard_indent << "node [shape=\"record\" fontname=\"Helvetica\" fontsize=12 height=0 width=0]\n";
		Output << k3d::standard_indent << "edge [fontname=\"Helvetica\" fontsize=10]\n";

		Output << k3d::standard_indent << "v" << &Input;
		Output << " [";
		Output << " shape=\"plaintext\"";
		Output << " label=<";
		Output << " <table border=\"0\" cellborder=\"1\" cellspacing=\"0\">";
		Output << " <tr><td port=\"geometry\" bgcolor=\"#ffdddd\">Geometry</td></tr>";
		Output << " <tr><td port=\"points\">points</td></tr>";
		Output << " <tr><td port=\"point_selection\">point_selection</td></tr>";
		Output << " <tr><td port=\"point_attributes\">point_attributes</td></tr>";
		Output << " <tr><td port=\"primitives\">primitives</td></tr>";
		Output << " </table>";
		Output << ">";
		Output << "]\n";
		Output << k3d::push_indent;

		Output << k3d::standard_indent << "subgraph cluster_0\n";
		Output << k3d::standard_indent << "{\n";
		Output << k3d::push_indent;
		Output << k3d::standard_indent << "color=white;\n";

		if(Input.points)
		{
			Output << k3d::standard_indent << dot_array_vertex(Input.points) << "\n";
			Output << k3d::standard_indent << "v" << &Input << ":points:e -> " << dot_array_reference(Input.points) << ":w\n";
		}

		if(Input.point_selection)
		{
			Output << k3d::standard_indent << dot_array_vertex(Input.point_selection) << "\n";
			Output << k3d::standard_indent << "v" << &Input << ":point_selection:e -> " << dot_array_reference(Input.point_selection) << ":w\n";
		}

		Output << k3d::pop_indent;
		Output << k3d::standard_indent << "}\n";

		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{ 
			Output << k3d::standard_indent << "v" << (*primitive).get();
			Output << " [";
			Output << " shape=\"plaintext\"";
			Output << " label=<";
			Output << " <table border=\"0\" cellborder=\"1\" cellspacing=\"0\">";
			Output << " <tr><td port=\"primitive\" bgcolor=\"#ddffdd\">Primitive \"" << (*primitive)->type << "\"</td></tr>";
			Output << " <tr><td port=\"structure\">structure</td></tr>";
			Output << " <tr><td port=\"attributes\">attributes</td></tr>";
			Output << " </table>";
			Output << ">";
			Output << "]\n";
			Output << k3d::standard_indent << "v" << &Input << ":primitives:e -> " << "v" << (*primitive).get() << ":primitive:w\n";
			Output << k3d::push_indent;

			for(k3d::mesh::named_tables_t::const_iterator named_table = (**primitive).structure.begin(); named_table != (**primitive).structure.end(); ++named_table)
			{
				table_vertex(named_table->first, named_table->second, Output);
				Output << k3d::standard_indent << "v" << (*primitive).get() << ":structure:e -> " << "v" << &named_table->second << ":name:w\n";
			}

			for(k3d::mesh::named_tables_t::const_iterator named_table = (**primitive).attributes.begin(); named_table != (**primitive).attributes.end(); ++named_table)
			{
				table_vertex(named_table->first, named_table->second, Output);
				Output << k3d::standard_indent << "v" << (*primitive).get() << ":attributes:e -> " << "v" << &named_table->second << ":name:w\n";
			}

			Output << k3d::pop_indent;
		}

		Output << k3d::pop_indent;
		Output << k3d::pop_indent;
		Output << k3d::standard_indent << "}\n";
	}
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

} // namespace graphviz

} // namespace module

