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
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/custom_property_control.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/spin_button.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/type_registry.h>

#include <gtkmm/label.h>
#include <gtkmm/table.h>

#include <boost/assign/list_of.hpp>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace rectangle
{

/// Adapts a spin button to control a single rectangle value
class spin_button_model :
	public k3d::ngui::spin_button::imodel
{
public:
	spin_button_model(k3d::iproperty& Property, k3d::double_t k3d::rectangle::* Value) :
		m_readable_data(Property),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Property)),
		m_value(Value)
	{
	}

	const Glib::ustring label()
	{
		Glib::ustring result = m_readable_data.property_label();

		if(m_readable_data.property_node())
			result = m_readable_data.property_node()->name() + " " + result;

		return result;
	}

	k3d::bool_t writable()
	{
		return m_writable_data;
	}

	k3d::double_t value()
	{
		return boost::any_cast<k3d::rectangle>(m_readable_data.property_internal_value()).*m_value;
	}

	void set_value(const k3d::double_t Value)
	{
		return_if_fail(m_writable_data);

		k3d::rectangle value = boost::any_cast<k3d::rectangle>(m_readable_data.property_internal_value());
		value.*m_value = Value;
		m_writable_data->property_set_value(value);
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_readable_data.property_changed_signal().connect(sigc::hide(Slot));
	}

	k3d::double_t step_increment()
	{
		return 0.1;
	}

	const std::type_info& units()
	{
		return typeid(k3d::measurement::scalar);
	}

private:
	k3d::iproperty& m_readable_data;
	k3d::iwritable_property* const m_writable_data;
	k3d::double_t k3d::rectangle::* m_value;
};

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a control for k3d::rectangle properties.
class control :
	public k3d::ngui::custom_property::control,
	public k3d::iunknown,
	public Gtk::Table
{
public:
	control() :
		Gtk::Table(4, 3, false)
	{
	}

	~control()
	{
	}

	void initialize(document_state& DocumentState, k3d::iproperty& Property)
	{
		spin_button::control* const x1 = new spin_button::control(new spin_button_model(Property, &k3d::rectangle::x1), &DocumentState.document().state_recorder());
		spin_button::control* const x2 = new spin_button::control(new spin_button_model(Property, &k3d::rectangle::x2), &DocumentState.document().state_recorder());
		spin_button::control* const y1 = new spin_button::control(new spin_button_model(Property, &k3d::rectangle::y1), &DocumentState.document().state_recorder());
		spin_button::control* const y2 = new spin_button::control(new spin_button_model(Property, &k3d::rectangle::y2), &DocumentState.document().state_recorder());

		attach(*Gtk::manage(new Gtk::Label(_("X1"))), 0, 1, 0, 1);
		attach(*Gtk::manage(x1), 1, 2, 0, 1);

		attach(*Gtk::manage(new Gtk::Label(_("X2"))), 0, 1, 1, 2);
		attach(*Gtk::manage(x2), 1, 2, 1, 2);

		attach(*Gtk::manage(new Gtk::Label(_("Y1"))), 0, 1, 2, 3);
		attach(*Gtk::manage(y1), 1, 2, 2, 3);

		attach(*Gtk::manage(new Gtk::Label(_("Y2"))), 0, 1, 3, 4);
		attach(*Gtk::manage(y2), 1, 2, 3, 4);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<control> factory(
			k3d::uuid(0x85a9b466, 0x6541a591, 0x69de379e, 0x90a31786),
			"NGUIRectangleControl",
			_("Provides a custom property control for k3d::rectangle properties."),
			"NGUI Control",
			k3d::iplugin_factory::STABLE,
			boost::assign::map_list_of("ngui:component-type", "property-control")("ngui:property-type", "k3d::rectangle"));

		return factory;
	}
};

} // namespace rectangle

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::rectangle::control::get_factory());
K3D_MODULE_END

