#ifndef MODULES_QTUI_STRING_WIDGET_H
#define MODULES_QTUI_STRING_WIDGET_H

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

#include <k3dsdk/qtui/property_widget.h>
#include <k3dsdk/signal_system.h>
#include <QLineEdit>

namespace k3d { class iplugin_factory; }
namespace k3d { class iwritable_property; }

namespace module
{

namespace qtui
{

namespace string
{

/////////////////////////////////////////////////////////////////////////////
// widget

/// Provides a widget for k3d::bool_t properties.
class widget :
  public QLineEdit,
	public k3d::qtui::property_widget,
	public k3d::iunknown
{
  Q_OBJECT;

public:
  widget();

	void initialize(k3d::idocument& Document, k3d::iproperty& Property, k3d::istate_recorder* StateRecorder);

	static k3d::iplugin_factory& get_factory();

public Q_SLOTS:
  void on_editing_finished();

private:
  void on_property_changed();

  k3d::iproperty* property;
  k3d::iwritable_property* writable_property;
  k3d::istate_recorder* state_recorder;
  sigc::connection change_connection;
};

} // namespace string

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_STRING_WIDGET_H
