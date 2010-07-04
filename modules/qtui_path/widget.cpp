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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

#include "widget.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ipath_property.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/module.h>
#include <k3dsdk/path.h>
#include <k3dsdk/property.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/file_dialog.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/type_registry.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace qtui
{

namespace path
{

/////////////////////////////////////////////////////////////////////////////
// widget

widget::widget() :
	property(0),
	path_property(0),
	writable_property(0),
	state_recorder(0)
{
	ui.setupUi(this);
}

void widget::initialize(k3d::idocument& Document, k3d::iproperty& Property, k3d::istate_recorder* StateRecorder)
{
	property = 0;
	path_property = 0;
	writable_property = 0;
	state_recorder = 0;
	change_connection.disconnect();
	setEnabled(false);

	return_if_fail(Property.property_type() == typeid(k3d::filesystem::path));

	property = &Property;
	path_property = dynamic_cast<k3d::ipath_property*>(&Property);
	writable_property = dynamic_cast<k3d::iwritable_property*>(&Property);
	state_recorder = StateRecorder;
	change_connection = property->property_changed_signal().connect(sigc::hide(sigc::mem_fun(*this, &widget::on_property_changed)));
	setEnabled(writable_property);

	on_property_changed();
}

void widget::on_path_editingFinished()
{
	return_if_fail(writable_property);

	const k3d::filesystem::path new_value = k3d::filesystem::native_path(k3d::convert<k3d::ustring>(ui.path->text()));
	if(new_value == boost::any_cast<k3d::filesystem::path>(property->property_internal_value()))
		return;

	if(state_recorder)
		state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	writable_property->property_set_value(new_value);

	if(state_recorder)
		state_recorder->commit_change_set(state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), "Change " + property->property_label(), K3D_CHANGE_SET_CONTEXT);
}

void widget::on_browse_clicked()
{
	const k3d::ipath_property::mode_t path_mode = path_property ? path_property->property_path_mode() : k3d::ipath_property::READ;
	const k3d::string_t path_type = path_property ? path_property->property_path_type() : "";

	k3d::filesystem::path new_path;
	switch(path_mode)
	{
		case k3d::ipath_property::READ:
		{
			new_path = k3d::qtui::file_dialog::get_open_filename(
				this,
				QString(tr("Select %1")).arg(k3d::convert<QString>(property->property_label())),
				path_type
				); 
			break;
		}
		case k3d::ipath_property::WRITE:
		{
			new_path = k3d::qtui::file_dialog::get_save_filename(
				this,
				QString(tr("Select %1")).arg(k3d::convert<QString>(property->property_label())),
				path_type
				); 
			break;
		}
	}

	if(new_path.empty())
		return;

	ui.path->setText(k3d::convert<QString>(new_path.native_utf8_string()));
	on_path_editingFinished();
}

void widget::on_property_changed()
{
	ui.path->setText(k3d::convert<QString>(boost::any_cast<k3d::filesystem::path>(property->property_internal_value()).native_utf8_string()));
}

k3d::iplugin_factory& widget::get_factory()
{
	static k3d::application_plugin_factory<widget> factory(
	 k3d::uuid(0xafec343d, 0x804e4218, 0x85ec4130, 0x291c4ae1),
		"QTUIPathWidget",
		_("Provides a custom property widget for k3d::filesystem::path properties."),
		"QTUI Widget",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "property-widget")("qtui:property-type", "k3d::filesystem::path"));

	return factory;
}

} // namespace path

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::path::widget::get_factory());
K3D_MODULE_END

