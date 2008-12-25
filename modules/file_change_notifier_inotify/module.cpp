// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>
#include <k3d-platform-config.h>

namespace module
{

namespace file_change_notifier
{

namespace inotify
{
#ifndef FILE_CHANGE_NOTIFIER_THREAD
extern k3d::iplugin_factory& file_change_notifier_poll_factory();
#else
extern k3d::iplugin_factory& file_change_notifier_thread_factory();
#endif

} // namespace inotify

} // namespace file_change_notifier

} // namespace module

K3D_MODULE_START(Registry)
#ifndef FILE_CHANGE_NOTIFIER_THREAD
	Registry.register_factory(module::file_change_notifier::inotify::file_change_notifier_poll_factory());
#else
	Registry.register_factory(module::file_change_notifier::inotify::file_change_notifier_thread_factory());
#endif
K3D_MODULE_END
