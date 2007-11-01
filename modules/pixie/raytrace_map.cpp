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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/istream_ri.h>
#include <k3dsdk/itexture_ri.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace module
{

namespace pixie
{

class raytrace_map :
	public k3d::persistent<k3d::node>,
	public k3d::ri::itexture
{
	typedef k3d::persistent<k3d::node> base;

public:
	raytrace_map(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void setup_renderman_texture(k3d::inetwork_render_frame& Frame, k3d::ri::istream& Stream, k3d::ri::ishader_collection& Shaders)
	{
	}

	const k3d::filesystem::path renderman_texture_path(const k3d::ri::render_state& State)
	{
		return k3d::filesystem::generic_path("raytrace");
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<raytrace_map,
			k3d::interface_list<k3d::ri::itexture> > factory(
				k3d::uuid(0x896ac401, 0x5fae47ce, 0x92836627, 0x934dacdc),
				"PixieRaytraceMap",
				_("Dummy shadow map source for raytracing with Pixie"),
				"RenderMan Pixie",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// raytrace_map_factory

k3d::iplugin_factory& raytrace_map_factory()
{
	return raytrace_map::get_factory();
}

} // namespace pixie

} // namespace module

