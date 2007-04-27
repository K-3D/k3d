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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/i3d_2d_mapping.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <iterator>

namespace libk3dtexture
{

/////////////////////////////////////////////////////////////////////////////
// planar_map

class planar_map :
	public k3d::persistent<k3d::node>,
	public k3d::i3d_2d_mapping
{
	typedef k3d::persistent<k3d::node> base;

public:
	planar_map(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	const k3d::point2 map_3d_to_2d(const k3d::point3& Position, const k3d::bounding_box3& Bounds)
	{
		return_val_if_fail(Bounds.width() && Bounds.height() && Bounds.depth(), k3d::point2(0, 0));

		return k3d::point2(
			(Position[0] - Bounds.nx) / (Bounds.width()),
			(Bounds.py - Position[1]) / (Bounds.height()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<planar_map,
			k3d::interface_list<k3d::i3d_2d_mapping> > factory(
				k3d::uuid(0xd9148b60, 0x7f2a49f0, 0xa28e505a, 0xa3f4d4df),
				"PlanarMap",
				_("Planar projection map"),
				"Textures",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// planar_map_factory

k3d::iplugin_factory& planar_map_factory()
{
	return planar_map::get_factory();
}

} // namespace libk3dtexture

