#ifndef LIBK3DBITMAP_SIMPLE_BITMAP_MODIFIER_H
#define LIBK3DBITMAP_SIMPLE_BITMAP_MODIFIER_H

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

#include <k3dsdk/bitmap_modifier.h>
#include <k3dsdk/node.h>

namespace module
{

namespace bitmap
{

class simple_modifier :
	public k3d::node,
	public k3d::bitmap_modifier<simple_modifier>
{
	typedef k3d::node base;

public:
	simple_modifier(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

private:
	virtual void on_create_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		Output.recreate(Input.width(), Input.height());
	}
};

} // namespace bitmap

} // namespace module

#endif // !LIBK3DBITMAP_SIMPLE_BITMAP_MODIFIER_H

