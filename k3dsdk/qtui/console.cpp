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

#include <k3dsdk/log.h>
#include <k3dsdk/qtui/console.h>

#include <boost/algorithm/string.hpp>

#include <QScrollBar>

namespace k3d
{

namespace qtui
{

namespace console
{

/////////////////////////////////////////////////////////////////////////////
// widget::implementation

class widget::implementation
{
public:
	std::vector<QString> history;
	size_t history_index;
	QString command;
	int edit_begin;
	int edit_end;
};

/////////////////////////////////////////////////////////////////////////////
// widget

widget::widget(QWidget* Parent) :
	base(Parent),
	internal(new implementation())
{
	internal->history_index = 0;
	internal->edit_begin = 0;
	internal->edit_end = 0;

	QObject::connect(this, SIGNAL(textChanged()), this, SLOT(on_text_changed()));
}

widget::~widget()
{
	delete internal;
}

void widget::print_text(const QString& String)
{
	appendPlainText(String);
	print();
}

void widget::print_html(const QString& HTML)
{
	appendHtml(HTML);
	print();
}

void widget::scroll_to_end()
{
	horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());
	verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void widget::on_text_changed()
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::End);

	internal->edit_end = cursor.position();
	internal->edit_begin = std::min(internal->edit_begin, internal->edit_end);
}

void widget::print()
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::End);

	internal->edit_end = cursor.position();
	internal->edit_begin = cursor.position();

	scroll_to_end();
}

void widget::showEvent(QShowEvent* Event)
{
	scroll_to_end();
}

void widget::keyPressEvent(QKeyEvent* Event)
{
	QTextCursor cursor = textCursor();
	if(cursor.anchor() < internal->edit_begin || cursor.position() < internal->edit_begin)
	{
		cursor.movePosition(QTextCursor::End);
		setTextCursor(cursor);
		return;
	}
	else
	{
		switch(Event->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			{
				const QString command = toPlainText().mid(internal->edit_begin, internal->edit_end - internal->edit_begin);

				if(!command.isEmpty() && (internal->history.empty() || command != internal->history.back()))
					internal->history.push_back(command);
				internal->history_index = internal->history.size();

				Q_EMIT execute(command);

				cursor.movePosition(QTextCursor::End);
				setTextCursor(cursor);
				return;
			}

			case Qt::Key_Backspace:
			case Qt::Key_Left:
			{
				if(cursor.position() > internal->edit_begin)
					base::keyPressEvent(Event);
				return;
			}

			case Qt::Key_Up:
			{
				if(internal->history_index)
				{
					if(internal->history_index == internal->history.size())
						internal->command = toPlainText().mid(internal->edit_begin, internal->edit_end - internal->edit_begin);

					internal->history_index -= 1;

					cursor.setPosition(internal->edit_begin);
					cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, internal->edit_end - internal->edit_begin);
					cursor.insertText(internal->history[internal->history_index]);

					cursor.movePosition(QTextCursor::End);
					setTextCursor(cursor);
				}

				return;
			}

			case Qt::Key_Down:
			{
				if(internal->history_index < internal->history.size())
				{
					internal->history_index += 1;

					cursor.setPosition(internal->edit_begin);
					cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, internal->edit_end - internal->edit_begin);
					cursor.insertText(internal->history_index < internal->history.size()
						? internal->history[internal->history_index]
						: internal->command);

					cursor.movePosition(QTextCursor::End);
					setTextCursor(cursor);
				}

				return;
			}

			case Qt::Key_Home:
			{
				cursor.setPosition(internal->edit_begin);
				setTextCursor(cursor);
				return;
			}

			case Qt::Key_End:
			{
				cursor.setPosition(internal->edit_end);
				setTextCursor(cursor);
				return;
			}
		}
	}

	base::keyPressEvent(Event);
}

} // namespace console

} // namespace qtui

} // namespace k3d

