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
#include "scalar_texture.h"
#include "texture2.h"

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
// wrap_names

static const k3d::ilist_property<std::string>::values_t& wrap_names()
{
	static k3d::ilist_property<std::string>::values_t values;
	if(values.empty())
	{
		values.push_back("repeat");
		values.push_back("black");
		values.push_back("clamp");
	}
	return values;
}

/////////////////////////////////////////////////////////////////////////////
// image_texture

class image_texture :
	public texture2,
	public color_texture,
	public scalar_texture
{
	typedef texture2 base;

public:
	image_texture(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Path to a bitmap image.")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("texture")),
		m_wrap(init_owner(*this) + init_name("wrap") + init_label(_("Wrap")) + init_description(_("Specifies how to wrap the texture.")) + init_value(k3d::string_t("repeat")) + init_values(wrap_names())),
		m_max_anisotropy(init_owner(*this) + init_name("max_anisotropy") + init_label(_("Max Anisotropy")) + init_description(_("Eccentricity of the ellipse used for EWA filtering.")) + init_value(8.0)),
		m_trilinear(init_owner(*this) + init_name("trilinear") + init_label(_("Trilinear")) + init_description(_("Use faster / lower-quality trilinear filtering.")) + init_value(false))
	{
	}

	void setup_color_texture(const k3d::string_t& Name, std::ostream& Stream)
	{
		setup(Name, "color", Stream);
	}

	void setup_scalar_texture(const k3d::string_t& Name, std::ostream& Stream)
	{
		setup(Name, "float", Stream);
	}

	void setup(const k3d::string_t& Name, const k3d::string_t& Type, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "Texture \"" << Name << "\" \"" << Type << "\" \"imagemap\"";
		setup_texture2(Stream);
		Stream << " \"string filename\" \"" << m_file.pipeline_value().native_filesystem_string() << "\"";
		Stream << " \"string wrap\" \"" << m_wrap.pipeline_value() << "\"";
		Stream << " \"float maxanisotropy\" \"" << m_max_anisotropy.pipeline_value() << "\"";
		Stream << " \"bool trilinear\" \"" << m_trilinear.pipeline_value() << "\"";
		Stream << "\n"; 
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<image_texture,
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
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_wrap;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_max_anisotropy;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_trilinear;
};

k3d::iplugin_factory& image_texture_factory()
{
	return image_texture::get_factory();
}

} // namespace luxrender

} // namespace module

