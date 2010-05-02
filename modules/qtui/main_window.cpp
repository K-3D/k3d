// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include "main_window.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/qtui/action.h>
#include <k3dsdk/qtui/application.h>
#include <k3dsdk/qtui/document.h>
#include <k3dsdk/qtui/file_dialog.h>
#include <k3dsdk/share.h>

#include <QAction>
#include <QDialog>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include <boost/scoped_ptr.hpp>

#include <iomanip>

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// main_window
	
main_window::main_window(k3d::idocument& Document) :
	document_widget(*this, Document)
{
	initialize(Document);
}

main_window::main_window(const k3d::filesystem::path& DocumentPath, k3d::idocument& Document) :
	document_widget(*this, Document)
{
	initialize(Document);
	setWindowTitle(("K-3D - " + DocumentPath.leaf().raw()).c_str());
}

void main_window::on_file_new_activated()
{
	k3d::idocument* const document = k3d::application().create_document();
	return_if_fail(document);

	k3d::qtui::populate_new_document(*document);

	main_window* const window = new main_window(*document);
	window->show();
}

void main_window::on_file_open_activated()
{
	boost::scoped_ptr<k3d::idocument_importer> importer(k3d::plugin::create<k3d::idocument_importer>(k3d::classes::DocumentImporter()));
	if(!importer.get())
	{
		QMessageBox::warning(this, _("Open K-3D Document:"), _("Document reader plugin not installed."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	const k3d::filesystem::path document_path = k3d::qtui::file_dialog::get_open_filename(this, tr("Choose a file to open:"), k3d::options::path::documents(), tr("K-3D Documents (*.k3d)"));
	if(document_path.empty())
		return;

	k3d::idocument* const document = k3d::application().create_document();
	return_if_fail(document);

	if(!importer->read_file(document_path, *document))
	{
		QMessageBox::warning(this, _("Open K-3D Document:"), _("Error reading document."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	main_window* const window = new main_window(document_path, *document);
	window->show();
}

void main_window::on_file_quit_activated()
{
	k3d::qtui::application::instance().close();
}

void main_window::on_help_about_activated()
{
	QDialog* const dialog = k3d::plugin::create<QDialog>("QTUIAboutDialog");
	return_if_fail(dialog);
	dialog->show();
}

void main_window::initialize(k3d::idocument& Document)
{
	ui.setupUi(this);
	ui.file_new->setIcon(QIcon::fromTheme("document-new"));
	ui.file_open->setIcon(QIcon::fromTheme("document-open"));
	ui.file_close->setIcon(QIcon::fromTheme("window-close"));
	ui.file_quit->setIcon(QIcon::fromTheme("application-exit"));
	ui.help_about->setIcon(QIcon::fromTheme("help-about"));
	
	statusBar()->setVisible(false);
	setAttribute(Qt::WA_DeleteOnClose);

	m_scene.reset(new scene(Document));
	ui.viewport->setScene(m_scene.get());

	// Setup manual access to edit modes ...
	QMenu* const mode_menu = ui.menuEdit->addMenu(tr("Mode"));
	mode_menu->addAction(new k3d::qtui::action(tr("-- None --"), mode_menu, sigc::bind(sigc::mem_fun(*this, &main_window::on_edit_mode), static_cast<k3d::iplugin_factory*>(0))));
	mode_menu->addSeparator();
	std::vector<k3d::iplugin_factory*> modes = k3d::plugin::factory::lookup("qtui:component-type", "mode");
	std::sort(modes.begin(), modes.end(), k3d::sort_by_name());
	for(int i = 0; i != modes.size(); ++i)
		mode_menu->addAction(new k3d::qtui::action(modes[i]->name().c_str(), mode_menu, sigc::bind(sigc::mem_fun(*this, &main_window::on_edit_mode), modes[i])));

	// Setup manual access to dialogs ...
	QMenu* const dialog_menu = ui.menuAdvanced->addMenu(tr("Dialogs"));
	std::vector<k3d::iplugin_factory*> dialogs = k3d::plugin::factory::lookup("qtui:component-type", "dialog");
	std::sort(dialogs.begin(), dialogs.end(), k3d::sort_by_name());
	for(int i = 0; i != dialogs.size(); ++i)
		dialog_menu->addAction(new k3d::qtui::action(dialogs[i]->name().c_str(), dialog_menu, sigc::bind(sigc::mem_fun(*this, &main_window::on_advanced_dialog), dialogs[i])));
}

void main_window::on_edit_mode(k3d::iplugin_factory* const Mode)
{
	m_scene->set_active_mode(Mode ? k3d::plugin::create<k3d::qtui::mode>(*Mode) : static_cast<k3d::qtui::mode*>(0));
}

void main_window::on_advanced_dialog(k3d::iplugin_factory* const Widget)
{
	QWidget* const widget = Widget ? k3d::plugin::create<QWidget>(*Widget) : static_cast<QWidget*>(0);
	return_if_fail(widget);
	widget->show();
}

} // namespace qtui

} // namespace module

