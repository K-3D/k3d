#ifndef MODULES_QTUI_AQSIS_THREAD_H
#define MODULES_QTUI_AQSIS_THREAD_H

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

#include <QThread>

namespace Aqsis { class CqRegion; }
namespace Aqsis { class IqChannelBuffer; }

class QRect;

namespace module
{

namespace qtui
{

namespace aqsis
{

class thread :
	public QThread
{
	Q_OBJECT;

Q_SIGNALS:
	void open_display(int Width, int Height);
	void display_bucket(const QRect& Region, const Aqsis::IqChannelBuffer* Buffer);

private:
	virtual void run();
	void on_open_display(int Width, int Height);
	void on_display_bucket(const Aqsis::CqRegion& Region, const Aqsis::IqChannelBuffer* Buffer);
};

} // namespace aqsis

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_AQSIS_THREAD_H
