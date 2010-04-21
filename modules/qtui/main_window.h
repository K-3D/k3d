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

#include <boost/scoped_ptr.hpp>

namespace k3d { class idocument; }
namespace k3d { class iplugin_factory; }
namespace k3d { namespace filesystem { class path; } }

namespace module
{

namespace qtui
{

class scene;

//////////////////////////////////////////////////////////////////////////
// main_window
	
class main_window :
	public QMainWindow
{
	Q_OBJECT

public:
	main_window(k3d::idocument& Document);
	main_window(const k3d::filesystem::path& DocumentPath, k3d::idocument& Document);
	~main_window();

private Q_SLOTS:
	void on_file_new();
	void on_file_open();

private:
	void initialize(k3d::idocument& Document);

	void on_edit_mode(k3d::iplugin_factory* const Mode);

	Ui::main_window ui;
	boost::scoped_ptr<scene> m_scene;
};
	
} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_MAIN_WINDOW_H

