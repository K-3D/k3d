// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "asynchronous_update.h"
#include "utility.h"

#include <glibmm/main.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// asynchronous_update

asynchronous_update::asynchronous_update() :
	m_block_updates(false)
{
}
	
asynchronous_update::~asynchronous_update()
{
}

void asynchronous_update::schedule_update()
{
	if(!m_idle_connection.connected())
		m_idle_connection = Glib::signal_idle().connect(sigc::mem_fun(*this, &asynchronous_update::on_idle));
}

void asynchronous_update::block_updates()
{
	m_block_updates = true;
}

void asynchronous_update::unblock_updates()
{
	m_block_updates = false;
}

bool asynchronous_update::on_idle()
{
	if(!m_block_updates)
		on_update();

	m_idle_connection.disconnect();

	return false;
}

} // namespace ngui

} // namespace k3d

