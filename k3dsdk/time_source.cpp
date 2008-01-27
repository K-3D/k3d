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

#include "idocument.h"
#include "nodes.h"
#include "properties.h"
#include "time_source.h"

namespace k3d
{

inode* get_time_source(idocument& Document)
{
	const nodes_t nodes = find_nodes(Document.nodes(), "TimeSource");
	return (1 == nodes.size()) ? *nodes.begin() : 0;
}

iproperty* get_start_time(idocument& Document)
{
	inode* const object = get_time_source(Document);
	return object ? property::get<double>(*object, "start_time") : 0;
}

iproperty* get_end_time(idocument& Document)
{
	inode* const object = get_time_source(Document);
	return object ? property::get<double>(*object, "end_time") : 0;
}

iproperty* get_frame_rate(idocument& Document)
{
	inode* const object = get_time_source(Document);
	return object ? property::get<double>(*object, "frame_rate") : 0;
}

iproperty* get_time(idocument& Document)
{
	inode* const object = get_time_source(Document);
	return object ? property::get<double>(*object, "time") : 0;
}

} // namespace k3d

