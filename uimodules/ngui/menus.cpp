// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "icons.h"
#include "image_menu_item.h"
#include "menus.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/accellabel.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>

#include <boost/format.hpp>

namespace libk3dngui
{

namespace detail
{

const std::string plugin_factory_markup(k3d::iplugin_factory& Factory)
{
	std::string markup;

	if(k3d::iplugin_factory::EXPERIMENTAL == Factory.quality())
	{
		markup = k3d::string_cast(boost::format(_("<span color=\"blue\">%1% (Experimental)</span>")) % Factory.name());
	}
	else if(k3d::iplugin_factory::DEPRECATED == Factory.quality())
	{
		markup = k3d::string_cast(boost::format(_("<span color=\"red\" strikethrough=\"true\">%1%</span><span color=\"red\"> (Deprecated)</span>")) % Factory.name());
	}
	else
	{
		markup = Factory.name();
	}

	return markup;
}

} // namespace detail

image_menu_item::control* create_menu_item(k3d::icommand_node& Parent, const std::string& NamePrefix, k3d::iplugin_factory& Factory)
{
	Gtk::Image* const image = new Gtk::Image(quiet_load_icon(Factory.name(), Gtk::ICON_SIZE_MENU));

	image_menu_item::control* const menu_item =
		new image_menu_item::control(Parent, NamePrefix + Factory.name(),
			*Gtk::manage(image),
			"",
			true)
		<< set_tooltip(Factory.short_description());

	menu_item->get_label()->set_markup(detail::plugin_factory_markup(Factory));

	return menu_item;
}

} // namespace libk3dngui

