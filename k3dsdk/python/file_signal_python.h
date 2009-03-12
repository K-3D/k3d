#ifndef K3DSDK_PYTHON_FILE_SIGNAL_PYTHON_H
#define K3DSDK_PYTHON_FILE_SIGNAL_PYTHON_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/signal_system.h>
#include <k3dsdk/types.h>

namespace k3d
{

namespace python
{

/// Helper class that converts Python output to a signal.  Mainly useful when
/// redirecting Python stdout and stderr to interactive dialogs.
class file_signal
{
public:
	/// Connects a slot to a signal that will be called with Python output
	sigc::connection connect_output_signal(const sigc::slot<void, const string_t&>& Slot);

private:
	friend void write(file_signal&, const string_t&);
	sigc::signal<void, const string_t&> signal;
};

void define_class_file_signal();

} // namespace python

} // namespace k3d

#endif // !K3DSDK_PYTHON_FILE_SIGNAL_PYTHON_H
