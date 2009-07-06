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

#include "scalar_texture.h"
#include "texture2.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// scalar_imagemap_texture

class scalar_imagemap_texture :
	public texture2,
	public scalar_texture
{
	typedef texture2 base;

public:
	scalar_imagemap_texture(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Path to a bitmap image.")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("texture"))
	{
	}

	void setup(const k3d::string_t& Name, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "Texture \"" << Name << "\" \"float\" \"imagemap\"";
		texture2_setup(Stream);
		Stream << " \"string filename\" \"" << m_file.pipeline_value().native_filesystem_string() << "\"";
		Stream << "\n"; 
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scalar_imagemap_texture,
			k3d::interface_list<k3d::itexture> > factory(
			k3d::uuid(0xd887e90e, 0x1e449038, 0x2b7f1d8c, 0xf6e57a15),
			"LuxRenderScalarImagemapTexture",
			_("LuxRender Scalar Imagemap Texture"),
			"LuxRender Texture",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, with_serialization) m_file;
};

k3d::iplugin_factory& scalar_imagemap_texture_factory()
{
	return scalar_imagemap_texture::get_factory();
}

} // namespace luxrender

} // namespace module

