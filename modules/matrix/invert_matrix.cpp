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

/** \file
	\author Tim Shead (tshead@k-3d.com)
	\author Joaquín Duo (joaduo at lugmen org ar)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace matrix
{

/////////////////////////////////////////////////////////////////////////////
// invert_matrix

class invert_matrix :
	public k3d::transformable<k3d::node >
{
	typedef k3d::transformable<k3d::node > base;

public:
	invert_matrix(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<invert_matrix,
			k3d::interface_list<k3d::itransform_sink, k3d::interface_list<k3d::itransform_source> > > factory(
			k3d::uuid(0x9582e25c, 0xcf41e6fb, 0xc796fd94, 0x35a89d33),
			"InvertMatrix",
			_("Inverts a matrix mathematically speaking"),
			"Matrix",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	void on_update_matrix(const k3d::matrix4& Input, k3d::matrix4& Output)
	{
		Output = k3d::inverse(Input);
	}
};

/////////////////////////////////////////////////////////////////////////////
// invert_matrix_factory

k3d::iplugin_factory& invert_matrix_factory()
{
	return invert_matrix::get_factory();
}

} // namespace matrix

} // namespace module


