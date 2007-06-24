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

#include "user_interface.h"

#include <k3dsdk/application.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/auto_ptr.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/module.h>
#include <k3dsdk/share.h>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include <iomanip>

namespace libk3dqtui
{

/////////////////////////////////////////////////////////////////////////
// timer_helper

timer_helper::timer_helper(const unsigned int Interval) :
	timer(new QTimer())
{
	connect(timer.get(), SIGNAL(timeout()), this, SLOT(on_timer()));
	timer->start(Interval);
}

void timer_helper::on_timer()
{
	m_signal();
}
	
//////////////////////////////////////////////////////////////////////////
// viewport

viewport::viewport(QWidget* parent) :
	QGLWidget(parent),
	m_camera(init_value<k3d::icamera*>(0)),
	m_gl_engine(init_value<k3d::gl::irender_engine*>(0)),
	m_font_begin(0),
	m_font_end(0)
{
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
}

void viewport::on_camera_changed(k3d::icamera* const Camera)
{
	m_camera.set_value(Camera);
	update();
}

void viewport::on_render_engine_changed(k3d::gl::irender_engine* const Engine)
{
	m_gl_engine.set_value(Engine);
	update();
}

void viewport::createFont()
{
	if(m_font_begin != m_font_end)
		return;

	const unsigned long begin_glyph = 0;
	const unsigned long end_glyph = 256;
	m_font_begin = glGenLists(end_glyph - begin_glyph);
	return_if_fail(m_font_begin);
	m_font_end = m_font_begin + end_glyph - begin_glyph;
}

void viewport::initializeGL()
{
	createFont();
}

void viewport::paintGL()
{
	glViewport(0, 0, width(), height());
	if(m_gl_engine.value() && m_camera.value())
	{
		k3d::timer timer;

		m_gl_engine.value()->redraw(*m_camera.value(), width(), height(), m_font_begin, m_gl_view_matrix, m_gl_projection_matrix, m_gl_viewport);

		const double elapsed = timer.elapsed();
		if(elapsed)
		{
			std::stringstream buffer;
			buffer << std::fixed << std::setprecision(1) << 1.0 / elapsed << "fps";

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1, 1, -1, 1, -1, 1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_1D);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);

			glColor3d(0, 0, 0);
			renderText(-0.95, -0.95, 0.0, buffer.str().c_str());
		}
	}
	else
	{
		glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glColor3d(0, 0, 0);
		renderText(-0.95, 0.0, 0.0, _("Use File > Open to load a document"));
	}
	glFlush();
}
	
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

	m_viewport = new viewport(this);
	connect(this, SIGNAL(camera_changed(k3d::icamera* const)), m_viewport, SLOT(on_camera_changed(k3d::icamera* const)));
	connect(this, SIGNAL(render_engine_changed(k3d::gl::irender_engine* const)), m_viewport, SLOT(on_render_engine_changed(k3d::gl::irender_engine* const)));

	setCentralWidget(m_viewport);
}

