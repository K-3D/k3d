#ifndef K3DSDK_TIME_SOURCE_H
#define K3DSDK_TIME_SOURCE_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

namespace k3d
{

class idocument;
class inode;
class iproperty;

/// Returns the time source node for a document (could return NULL, if there is no source, or there's more than one)
inode* get_time_source(idocument& Document);
/// Returns the start time property for a document (could return NULL)
iproperty* get_start_time(idocument& Document);
/// Returns the end time property for a document (could return NULL)
iproperty* get_end_time(idocument& Document);
/// Returns the frame rate property for a document (could return NULL)
iproperty* get_frame_rate(idocument& Document);
/// Returns the time property for a document (could return NULL)
iproperty* get_time(idocument& Document);

} // namespace k3d

#endif // !K3DSDK_TIME_SOURCE_H

