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

#include "mode.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/types.h>

#include <QGraphicsScene>
#include <QInputDialog>
#include <QMenu>
#include <QTimer>
#include <QToolButton>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

namespace module
{

namespace qtui
{

namespace programmable
{

/////////////////////////////////////////////////////////////////////////////
// mode

mode::mode() :
	scene(0),
	program(k3d::resource::get_string("/QTUIProgrammableMode/default_script.js").c_str())
{
}

void mode::enable(QGraphicsScene& Scene)
{
	if(scene)
		disconnect(scene, 0, this, 0);

	scene = &Scene;

	connect(scene, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(on_scene_rect_changed(const QRectF&)));

	on_reload();
}

void mode::on_load()
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << std::endl;
}

void mode::on_edit()
{
	k3d::bool_t ok = false;
	const QString new_program = QInputDialog::getText(0, tr("Edit program"), tr("Program:"), QLineEdit::Normal, program, &ok);
	if(!ok)
		return;

	program = new_program;
	
	QTimer::singleShot(0, this, SLOT(on_reload()));
}

void mode::on_save()
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << std::endl;
}

void mode::on_reload()
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << std::endl;

	QToolButton* const edit_menu_button = new QToolButton();
	edit_menu_button->setText("Edit");
	edit_menu_button->setPopupMode(QToolButton::MenuButtonPopup);
	connect(edit_menu_button, SIGNAL(clicked()), this, SLOT(on_edit()));

	QMenu* const edit_menu = new QMenu(edit_menu_button);
	connect(edit_menu->addAction("Load"), SIGNAL(activated()), this, SLOT(on_load()));
	connect(edit_menu->addAction("Edit"), SIGNAL(activated()), this, SLOT(on_edit()));
	connect(edit_menu->addAction("Save"), SIGNAL(activated()), this, SLOT(on_save()));

	edit_menu_button->setMenu(edit_menu);

	edit_menu_proxy.reset(scene->addWidget(edit_menu_button));

	script_engine.reset(new QScriptEngine());
	script_engine->importExtension("qt");
	script_engine->importExtension("qt.core");
	script_engine->importExtension("qt.gui");
	script_engine->globalObject().setProperty("scene", script_engine->newQObject(scene));
	QScriptValue result = script_engine->evaluate(program);
	if(result.isError())
		k3d::log() << error << result.toString().toAscii().data() << std::endl;

	on_scene_rect_changed(scene->sceneRect());
}

void mode::on_scene_rect_changed(const QRectF& Rect)
{
	if(!edit_menu_proxy)
		return;

	const QRectF proxy_rect = edit_menu_proxy->geometry();
	edit_menu_proxy->setPos(Rect.right() - proxy_rect.width() - 20, Rect.bottom() - proxy_rect.height() - 20);
}

k3d::iplugin_factory& mode::get_factory()
{
	static k3d::application_plugin_factory<mode> factory(
		k3d::uuid(0xccadd74f, 0xf44ad6e9, 0x12eeefa4, 0xde50723a),
		"QTUIProgrammableMode",
		_("Provides an edit-mode with programmable behavior, intended mainly as a development and debugging tool."),
		"QTUI Mode",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "mode"));

	return factory;
}

} // namespace programmable

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::programmable::mode::get_factory());
K3D_MODULE_END

