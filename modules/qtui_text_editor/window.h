#ifndef MODULES_QTUI_TEXT_EDITOR_WINDOW_H
#define MODULES_QTUI_TEXT_EDITOR_WINDOW_H

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

#include "ui_window.h"

#include <k3dsdk/iunknown.h>
#include <k3dsdk/qtui/application_widget.h>
#include <k3dsdk/qtui/modal_text_editor.h>

#include <QMainWindow>
#include <QPointer>

namespace k3d { class iplugin_factory; }

class QEventLoop;
class QLabel;

namespace module
{

namespace qtui
{

namespace text_editor
{

class window :
	public QMainWindow,
	public k3d::qtui::modal_text_editor,
	public k3d::iunknown
{
	Q_OBJECT

	typedef QMainWindow base;

public:
	window();
	~window();

	k3d::bool_t modal_edit(const QString& Label, QString& Text);

	static k3d::iplugin_factory& get_factory();

private Q_SLOTS:
/*
	void on_file_commit();
	void set_text(const k3d::string_t& Text);
	void set_text(const k3d::filesystem::path& Path);
	void set_path(const k3d::filesystem::path& Path);
	void on_new();
	void on_open();
	void on_save();
	bool on_save_as();
	void on_revert();
	void on_file_import();
	void on_file_export();
	void on_execute();
*/
	void update_title();
	void update_widgets();
	void update_cursor_position();

	void modal_accept();
	void modal_reject();

private:
	void closeEvent(QCloseEvent* Event);

	Ui::QTUITextEditor ui;
	QLabel* cursor_position;
	k3d::qtui::application_widget application_widget;

	QPointer<QEventLoop> modal;
	QString modal_label;
};

} // namespace text_editor

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_TEXT_EDITOR_WINDOW_H

