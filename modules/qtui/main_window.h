#ifndef MODULES_QTUI_MAIN_WINDOW_H
#define MODULES_QTUI_MAIN_WINDOW_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <QMainWindow>
#include <ui_main_window.h>

namespace k3d { class idocument; }

namespace module
{

namespace qtui
{

class viewport_scene;
class viewport_view;

//////////////////////////////////////////////////////////////////////////
// main_window
	
class main_window :
	public QMainWindow
{
	Q_OBJECT

public:
	main_window(QApplication& Application);

Q_SIGNALS:
	void document_changed(k3d::idocument&);
	
private Q_SLOTS:
	void on_file_open();

private:
	Ui::main_window ui;
	viewport_view* m_viewport_view;
	viewport_scene* m_viewport_scene;
	k3d::idocument* m_document;
};
	
} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_MAIN_WINDOW_H

