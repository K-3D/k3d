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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/itexture_ri.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/persistent.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// direct_texture_map

class direct_texture_map :
	public k3d::persistent<k3d::node>,
	public k3d::ri::itexture
{
	typedef k3d::persistent<k3d::node> base;

public:
	direct_texture_map(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Texture File")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::bitmaps()))
	{
	}

	~direct_texture_map()
	{
	}

	void setup_renderman_texture(k3d::inetwork_render_frame& Frame, k3d::ri::irender_engine& Engine, k3d::ri::ishader_collection& Shaders)
	{
	}

	const k3d::filesystem::path renderman_texture_path(const k3d::ri::render_state& State)
	{
		return m_file.pipeline_value();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<direct_texture_map,
			k3d::interface_list<k3d::ri::itexture> > factory(
				k3d::uuid(0x1792bf68, 0x9cbb46f6, 0xaf7d5967, 0x75664ebd),
				"RenderManDirectTextureMap",
				_("Links a RenderMan Texture Map directly to a file"),
				"RenderMan",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

/////////////////////////////////////////////////////////////////////////////
// direct_texture_map_factory

k3d::iplugin_factory& direct_texture_map_factory()
{
	return direct_texture_map::get_factory();
}

} // namespace libk3drenderman

