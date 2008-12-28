#ifndef K3DSDK_IFILE_CHANGE_NOTIFIER_H
#define K3DSDK_IFILE_CHANGE_NOTIFIER_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file ifile_change_notifier.h
	\author Tim Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
	\created Dec 17, 2008
*/

#include "iunknown.h"
#include "signal_system.h"
#include "types.h"

namespace k3d
{

namespace filesystem
{

class path;

}

class ifile_change_notifier :
	public virtual iunknown
{
public:
	virtual ~ifile_change_notifier() {}
	
	/// Call a slot whenever given filesystem path is modified.
	/**
	* Note that we are watching the
	* path, not an inode, so it isn't an error to specify a path for a nonexistent file.
	* The slot will be called when a file is created / modified / renamed / deleted at that
	* location.  Returns a nonzero watch identifier that is used to cancel the watch later-on,
	* or 0 if there is an error.
	*/
	virtual uint_t watch_file(const filesystem::path& Path, const sigc::slot<void>& Slot) = 0;

	/// Stop watching the given path.
	virtual void unwatch_file(const uint_t WatchID) = 0;

	/// Returns true if there are any notification events waiting.
	virtual const bool_t pending_changes() = 0;

	/// Handles the next notification event (calls one slot), blocking if there are no events pending.
	virtual void notify_change() = 0;
  
protected:
	ifile_change_notifier() {}
	ifile_change_notifier(const ifile_change_notifier&) {}
	ifile_change_notifier& operator=(const ifile_change_notifier&) {return *this;}
};

} // namespace k3d

#endif // !K3DSDK_IFILE_CHANGE_NOTIFIER_H
