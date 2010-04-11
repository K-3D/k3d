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

#include "canvas.h"
#include "main_window.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/node.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/share.h>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include <boost/scoped_ptr.hpp>

#include <iomanip>

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// main_window
	
main_window::main_window(QApplication& Application) :
	m_document(0)
{
	setWindowTitle("K-3D Sample Qt User Interface");

	QMenu* const fileMenu = this->menuBar()->addMenu(tr("&File"));

	QAction* const fileOpenAction = fileMenu->addAction(
		QPixmap((k3d::share_path() / k3d::filesystem::generic_path("qtui/stock_open.png")).native_filesystem_string().c_str()),
		"&Open...");
	connect(fileOpenAction, SIGNAL(activated()), this, SLOT(on_file_open()));

	QAction* const fileQuitAction = fileMenu->addAction(
		QPixmap((k3d::share_path() / k3d::filesystem::generic_path("qtui/stock_exit.png")).native_filesystem_string().c_str()),
		"&Quit");
	connect(fileQuitAction, SIGNAL(activated()), &Application, SLOT(quit()));

	QToolBar* const viewTools = new QToolBar("View Operations", this);
	addToolBar(Qt::TopToolBarArea, viewTools);

	m_camera_combo = new QComboBox(viewTools);
	viewTools->addWidget(m_camera_combo);
	m_camera_combo->setEnabled(false);
	connect(m_camera_combo, SIGNAL(activated(int)), this, SLOT(on_camera_changed(int)));

	m_render_engine_combo = new QComboBox(viewTools);
	viewTools->addWidget(m_render_engine_combo);
	m_render_engine_combo->setEnabled(false);
	connect(m_render_engine_combo, SIGNAL(activated(int)), this, SLOT(on_render_engine_changed(int)));

	m_viewport_scene = new viewport_scene();
	m_viewport_view = new viewport_view(this);
	m_viewport_view->setScene(m_viewport_scene);

	connect(this, SIGNAL(camera_changed(k3d::icamera* const)), m_viewport_scene, SLOT(on_camera_changed(k3d::icamera* const)));
	connect(this, SIGNAL(render_engine_changed(k3d::gl::irender_viewport* const)), m_viewport_scene, SLOT(on_render_engine_changed(k3d::gl::irender_viewport* const)));

	setCentralWidget(m_viewport_view);
}

void main_window::on_file_open()
{
	boost::scoped_ptr<k3d::idocument_importer> importer(k3d::plugin::create<k3d::idocument_importer>(k3d::classes::DocumentImporter()));
	if(!importer.get())
	{
		QMessageBox::warning(this, _("Open K-3D Document:"), _("Document reader plugin not installed."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	const QString filepath = QFileDialog::getOpenFileName(this, _("Choose a file to open:"), "/home", _("K-3D Documents (*.k3d)"));
	if(filepath.isEmpty())
		return;

	const k3d::filesystem::path document_path = k3d::filesystem::native_path(k3d::ustring::from_utf8(filepath.toAscii().data()));
	
	if(m_document)
	{
		k3d::application().close_document(*m_document);
		m_document = 0;
	}
	
	m_document = k3d::application().create_document();
	return_if_fail(m_document);

	if(!importer->read_file(document_path, *m_document))
	{
		QMessageBox::warning(this, _("Open K-3D Document:"), _("Error reading document."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	setWindowTitle(("K-3D Sample Qt User Interface - " + document_path.leaf().raw()).c_str());
	statusBar()->showMessage(("Loaded document " + document_path.leaf().raw()).c_str(), 0);

	m_cameras = k3d::node::lookup<k3d::icamera>(*m_document);
	m_render_engines = k3d::node::lookup<k3d::gl::irender_viewport>(*m_document);

	m_camera_combo->clear();
	m_camera_combo->setEnabled(m_cameras.size());
	for(std::vector<k3d::icamera*>::iterator camera = m_cameras.begin(); camera != m_cameras.end(); ++camera)
		m_camera_combo->addItem(dynamic_cast<k3d::inode*>(*camera)->name().c_str());
	m_camera_combo->adjustSize();

	m_render_engine_combo->clear();
	m_render_engine_combo->setEnabled(m_render_engines.size());
	for(std::vector<k3d::gl::irender_viewport*>::iterator render_engine = m_render_engines.begin(); render_engine != m_render_engines.end(); ++render_engine)
		m_render_engine_combo->addItem(dynamic_cast<k3d::inode*>(*render_engine)->name().c_str());
	m_render_engine_combo->adjustSize();
	
	Q_EMIT camera_changed(m_cameras.size() ? m_cameras[0] : 0);
	Q_EMIT render_engine_changed(m_render_engines.size() ? m_render_engines[0] : 0);
}

void main_window::on_camera_changed(int Index)
{
	Q_EMIT camera_changed(m_cameras[Index]);
}

void main_window::on_render_engine_changed(int Index)
{
	Q_EMIT render_engine_changed(m_render_engines[Index]);
}
	

} // namespace qtui

} // namespace module

