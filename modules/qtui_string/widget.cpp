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

#include "widget.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/module.h>
#include <k3dsdk/property.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/result.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/type_registry.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace qtui
{

namespace string
{

/////////////////////////////////////////////////////////////////////////////
// widget

widget::widget() :
  property(0),
  writable_property(0),
  state_recorder(0)
{
}

void widget::initialize(k3d::idocument& Document, k3d::iproperty& Property, k3d::istate_recorder* StateRecorder)
{
  property = 0;
  writable_property = 0;
  state_recorder = 0;
  change_connection.disconnect();
  setEnabled(false);

  return_if_fail(Property.property_type() == typeid(k3d::string_t));

  property = &Property;
  writable_property = dynamic_cast<k3d::iwritable_property*>(&Property);
  state_recorder = StateRecorder;
  change_connection = property->property_changed_signal().connect(sigc::hide(sigc::mem_fun(*this, &widget::on_property_changed)));
  setEnabled(writable_property);

  on_property_changed();
  QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(on_editing_finished()));
}

void widget::on_editing_finished()
{
  return_if_fail(writable_property);

  if(state_recorder)
    state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

  writable_property->property_set_value(k3d::convert<k3d::string_t>(text()));

  if(state_recorder)
    state_recorder->commit_change_set(state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), "Change " + property->property_label(), K3D_CHANGE_SET_CONTEXT);
}

void widget::on_property_changed()
{
  setText(k3d::convert<QString>(boost::any_cast<k3d::string_t>(property->property_internal_value())));
}

k3d::iplugin_factory& widget::get_factory()
{
  static k3d::application_plugin_factory<widget> factory(
   k3d::uuid(0x4f137967, 0xe5434075, 0x88743f39, 0xe83be9c2),
    "QTUIStringWidget",
    _("Provides a custom property widget for k3d::string_t properties."),
    "QTUI Widget",
    k3d::iplugin_factory::EXPERIMENTAL,
    boost::assign::map_list_of("qtui:component-type", "property-widget")("qtui:property-type", "k3d::string_t"));

  return factory;
}

} // namespace string

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::string::widget::get_factory());
K3D_MODULE_END

