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
#include <k3dsdk/mesh.h>
#include <k3dsdk/properties.h>

#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>

#include <boost/assign/list_of.hpp>

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

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent, k3d::iproperty& Property)
	{
		k3d::mesh::knots_t knot_vector = boost::any_cast<k3d::mesh::knots_t >(k3d::property::internal_value(Property));
		//get the number of knots
		int m_nr_knots = knot_vector.size();

		k3d::log() << debug << m_nr_knots << std::endl;
		//initialize all spin buttons
		for(int i = 0; i < m_nr_knots; i++)
		{
			Gtk::SpinButton* btn = Gtk::manage(new Gtk::SpinButton(0.1,1));
			//add callback
			btn->signal_value_changed().connect(sigc::mem_fun(*this, &control::on_change));
			k3d::log() << debug << knot_vector[i] << std::endl;
			btn->set_editable();
			btn->set_value(knot_vector[i]);
			pack_start(*btn, Gtk::PACK_EXPAND_WIDGET, 1);
			m_knots.push_back(btn);
		}
		
		show_all_children();
	}
	
	void on_change()
	{
		//pass new data to nurbs plugin?
		k3d::log() << debug << "Value changed" << std::endl;
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
	
private:
	std::vector<Gtk::SpinButton* > m_knots;
	int m_nr_knots;
};

} // namespace knot_vector

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::knot_vector::control::get_factory());
K3D_MODULE_END

