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

#include "dialog.h"

namespace module
{

namespace qtui
{

namespace matrix
{

/////////////////////////////////////////////////////////////////////////////
// dialog

dialog::dialog(const k3d::matrix4& Value) :
	value(Value)
{
	ui.setupUi(this);

	for(int i = 0; i != 4; ++i)
	{
		for(int j = 0; j != 4; ++j)
		{
			ui.matrix->setItem(i, j, new QTableWidgetItem(QString::number(value[i][j])));
		}
	}

	QObject::connect(this, SIGNAL(accepted()), this, SLOT(on_accepted()));
}

void dialog::on_accepted()
{
	for(int i = 0; i != 4; ++i)
	{
		for(int j = 0; j != 4; ++j)
		{
			value[i][j] = ui.matrix->item(i, j)->text().toDouble();
		}
	}
}

} // namespace matrix

} // namespace qtui

} // namespace module

