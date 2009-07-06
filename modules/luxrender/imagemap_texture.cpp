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
	\author Tim Shead <tshead@k-3d.com>
*/

#include "color_texture.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// imagemap_texture

class imagemap_texture :
	public k3d::node,
	public color_texture
{
	typedef k3d::node base;

public:
	imagemap_texture(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Path to a bitmap image.")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("texture"))
	{
	}

	void setup(const k3d::string_t& Name, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "Texture \"" << Name << "\" \"color\" \"imagemap\"";
		Stream << " \"string filename\" \"" << m_file.pipeline_value().native_filesystem_string() << "\"";
		Stream << "\n"; 
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<imagemap_texture,
			k3d::interface_list<k3d::itexture> > factory(
			k3d::uuid(0x4c6d4e1b, 0x46488112, 0xc82d33a9, 0x0518b7df),
			"LuxRenderImagemapTexture",
			_("LuxRender Imagemap Texture"),
			"LuxRender Texture",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, with_serialization) m_file;
};

k3d::iplugin_factory& imagemap_texture_factory()
{
	return imagemap_texture::get_factory();
}

} // namespace luxrender

} // namespace module

