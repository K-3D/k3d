// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief An object that converts a scalar to a string using strftime()
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dcore
{

class null :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;
public:
	null(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<null> factory(
			k3d::uuid(0xe1cbc7e2, 0xf4b44daf, 0x9176080c, 0x80d2c4ab),
			"Null",
			_("Empty node"),
			"Utility",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

k3d::iplugin_factory& null_factory()
{
	return null::get_factory();
}

} //namespace libk3dcore

