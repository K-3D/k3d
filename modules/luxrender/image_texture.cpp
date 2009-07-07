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
// wrap_values

static const k3d::ilist_property<std::string>::values_t& wrap_values()
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
// filtertype_values

static const k3d::ilist_property<std::string>::values_t& filtertype_values()
{
	static k3d::ilist_property<std::string>::values_t values;
	if(values.empty())
	{
		values.push_back("bilinear");
		values.push_back("mipmap_trilinear");
		values.push_back("mapmap_ewa");
		values.push_back("nearest");
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
		m_wrap(init_owner(*this) + init_name("wrap") + init_label(_("Wrap")) + init_description(_("Specifies how to wrap the texture.")) + init_value(k3d::string_t("repeat")) + init_values(wrap_values())),
		m_filtertype(init_owner(*this) + init_name("filtertype") + init_label(_("Filter")) + init_description(_("Specifies how to filter the texture.")) + init_value(k3d::string_t("bilinear")) + init_values(filtertype_values())),
		m_gain(init_owner(*this) + init_name("gain") + init_label(_("Gain")) + init_description(_("Applies a gain function to the texture.")) + init_value(1.0)),
		m_gamma(init_owner(*this) + init_name("gamma") + init_label(_("Gamma")) + init_description(_("Applies a gamma function to the texture.")) + init_value(2.2))
	{
	}

	void on_setup(name_map& TextureNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "Texture \"" << Name << "\" \"color\" \"imagemap\"\n" << k3d::push_indent;
		setup_texture2(Stream);
		Stream << k3d::standard_indent << "\"string filename\" [\"" << m_file.pipeline_value().native_filesystem_string() << "\"]\n";
		Stream << k3d::standard_indent << "\"string wrap\" [\"" << m_wrap.pipeline_value() << "\"]\n";
		Stream << k3d::standard_indent << "\"string filtertype\" [\"" << m_filtertype.pipeline_value() << "\"]\n";
		Stream << k3d::standard_indent << "\"float gain\" [" << m_gain.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"float gamma\" [" << m_gamma.pipeline_value() << "]\n";
		Stream << k3d::pop_indent; 
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
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_filtertype;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gain;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gamma;
};

k3d::iplugin_factory& image_texture_factory()
{
	return image_texture::get_factory();
}

} // namespace luxrender

} // namespace module

