// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dtest
{

/////////////////////////////////////////////////////////////////////////////
// legacy_mesh_conversion

class legacy_mesh_conversion :
	public k3d::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	legacy_mesh_conversion(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::legacy::mesh legacy_mesh;
		legacy_mesh = Input;
		Output = legacy_mesh;
		k3d::validate(Output);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<legacy_mesh_conversion,
			k3d::interface_list<k3d::imesh_sink, k3d::interface_list<k3d::imesh_source> > > factory(
			k3d::uuid(0x50ba87cf, 0x2946be80, 0xbcf61fb2, 0x1089463a),
			"LegacyMeshConversion",
			_("Converts an input mesh to the legacy mesh format, then back, for regression testing"),
			"Test",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// legacy_mesh_conversion_factory

k3d::iplugin_factory& legacy_mesh_conversion_factory()
{
	return legacy_mesh_conversion::get_factory();
}

} // namespace libk3dtest

