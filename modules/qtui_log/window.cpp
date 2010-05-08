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

#include "window.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/module.h>
#include <k3dsdk/qtui/convert.h>

#include <boost/assign/list_of.hpp>

#include <QDateTime>

namespace module
{

namespace qtui
{

namespace log
{

window::window() :
	application_widget(*this)
{
	ui.setupUi(this);

	k3d::get_log_cache(sigc::mem_fun(*this, &window::on_log_message));
	k3d::connect_log_message(sigc::mem_fun(*this, &window::on_log_message));

	this->setAttribute(Qt::WA_DeleteOnClose);
}

void window::on_log_message(const time_t Timestamp, const k3d::log_level_t Level, const std::string& Message)
{
	QString buffer;

	buffer += "<span style='color: grey; font-size: small;'>";
	buffer += QDateTime::fromTime_t(Timestamp).toString(Qt::ISODate);
	buffer += "</span>";

	switch(Level)
	{
	case k3d::K3D_LOG_LEVEL_CRITICAL:
		buffer += "<span style='color: darkred; font-weight:bold;'>";
		break;
	case k3d::K3D_LOG_LEVEL_ERROR:
		buffer += "<span style='color: red; font-weight:bold;'>";
		break;
	case k3d::K3D_LOG_LEVEL_WARNING:
		buffer += "<span style='color: orange;'>";
		break;
	case k3d::K3D_LOG_LEVEL_INFO:
		buffer += "<span>";
		break;
	case k3d::K3D_LOG_LEVEL_DEBUG:
		buffer += "<span style='color: green;'>";
		break;
	default:
		break;
	}

	switch(Level)
	{
	case k3d::K3D_LOG_LEVEL_CRITICAL:
		buffer += " CRITICAL: ";
		break;
	case k3d::K3D_LOG_LEVEL_ERROR:
		buffer += " ERROR: ";
		break;
	case k3d::K3D_LOG_LEVEL_WARNING:
		buffer += " WARNING: ";
		break;
	case k3d::K3D_LOG_LEVEL_INFO:
		buffer += " INFO: ";
		break;
	case k3d::K3D_LOG_LEVEL_DEBUG:
		buffer += " DEBUG: ";
		break;
	}

	buffer += k3d::convert<QString>(Message);

	buffer += "</span>";

	ui.console->print_string(buffer);
}

k3d::iplugin_factory& window::get_factory()
{
	static k3d::application_plugin_factory<window> factory(
		k3d::uuid(0xb7ca1696, 0x3d4c5f65, 0xf379149e, 0xd3412d61),
		"QTUILog",
		_("Displays the contents of the K-3D log"),
		"QTUI",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "dialog")
		);

	return factory;
}

} // namespace log

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::log::window::get_factory());
K3D_MODULE_END

