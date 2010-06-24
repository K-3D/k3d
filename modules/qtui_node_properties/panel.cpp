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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "panel.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/focus.h>
#include <k3dsdk/qtui/property_widget_factory.h>

#include <boost/assign/list_of.hpp>

#include <QFormLayout>
#include <QLabel>

namespace module
{

namespace qtui
{

namespace node_properties
{

panel::panel() :
	document(0)
{
	ui.setupUi(this);
}

void panel::initialize(k3d::idocument& Document)
{
	document = &Document;
  k3d::qtui::focus::mediator(Document).connect_focus_node_signal(sigc::mem_fun(*this, &panel::on_view_node_properties));
}

void panel::on_view_node_properties(k3d::inode* const Node, k3d::iunknown* const Sender)
{
  QObjectList children = ui.properties->children();
  for(int i = 0; i != children.size(); ++i)
    delete children[i];

  QFormLayout* const layout = new QFormLayout(ui.properties);

	if(k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(Node))
  {
    const k3d::iproperty_collection::properties_t& properties = property_collection->properties();
    k3d::qtui::property_widget_factory property_widget_factory;

    for(int i = 0; i != properties.size(); ++i)
    {
      k3d::iproperty* const property = properties[i];

      QLabel* const property_label = new QLabel(k3d::convert<QString>(property->property_label()));
      property_label->setToolTip(k3d::convert<QString>(property->property_description()));

      QWidget* const property_widget = property_widget_factory.create(*document, *property);
      property_widget->setToolTip(k3d::convert<QString>(property->property_description()));

      layout->addRow(property_label, property_widget);
    }
  }
}

k3d::iplugin_factory& panel::get_factory()
{
	static k3d::application_plugin_factory<panel> factory(
		k3d::uuid(0x765f717b, 0xf73a425e, 0xa83d927b, 0x4f27ccdc),
		"QTUINodePropertiesPanel",
		_("Displays document node properties."),
		"QTUI Panel",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "panel")("qtui:panel-label", "Node Properties")
		);

	return factory;
}

} // namespace node_properties

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::node_properties::panel::get_factory());
K3D_MODULE_END