void main_window::on_file_open()
{
	k3d::auto_ptr<k3d::idocument_importer> filter(k3d::create_plugin<k3d::idocument_importer>(k3d::classes::DocumentImporter()));
	if(!filter.get())
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

	if(!filter->read_file(*m_document, document_path))
	{
		QMessageBox::warning(this, _("Open K-3D Document:"), _("Error reading document."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	setWindowTitle(("K-3D Sample Qt User Interface - " + document_path.leaf().raw()).c_str());
	statusBar()->showMessage(("Loaded document " + document_path.leaf().raw()).c_str(), 0);

	m_cameras = k3d::find_nodes<k3d::icamera>(m_document->nodes());
	m_render_engines = k3d::find_nodes<k3d::gl::irender_engine>(m_document->nodes());

	m_camera_combo->clear();
	m_camera_combo->setEnabled(m_cameras.size());
	for(k3d::nodes_t::iterator camera = m_cameras.begin(); camera != m_cameras.end(); ++camera)
		m_camera_combo->addItem((*camera)->name().c_str());
	m_camera_combo->adjustSize();

	m_render_engine_combo->clear();
	m_render_engine_combo->setEnabled(m_render_engines.size());
	for(k3d::nodes_t::iterator render_engine = m_render_engines.begin(); render_engine != m_render_engines.end(); ++render_engine)
		m_render_engine_combo->addItem((*render_engine)->name().c_str());
	m_render_engine_combo->adjustSize();
	
	emit camera_changed(m_cameras.size() ? dynamic_cast<k3d::icamera*>(*m_cameras.begin()) : 0);
	emit render_engine_changed(m_render_engines.size() ? dynamic_cast<k3d::gl::irender_engine*>(*m_render_engines.begin()) : 0);
}

void main_window::on_camera_changed(int Index)
{
	k3d::nodes_t::iterator it = m_cameras.begin();
	std::advance(it, Index);
	emit camera_changed(dynamic_cast<k3d::icamera*>(*it));
}

void main_window::on_render_engine_changed(int Index)
{
	k3d::nodes_t::iterator it = m_render_engines.begin();
	std::advance(it, Index);
	emit render_engine_changed(dynamic_cast<k3d::gl::irender_engine*>(*it));
}
	
/////////////////////////////////////////////////////////////////////////////
// user_interface

void user_interface::get_command_line_arguments(boost::program_options::options_description& Description)
{
	Description.add_options()
		("--no-splash", "Disables the startup splash screen.")
		;
}

const k3d::iuser_interface_plugin::arguments_t user_interface::parse_startup_arguments(const arguments_t& Arguments, bool& Quit, bool& Error)
{
	// Keep track of whether to display a splash screen or not ...
	bool show_splash = true;

	// Qt expects to parse and modify argc / argv, so create some temporary storage for it to fiddle with ...
	static std::vector<char*> argv_buffer;
	argv_buffer.push_back("k3d");
	int argc = argv_buffer.size();
	char** argv = &argv_buffer[0];

	// We return any "unused" arguments ...
	arguments_t unused;

	// For each command-line argument ...
	for(arguments_t::const_iterator argument = Arguments.begin(); argument != Arguments.end(); ++argument)
	{
		if(argument->string_key == "no-splash")
		{
			show_splash = false;
		}
		else
		{
			unused.push_back(*argument);
		}
	}

	m_application.reset(new QApplication(argc, argv));
	
	if(show_splash)
	{
		QPixmap pixmap((k3d::share_path() / k3d::filesystem::generic_path("qtui/splash.png")).native_filesystem_string().c_str());
		m_splash_box.reset(new QSplashScreen(pixmap));
		m_splash_box->show();
	}

	return unused;
}

void user_interface::startup_message_handler(const std::string& Message)
{
	if(m_splash_box.get())
		m_splash_box->showMessage(Message.c_str());
}

void user_interface::display_user_interface()
{
	m_window.reset(new main_window(*m_application));
	m_window->show();
	m_splash_box.reset();
}

const k3d::iuser_interface_plugin::arguments_t user_interface::parse_runtime_arguments(const arguments_t& Arguments, bool& Quit, bool& Error)
{
	return Arguments;
}

void user_interface::start_event_loop()
{
	m_application->exec();
}

void user_interface::stop_event_loop()
{
	m_application->quit();
}

bool user_interface::batch_mode()
{
	return false;
}

void user_interface::browser_navigate(const std::string& URL)
{
}

void user_interface::message(const std::string& Message)
{
	QMessageBox::information(0, _("Information"), Message.c_str());
}

void user_interface::warning_message(const std::string& Message)
{
	QMessageBox::warning(0, _("Warning"), Message.c_str());
}

void user_interface::error_message(const std::string& Message)
{
	QMessageBox::critical(0, _("Error"), Message.c_str());
}

unsigned int user_interface::query_message(const std::string& Message, const unsigned int DefaultOption, const std::vector<std::string>& Options)
{
	return 0;
}

bool user_interface::tutorial_message(const std::string& Message)
{
	return false;
}

bool user_interface::get_file_path(const k3d::ipath_property::mode_t Mode, const std::string& Type, const std::string& Prompt, const k3d::filesystem::path& OldPath, k3d::filesystem::path& Result)
{
	return false;
}

bool user_interface::show(iunknown& Object)
{
	return false;
}

sigc::connection user_interface::get_timer(const double FrameRate, sigc::slot<void> Slot)
{
	return_val_if_fail(FrameRate != 0.0, sigc::connection());
	const unsigned int interval = static_cast<unsigned int>(1000.0 / FrameRate);

	timer_helper* const helper = new timer_helper(interval);
	return helper->m_signal.connect(Slot);
}

k3d::iplugin_factory& user_interface::factory()
{
	return get_factory();
}

k3d::iplugin_factory& user_interface::get_factory()
{
	static k3d::application_plugin_factory<user_interface,
		k3d::interface_list<k3d::iuser_interface_plugin> > factory(
		k3d::uuid(0xc7c652c7, 0xfb4349fe, 0x9c7d52c4, 0x9f9e9d3f),
		"QtUI",
		"Qt User Interface (QTUI)",
		"");

	return factory;
}

} // namespace libk3dqtui

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dqtui::user_interface::get_factory());
K3D_MODULE_END

