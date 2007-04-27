// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Provides singleton access to the global K-3D object
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iapplication.h"
#include "register_application.h"

#include <cassert>

namespace
{

k3d::iapplication* g_application = 0;

} // namespace

namespace k3d
{

void register_application(k3d::iapplication& Application)
{
	assert(!g_application);
	g_application = &Application;
}

iapplication& application()
{
	assert(g_application);
	return *g_application;
}

} // namespace k3d

