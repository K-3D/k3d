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
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/custom_property_control.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/entry.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>

#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>

#include <boost/assign/list_of.hpp>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace knot_vector
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

	/// Implementation of entry::imodel for use with knot vectors
	class knot_vector_model :
		public entry::imodel
	{
	public:
		knot_vector_model(k3d::iproperty& Data) :
			m_readable_data(Data),
			m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data))
		{
		}

		const Glib::ustring label()
		{
			Glib::ustring result = m_readable_data.property_label();

			if(m_readable_data.property_node())
				result = m_readable_data.property_node()->name() + " " + result;

			return result;
		}

		const k3d::string_t value()
		{
			const std::type_info& type = m_readable_data.property_type();

			if(type == typeid(k3d::mesh::knots_t))
			{
				const k3d::mesh::knots_t knots = boost::any_cast<k3d::mesh::knots_t>(m_readable_data.property_internal_value());

				std::ostringstream buffer;
				std::copy(knots.begin(), knots.end(), std::ostream_iterator<double>(buffer, " "));

				return buffer.str();
			}
			
			k3d::log() << error << k3d_file_reference << ": unsupported property type: " << k3d::demangle(type) << std::endl;
			return k3d::string_t();
		}

		void set_value(const k3d::string_t& Value)
		{
			return_if_fail(m_writable_data);

			const std::type_info& type = m_readable_data.property_type();

			if(type == typeid(k3d::mesh::knots_t))
			{
				double knot;
				k3d::mesh::knots_t knots;
				std::istringstream buffer(Value);
				for(buffer >> knot; buffer; buffer >> knot)
					knots.push_back(knot);

				m_writable_data->property_set_value(knots);
				return;
			}

			k3d::log() << error << k3d_file_reference << ": unsupported property type: " << k3d::demangle(type) << std::endl;
		}

		sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
		{
			return m_readable_data.property_changed_signal().connect(sigc::hide(Slot));
		}

	private:
		k3d::iproperty& m_readable_data;
		k3d::iwritable_property* const m_writable_data;
	};

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent, k3d::iproperty& Property)
	{
		entry::control* const control = new entry::control(Parent, "knot_vector", new knot_vector_model(Property), &DocumentState.document().state_recorder());
		pack_start(*Gtk::manage(control), Gtk::PACK_EXPAND_WIDGET);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<control> factory(
			k3d::uuid(0xade18285, 0x9a4333fc, 0x7449f382, 0x39628d15),
			"NGUIKnotVectorControl",
			_("Provides a custom property control for NURBS knot vectors."),
			"NGUI Control",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "property-control")("ngui:property-type", "k3d:nurbs-knot-vector"));

		return factory;
	}
};

} // namespace knot_vector

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::knot_vector::control::get_factory());
K3D_MODULE_END

