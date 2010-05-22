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

#include <k3dsdk/qtui/application.h>
#include <k3dsdk/qtui/document_widget.h>

#include <QWidget>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// document_widget

document_widget::document_widget(QWidget& Owner, idocument& Document) :
	owner(Owner),
	owning_document(Document)
{
	connect(&application::instance(), SIGNAL(closing()), this, SLOT(application_closing()));
}

idocument& document_widget::document()
{
	return owning_document;
}

void document_widget::application_closing()
{
	owner.close();
}

} // namespace qtui

} // namespace k3d

