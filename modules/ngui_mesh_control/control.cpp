// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
#include <k3dsdk/fstream.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/custom_property_control.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/entry.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>

#include <gtkmm/box.h>
#include <gtkmm/menu.h>

#include <boost/assign/list_of.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/scoped_ptr.hpp>

#include <gtk/gtkmain.h>

namespace k3d
{
	/// Iterates over every structure in the given mesh, in hierarchical order, passing each structure to a functor.
	template<typename FunctorT>
	static void visit(const mesh& Mesh, FunctorT Functor)
	{
		Functor.start_mesh(Mesh);

		if(Mesh.points)
			Functor.mesh_points(*Mesh.points);

		if(Mesh.point_selection)
			Functor.mesh_point_selection(*Mesh.point_selection);

		for(mesh::primitives_t::const_iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p)
		{
			const mesh::primitive& primitive = **p;
			Functor.start_primitive(primitive);

			Functor.start_topology(primitive.topology);
			for(mesh::named_arrays_t::const_iterator array = primitive.topology.begin(); array != primitive.topology.end(); ++array)
				Functor.topology_array(array->first, *array->second);
			Functor.finish_topology(primitive.topology);

			Functor.start_named_attributes(primitive.attributes);
			for(mesh::named_attribute_arrays_t::const_iterator attributes = primitive.attributes.begin(); attributes != primitive.attributes.end(); ++attributes)
			{
				Functor.start_attributes(attributes->first, attributes->second);
				for(mesh::attribute_arrays_t::const_iterator array = attributes->second.begin(); array != attributes->second.end(); ++array)
					Functor.attribute_array(array->first, *array->second);
				Functor.finish_attributes(attributes->first, attributes->second);
			}
			Functor.finish_named_attributes(primitive.attributes);

			Functor.finish_primitive(primitive);
		}
		Functor.finish_mesh(Mesh);
	}

} // namespace k3d

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

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent, k3d::iproperty& Property)
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

		Gtk::MenuItem* const save_detailed_dotfile = new Gtk::MenuItem(_("Save GraphViz (.dot) file ..."));
		save_detailed_dotfile->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &control::on_save_dotfile), mesh));
		m_menu->items().push_back(*manage(save_detailed_dotfile));

		m_menu->show_all();
		m_menu->popup(1, gtk_get_current_event_time());
	}

	class save_dotfile
	{
	public:
		save_dotfile(std::ostream& Stream) :
			current_mesh(0),
			current_primitive(0),
			current_topology_arrays(0),
			current_named_attribute_arrays(0),
			current_attribute_arrays(0),
			current_array(0),
			stream(Stream)
		{
		}

		/// Inserts whitespace into a stream, proportional to its indentation level (we override the default to implement single_line behavior).
		static std::ostream& indentation(std::ostream& Stream)
		{
			Stream << k3d::string_t(2 * k3d::current_indent(Stream), ' ');
			return Stream;
		}
 
		void start_mesh(const k3d::mesh& Mesh)
		{
			current_mesh = &Mesh;

			stream << indentation << "digraph\n";
			stream << indentation << "{\n";
			stream << k3d::push_indent;

			stream << indentation << "graph [rankdir=\"LR\"]\n";
			stream << indentation << "node [shape=\"record\" fontname=\"Helvetica\" fontsize=12 height=0 width=0]\n";
			stream << indentation << "edge [fontname=\"Helvetica\" fontsize=10]\n";

			stream << indentation << "v" << current_mesh << " [label=\"k3d::mesh|<points>points|<point_selection>point_selection|<primitives>primitives\"]\n";
			stream << k3d::push_indent;
		}

		void mesh_points(const k3d::mesh::points_t& Points)
		{
			current_array = &Points;

			stream << indentation << "v" << current_array << " [label=\"{|||...|}\"]\n";
			stream << indentation << "v" << current_mesh << ":points:e -> " << "v" << current_array << ":w\n";

			current_array = 0;
		}

		void mesh_point_selection(const k3d::mesh::selection_t& PointSelection)
		{
			current_array = &PointSelection;

			stream << indentation << "v" << current_array << " [label=\"{|||...|}\"]\n";
			stream << indentation << "v" << current_mesh << ":point_selection:e -> " << "v" << current_array << ":w\n";

			current_array = 0;
		}

		void start_primitive(const k3d::mesh::primitive& Primitive)
		{
			current_primitive = &Primitive;

			stream << indentation << "v" << current_primitive << " [label=\"k3d::mesh::primitive|<type>type|<topology>topology|<attributes>attributes\"]\n";
			stream << indentation << "v" << current_mesh << ":primitives:e -> " << "v" << current_primitive << ":w\n";
			stream << k3d::push_indent;

			stream << indentation << "v" << current_primitive << "type [label=\"\\\"" << current_primitive->type << "\\\"\" shape=\"plaintext\"]\n";
			stream << indentation << "v" << current_primitive << ":type:e -> " << "v" << current_primitive << "type:w\n";
		}

		void start_topology(const k3d::mesh::named_arrays_t& Topology)
		{
			current_topology_arrays = &Topology;

			stream << indentation << "v" << current_topology_arrays;
			stream << " [label=\"k3d::named_arrays";
			
			for(k3d::mesh::named_arrays_t::const_iterator array = Topology.begin(); array != Topology.end(); ++array)
				stream << "|<" << array->first << ">" << "\\\"" << array->first << "\\\"";

			stream << "\"]\n";
			stream << indentation << "v" << current_primitive << ":topology:e -> " << "v" << current_topology_arrays << ":w\n";
		}

		void topology_array(const k3d::string_t& Name, const k3d::array& Array)
		{
			current_array = &Array;

			stream << indentation << "v" << current_array << " [label=\"{|||...|}\"]\n";
			stream << indentation << "v" << current_topology_arrays << ":" << Name << ":e -> " << "v" << current_array << ":w\n";

			if(Array.get_metadata_value(k3d::metadata::key::domain()) == k3d::metadata::value::mesh_point_indices_domain())
				stream << indentation << "v" << current_array << ":n -> " << "v" << current_mesh->points.get() << ":n\n";

			current_array = 0;
		}

		void finish_topology(const k3d::mesh::named_arrays_t& Topology)
		{
			current_topology_arrays = 0;
		}

		void start_named_attributes(const k3d::mesh::named_attribute_arrays_t& NamedAttributes)
		{
			current_named_attribute_arrays = &NamedAttributes;

			stream << indentation << "v" << current_named_attribute_arrays;
			stream << " [label=\"k3d::named_attribute_arrays";
			
			for(k3d::mesh::named_attribute_arrays_t::const_iterator attributes = NamedAttributes.begin(); attributes != NamedAttributes.end(); ++attributes)
				stream << "|<" << attributes->first << ">" << "\\\"" << attributes->first << "\\\"";

			stream << "\"]\n";
			stream << indentation << "v" << current_primitive << ":attributes:e -> " << "v" << current_named_attribute_arrays << ":w\n";
			stream << k3d::push_indent;
		}

		void start_attributes(const k3d::string_t& Name, const k3d::mesh::attribute_arrays_t& Attributes)
		{
			current_attribute_arrays = &Attributes;

			stream << indentation << "v" << current_attribute_arrays;
			stream << " [label=\"k3d::attribute_arrays";

			for(k3d::mesh::attribute_arrays_t::const_iterator array = Attributes.begin(); array != Attributes.end(); ++array)
				stream << "|<" << array->first << ">" << "\\\"" << array->first << "\\\"";

			stream << "\"]\n";
			stream << indentation << "v" << current_named_attribute_arrays << ":" << Name << ":e -> " << "v" << current_attribute_arrays << ":w\n";
		}

		void attribute_array(const k3d::string_t& Name, const k3d::array& Array)
		{
			current_array = &Array;

			stream << indentation << "v" << current_array << " [label=\"{|||...|}\"]\n";
			stream << indentation << "v" << current_attribute_arrays << ":" << Name << ":e -> " << "v" << current_array << ":w\n";

			current_array = 0;
		}

		void finish_attributes(const k3d::string_t& Name, const k3d::mesh::attribute_arrays_t& Attributes)
		{
			current_attribute_arrays = 0;
		}

		void finish_named_attributes(const k3d::mesh::named_attribute_arrays_t& NamedAttributes)
		{
			stream << k3d::pop_indent;
			current_named_attribute_arrays = 0;
		}

		void finish_primitive(const k3d::mesh::primitive& Primitive)
		{
			stream << k3d::pop_indent;
			current_primitive = 0;
		}

		void finish_mesh(const k3d::mesh&)
		{
			stream << k3d::pop_indent;
			stream << k3d::pop_indent;
			stream << indentation << "}\n";
			current_mesh = 0;
		}

	private:
		const k3d::mesh* current_mesh;
		const k3d::mesh::primitive* current_primitive;
		const k3d::mesh::named_arrays_t* current_topology_arrays;
		const k3d::mesh::named_attribute_arrays_t* current_named_attribute_arrays;
		const k3d::mesh::attribute_arrays_t* current_attribute_arrays;
		const k3d::array* current_array;
		std::ostream& stream;
	};

	void on_save_dotfile(const k3d::mesh* const Mesh)
	{
		k3d::filesystem::path output_path;
		{
			libk3dngui::file_chooser_dialog dialog(_("Save Mesh as GraphViz .dot file:"), "dotfile", Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(output_path))
				return;
		}

		k3d::filesystem::ofstream stream(output_path);
		k3d::visit(*Mesh, save_dotfile(stream));
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

