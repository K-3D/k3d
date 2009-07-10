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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/inode.h>
#include <k3dsdk/ngui/node.h>
#include <k3dsdk/properties.h>

namespace k3d
{

namespace ngui
{

namespace node
{

void show(inode& Node)
{
	// Note: in the future, visibility should be handled using painter links, not properties.
	// See http://www.k-3d.org/wiki/Node_Visibility
	if(iproperty* const prop = property::get(Node, "viewport_visible"))
		property::set_internal_value(*prop, true);
	if(iproperty* const prop = property::get(Node, "render_final"))
		property::set_internal_value(*prop, true);
}

void hide(inode& Node)
{
	// Note: in the future, visibility should be handled using painter links, not properties.
	// See http://www.k-3d.org/wiki/Node_Visibility
	if(iproperty* const prop = property::get(Node, "viewport_visible"))
		property::set_internal_value(*prop, false);
	if(iproperty* const prop = property::get(Node, "render_final"))
		property::set_internal_value(*prop, false);
}

} // namespace node

} // namespace ngui

} // namespace k3d

