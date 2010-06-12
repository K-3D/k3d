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
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace qtui
{

namespace node_list
{

panel::panel() :
	document(0)
{
	ui.setupUi(this);
	ui.list->setModel(&model);
}

void panel::initialize(k3d::idocument& Document)
{
	document = &Document;

	model.add_nodes(Document.nodes().collection());

	Document.nodes().add_nodes_signal().connect(sigc::mem_fun(model, &k3d::qtui::node_list_model::add_nodes));
	Document.nodes().remove_nodes_signal().connect(sigc::mem_fun(model, &k3d::qtui::node_list_model::remove_nodes));
}

k3d::iplugin_factory& panel::get_factory()
{
	static k3d::application_plugin_factory<panel> factory(
		k3d::uuid(0x460f8377, 0xe5034177, 0xb7080ea4, 0xbc23bf0a),
		"QTUINodeListPanel",
		_("Displays document nodes as a flat list."),
		"QTUI Panel",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "panel")("qtui:panel-label", "Node List")
		);

	return factory;
}

} // namespace node_list

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::node_list::panel::get_factory());
K3D_MODULE_END

