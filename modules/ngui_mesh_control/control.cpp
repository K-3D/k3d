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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/dependencies.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/custom_property_control.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/entry.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>

#include <gtkmm/box.h>
#include <gtkmm/menu.h>

#include <boost/assign/list_of.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/scoped_ptr.hpp>

#include <gtk/gtkmain.h>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a custom property-control for NURBS knot-vectors
class control :
	public k3d::ngui::custom_property::control,
	public k3d::iunknown,
	public Gtk::VBox
{
public:
	control() :
		Gtk::VBox()
	{
	}

	~control()
	{
	}

	void initialize(document_state& DocumentState, k3d::iproperty& Property)
	{
		Gtk::Button* const menu_button = new Gtk::Button(_("Mesh Options"));
		menu_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &control::on_choose), &Property));

		pack_start(*Gtk::manage(menu_button), Gtk::PACK_EXPAND_WIDGET);
	}

	void on_choose(k3d::iproperty* const Property)
	{
		const k3d::mesh* const mesh = k3d::property::pipeline_value<k3d::mesh*>(*Property);
		if(!mesh)
			return;
			
		m_menu.reset(new Gtk::Menu());

		Gtk::MenuItem* const save_mesh = new Gtk::MenuItem(_("Save Mesh ..."));
		save_mesh->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &control::on_save_mesh), mesh));
		m_menu->items().push_back(*manage(save_mesh));

		Gtk::MenuItem* const save_detailed_dotfile = new Gtk::MenuItem(_("Save GraphViz (.dot) file ..."));
		save_detailed_dotfile->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &control::on_save_dotfile), mesh));
		m_menu->items().push_back(*manage(save_detailed_dotfile));

		m_menu->show_all();
		m_menu->popup(1, gtk_get_current_event_time());
	}

	void on_save_mesh(const k3d::mesh* const Mesh)
	{
		k3d::filesystem::path output_path;
		{
			file_chooser_dialog dialog(_("Save mesh file:"), "k3d", Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(output_path))
				return;
		}

		const k3d::filesystem::path root_path = output_path.branch_path();
		k3d::dependencies dependencies;
		k3d::persistent_lookup lookup;
		k3d::ipersistent::save_context context(root_path, dependencies, lookup);

		k3d::xml::element xml("k3dml");
		k3d::xml::element& xml_mesh = xml.append(k3d::xml::element("mesh_arrays"));
		k3d::xml::save(*Mesh, xml_mesh, context);

		k3d::filesystem::ofstream stream(output_path);
		stream << k3d::xml::declaration() << xml << std::endl;
	}

  template<typename ContainerT>
  const k3d::string_t dot_array_vertex(const ContainerT& Container)
  {
    std::ostringstream buffer;
//    buffer << "v" << Container.get() << " [label=\"{|||...|}\"]";
//    buffer << "v" << Container.get() << " [label=\"{Array}\"]";
    buffer << "v" << Container.get() << " [label=\"{" << Container->type_string() << " array}\"]";
    return buffer.str();
  }

  template<typename ContainerT>
  const k3d::string_t dot_array_reference(const ContainerT& Container)
  {
    std::ostringstream buffer;
    buffer << "v" << Container.get();
    return buffer.str();
  }

	void on_save_dotfile(const k3d::mesh* const Mesh)
	{
		k3d::filesystem::path output_path;
		{
			file_chooser_dialog dialog(_("Save Mesh as GraphViz .dot file:"), "dotfile", Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(output_path))
				return;
		}

		k3d::filesystem::ofstream stream(output_path);

    stream << k3d::standard_indent << "digraph\n";
    stream << k3d::standard_indent << "{\n";
    stream << k3d::push_indent;

    stream << k3d::standard_indent << "graph [rankdir=\"LR\"]\n";
    stream << k3d::standard_indent << "node [shape=\"record\" fontname=\"Helvetica\" fontsize=12 height=0 width=0]\n";
    stream << k3d::standard_indent << "edge [fontname=\"Helvetica\" fontsize=10]\n";

    stream << k3d::standard_indent << "v" << Mesh << " [label=\"<geometry>Geometry|<points>points|<point_selection>point_selection|<point_attributes>point_attributes|<primitives>primitives\"]\n";
    stream << k3d::push_indent;

    stream << k3d::standard_indent << "subgraph cluster_0\n";
    stream << k3d::standard_indent << "{\n";
    stream << k3d::push_indent;
    stream << k3d::standard_indent << "color=white;\n";

    if(Mesh->points)
		{
			stream << k3d::standard_indent << dot_array_vertex(Mesh->points) << "\n";
			stream << k3d::standard_indent << "v" << Mesh << ":points:e -> " << dot_array_reference(Mesh->points) << ":w\n";
		}

    if(Mesh->point_selection)
		{
			stream << k3d::standard_indent << dot_array_vertex(Mesh->point_selection) << "\n";
			stream << k3d::standard_indent << "v" << Mesh << ":point_selection:e -> " << dot_array_reference(Mesh->point_selection) << ":w\n";
		}

    stream << k3d::pop_indent;
    stream << k3d::standard_indent << "}\n";
 
    for(k3d::mesh::primitives_t::const_iterator primitive = Mesh->primitives.begin(); primitive != Mesh->primitives.end(); ++primitive)
    { 
			stream << k3d::standard_indent << "v" << (*primitive).get() << " [label=\"<primitive>Primitive \\\"" << (*primitive)->type << "\\\"|<structure>structure|<attributes>attributes\"]\n";
			stream << k3d::standard_indent << "v" << Mesh << ":primitives:e -> " << "v" << (*primitive).get() << ":primitive:w\n";
			stream << k3d::push_indent;

      for(k3d::mesh::named_tables_t::const_iterator named_table = (**primitive).structure.begin(); named_table != (**primitive).structure.end(); ++named_table)
      {
        const k3d::string_t name = (*named_table).first;
        const k3d::mesh::table_t& table = (*named_table).second;

        stream << k3d::standard_indent << "v" << &table;
        stream << " [label=\"<name>Table \\\"" << name << "\\\"";

        for(k3d::mesh::table_t::const_iterator array = table.begin(); array != table.end(); ++array)
          stream << "|<" << array->first << ">" << "\\\"" << array->first << "\\\"";

        stream << "\"]\n";
        stream << k3d::standard_indent << "v" << (*primitive).get() << ":structure:e -> " << "v" << &table << ":name:w\n";
        stream << k3d::push_indent;

        for(k3d::mesh::table_t::const_iterator array = table.begin(); array != table.end(); ++array)
        {
          stream << k3d::standard_indent << dot_array_vertex(array->second) << "\n";
          stream << k3d::standard_indent << "v" << &table << ":" << array->first << ":e -> " << dot_array_reference(array->second) << ":w\n";
        }

        stream << k3d::pop_indent;
      }

      for(k3d::mesh::named_tables_t::const_iterator named_table = (**primitive).attributes.begin(); named_table != (**primitive).attributes.end(); ++named_table)
      {
        const k3d::string_t name = (*named_table).first;
        const k3d::mesh::table_t& table = (*named_table).second;

        stream << k3d::standard_indent << "v" << &table;
        stream << " [label=\"<name>Table \\\"" << name << "\\\"";

        for(k3d::mesh::table_t::const_iterator array = table.begin(); array != table.end(); ++array)
          stream << "|<" << array->first << ">" << "\\\"" << array->first << "\\\"";

        stream << "\"]\n";
        stream << k3d::standard_indent << "v" << (*primitive).get() << ":attributes:e -> " << "v" << &table << ":name:w\n";
        stream << k3d::push_indent;

        for(k3d::mesh::table_t::const_iterator array = table.begin(); array != table.end(); ++array)
        {
          stream << k3d::standard_indent << dot_array_vertex(array->second) << "\n";
          stream << k3d::standard_indent << "v" << &table << ":" << array->first << ":e -> " << dot_array_reference(array->second) << ":w\n";
        }

        stream << k3d::pop_indent;
      }

			stream << k3d::pop_indent;
    }

    stream << k3d::pop_indent;
    stream << k3d::pop_indent;
    stream << k3d::standard_indent << "}\n";
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<control> factory(
			k3d::uuid(0x8faf4941, 0x674986ba, 0x5cfbeead, 0xae08da4e),
			"NGUIMeshControl",
			_("Provides a standard control for mesh properties."),
			"NGUI Control",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "property-control")("ngui:property-type", "k3d::mesh*"));

		return factory;
	}

	boost::scoped_ptr<Gtk::Menu> m_menu;
};

} // namespace mesh

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::mesh::control::get_factory());
K3D_MODULE_END

