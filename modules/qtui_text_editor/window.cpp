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
#include <k3dsdk/classes.h>
#include <k3dsdk/data.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/idocument_sink.h>
#include <k3dsdk/iproperty_sink.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/istate_recorder_sink.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/mime_types.h>
#include <k3dsdk/module.h>
#include <k3dsdk/options.h>
#include <k3dsdk/property.h>
#include <k3dsdk/state_change_set.h>

#include <QEventLoop>
#include <QLabel>

#include <boost/assign/list_of.hpp>

#include <sstream>

namespace module
{

namespace qtui
{

namespace text_editor
{

window::window() :
	cursor_position(0),
	application_widget(*this)
{
	ui.setupUi(this);
	cursor_position = new QLabel();
	statusBar()->addPermanentWidget(cursor_position);

	update_title();
	update_widgets();
	update_cursor_position();

	connect(ui.file_close, SIGNAL(activated()), this, SLOT(close()));
	connect(ui.file_ok, SIGNAL(activated()), this, SLOT(modal_accept()));
	connect(ui.file_cancel, SIGNAL(activated()), this, SLOT(modal_reject()));
	connect(ui.edit_undo, SIGNAL(activated()), ui.text_edit, SLOT(undo()));
	connect(ui.edit_redo, SIGNAL(activated()), ui.text_edit, SLOT(redo()));
	connect(ui.text_edit, SIGNAL(undoAvailable(bool)), ui.edit_undo, SLOT(setEnabled(bool)));
	connect(ui.text_edit, SIGNAL(redoAvailable(bool)), ui.edit_redo, SLOT(setEnabled(bool)));
	connect(ui.text_edit->document(), SIGNAL(modificationChanged(bool)), this, SLOT(update_title()));
	connect(ui.text_edit, SIGNAL(cursorPositionChanged()), this, SLOT(update_cursor_position()));
	connect(ui.modal_controls, SIGNAL(accepted()), this, SLOT(modal_accept()));
	connect(ui.modal_controls, SIGNAL(rejected()), this, SLOT(modal_reject()));
}

window::~window()
{
k3d::log() << debug << __PRETTY_FUNCTION__ << std::endl;
}

k3d::bool_t window::modal_edit(const QString& Label, QString& Text)
{
	QEventLoop event_loop;
	modal = &event_loop;
	modal_label = Label;

	ui.text_edit->setPlainText(Text);

	update_title();
	update_widgets();
	update_cursor_position();

	setWindowFlags(Qt::Dialog);
	setWindowModality(Qt::ApplicationModal);
	show();

	if(0 == event_loop.exec())
	{
		Text = ui.text_edit->toPlainText();
		return true;
	}
	

	return false;
}

/*
void window::set_text(const k3d::string_t& Text)
{
	const k3d::mime::type mime_type = k3d::mime::type::lookup(Text);

#ifdef K3D_GTKSOURCEVIEW_FOUND
	GtkSourceLanguageManager* const language_manager =  gtk_source_language_manager_get_default();
	GtkSourceLanguage* const language = gtk_source_language_manager_guess_language(language_manager, 0, mime_type.str().c_str());
	GtkSourceBuffer* const buffer = GTK_SOURCE_BUFFER(m_view->get_buffer()->gobj());
	gtk_source_buffer_set_language(buffer, language);
	gtk_source_buffer_set_highlight_syntax(buffer, true);
#endif // K3D_GTKSOURCEVIEW_FOUND

	m_view->get_buffer()->set_text(Text);
	m_view->get_buffer()->set_modified(false);
}

void window::set_text(const k3d::filesystem::path& Path)
{
	const k3d::mime::type mime_type = k3d::mime::type::lookup(Path);

#ifdef K3D_GTKSOURCEVIEW_FOUND
	GtkSourceLanguageManager* const language_manager =  gtk_source_language_manager_get_default();
	GtkSourceLanguage* const language = gtk_source_language_manager_guess_language(language_manager, 0, mime_type.str().c_str());
	GtkSourceBuffer* const buffer = GTK_SOURCE_BUFFER(m_view->get_buffer()->gobj());
	gtk_source_buffer_set_language(buffer, language);
	gtk_source_buffer_set_highlight_syntax(buffer, true);
#endif // K3D_GTKSOURCEVIEW_FOUND

	k3d::filesystem::igzstream stream(Path);
	std::stringstream text;
	stream.get(*text.rdbuf(), '\0');

	m_view->get_buffer()->set_text(text.str().c_str());
	m_view->get_buffer()->set_modified(false);
}

void window::set_path(const k3d::filesystem::path& Path)
{
	m_path = Path;
	update_title();
}

void window::on_new()
{
	set_text("");
	update_title();
}

void window::on_open()
{
	k3d::filesystem::path filepath;
	{
		k3d::qtui::file_chooser_dialog dialog(_("Open File:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
		if(!dialog.get_file_path(filepath))
			return;
	}

	set_text(filepath);
	set_path(filepath);
}

void window::on_save()
{
	if(m_path.empty())
	{
		on_save_as();
		return;
	}

	k3d::filesystem::ofstream stream(m_path);
	stream << m_view->get_buffer()->get_text();

	m_view->get_buffer()->set_modified(false);
	update_title();
}

bool window::on_save_as()
{
	{
		k3d::qtui::file_chooser_dialog dialog(_("Save Script As::"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		if(!dialog.get_file_path(m_path))
			return false;
	}

	k3d::filesystem::ofstream stream(m_path);
	stream << m_view->get_buffer()->get_text();

	m_view->get_buffer()->set_modified(false);
	set_path(m_path);

	return true;
}

void window::on_revert()
{
	if(m_property)
	{
		set_text(m_property ? k3d::property::internal_value<k3d::string_t>(*m_property) : "");
	}
	else if(m_path.empty())
	{
		set_text("");
	}
	else
	{
		set_text(m_path);
	}
}

void window::on_import()
{
	k3d::filesystem::path path;
	{
		k3d::qtui::file_chooser_dialog dialog(_("Import File:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
		if(!dialog.get_file_path(path))
			return;
	}

	set_text(path);
}

void window::on_export()
{
	k3d::filesystem::path path;
	{
		k3d::qtui::file_chooser_dialog dialog(_("Export File:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		if(!dialog.get_file_path(path))
			return;
	}

	k3d::filesystem::ofstream stream(path);
	stream << m_view->get_buffer()->get_text();
}

void window::on_file_commit()
{
	return_if_fail(property);

	if(state_recorder)
		state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	k3d::property::set_internal_value(*property, ui.text_edit->toPlainText().toAscii().data());

	if(state_recorder)
		state_recorder->commit_change_set(state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), property->property_name(), K3D_CHANGE_SET_CONTEXT);
}

void window::on_execute()
{
	const k3d::script::code code(m_view->get_buffer()->get_text());
	const k3d::mime::type mime_type = k3d::mime::type::lookup(code.source());

	if(mime_type.empty())
	{
		k3d::qtui::error_message(
			_("Unknown script language"),
			_("Could not identify the MIME-type for this script. "
			" K-3D supports multiple scripting languages, and the MIME-type is used to "
			"match scripts to the correct script engines. "
			"You can add a \"magic token\" at the beginning of a script to "
			"force identification of its MIME-type - for example, add \"#python\" at the "
			"beginning of a Python script."
			)); 
		return;
	}

	const k3d::string_t name = get_title();

	k3d::iscript_engine::context context;
	if(m_document)
		context["document"] = m_document;

	m_running = true;
	update_title();

	k3d::ngui::execute_script(code, name, context);

	m_running = false;
	update_title();
}
*/

void window::update_title()
{
	QString title;

	if(modal)
	{
		title = modal_label;
	}

/*
	if(property)
	{
		if(property->property_node())
		{
			title = tr("%1 \"%2\" property").arg(property->property_node()->name().c_str()).arg(property->property_name().c_str());
		}
		else
		{
			title = tr("%1").arg(property->property_name().c_str());
		}
	}
	else if(!path.empty())
	{
		title = path.leaf().raw().c_str();
	}
	else
	{
		title = tr("Untitled");
	}
*/

	if(ui.text_edit->document()->isModified())
		title += _(" [changed]");

	setWindowTitle(title);
}

void window::update_widgets()
{
	ui.file_close->setVisible(!modal);
	ui.modal_controls->setVisible(modal);
}

void window::update_cursor_position()
{
	QTextCursor cursor = ui.text_edit->textCursor();
	cursor_position->setText(tr("Line: %1 Column: %2")
		.arg(ui.text_edit->toPlainText().left(cursor.position()).count("\n") + 1)
		.arg(cursor.columnNumber() + 1));
}

void window::modal_accept()
{
	return_if_fail(modal);
	modal->exit(0);
}

void window::modal_reject()
{
	return_if_fail(modal);
	modal->exit(1);
}

k3d::iplugin_factory& window::get_factory()
{
	static k3d::application_plugin_factory<window> factory(
		k3d::uuid(0x484be321, 0x754dfc58, 0x44260385, 0x37c8b30a),
		"QTUITextEditor",
		_("Provides a general-purpose editor for editing source-code, text, scripts, and shaders."),
		"QTUI",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "dialog")
		);

	return factory;
}

void window::closeEvent(QCloseEvent* Event)
{
	if(modal)
	{
		modal_reject();
	}

	base::closeEvent(Event);
}

} // namespace text_editor

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::text_editor::window::get_factory());
K3D_MODULE_END

