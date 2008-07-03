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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "ipipeline.h"

namespace k3d
{

class istate_recorder;

/////////////////////////////////////////////////////////////////////////////
// pipeline

/// Provides a concrete implementation of ipipeline that can be used to manage arbitrary connections between properties
class pipeline :
	public ipipeline,
	public sigc::trackable
{
public:
	/// Initialize the pipeline with (optional) undo/redo suppport
	pipeline(istate_recorder* StateRecorder = 0);
	~pipeline();

	// ipipeline methods
	void set_dependencies(dependencies_t& Dependencies, ihint* Hint = 0);
	iproperty* dependency(iproperty& Property);
	const dependencies_t& dependencies();
	dependency_signal_t& dependency_signal();

	/// Remove all pipeline connections (not undoable)
	void clear();
	/// Makes a connection between two properties
	void connect(iproperty& From, iproperty& To);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace k3d

