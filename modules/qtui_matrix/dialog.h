#ifndef MODULES_QTUI_MATRIX_DIALOG_H
#define MODULES_QTUI_MATRIX_DIALOG_H

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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

#include "ui_dialog.h"
#include <k3dsdk/algebra.h>
#include <QDialog>

namespace module
{

namespace qtui
{

namespace matrix
{

/////////////////////////////////////////////////////////////////////////////
// dialog

/// Provides a dialog for k3d::matrix4 properties.
class dialog :
	public QDialog
{
	Q_OBJECT;

public:
	dialog(const k3d::matrix4& Value);

	k3d::matrix4 value;

private Q_SLOTS:
	void on_accepted();

private:
	Ui::QTUIMatrixDialog ui;
};

} // namespace matrix

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_MATRIX_DIALOG_H
